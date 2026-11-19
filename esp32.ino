#include <WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

// --- Konfigurasi WiFi ---
const char* ssid = "AMANAH";
const char* password = "seloarum165";

// --- Konfigurasi ThingSpeak ---
unsigned long myChannelNumber = 3571364;                     
const char * myWriteAPIKey = "RJSAO61BOYU5TPZK";             

// --- Konfigurasi Sensor ---
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Inisialisasi Sensor DHT22
  dht.begin();
  Serial.println(F("Sensor DHT22 siap"));

  // --- TRIK MENCEGAH BROWNOUT ---
  WiFi.setTxPower(WIFI_POWER_8_5dBm); 
  
  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");

  // Inisialisasi ThingSpeak
  ThingSpeak.begin(client); 
}

void loop() {
  // 1. Baca data dari sensor DHT22
  float suhu = dht.readTemperature();
  float kelembaban = dht.readHumidity();

  // 2. Validasi pembacaan DHT22
  if (isnan(suhu) || isnan(kelembaban)) {
    Serial.println(F("Gagal membaca dari sensor DHT!"));
    delay(2000);
    return; 
  }

  // 3. Tampilkan di Serial Monitor
  Serial.print("Suhu: "); Serial.print(suhu); Serial.print(" °C, ");
  Serial.print("Kelembaban: "); Serial.print(kelembaban); Serial.println(" %");

  // 4. Siapkan data untuk ThingSpeak (Field 1=Suhu, Field 2=Kelembaban)
  ThingSpeak.setField(1, suhu);
  ThingSpeak.setField(2, kelembaban);

  // 5. Kirim data ke ThingSpeak
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (responseCode == 200) {
    Serial.println("Berhasil mengirim data ke ThingSpeak!");
  } else {
    Serial.println("Gagal mengirim data. Kode error HTTP: " + String(responseCode));
  }

  // Jeda 20 detik (ThingSpeak gratis butuh minimal 15 detik antar pengiriman)
  delay(20000); 
}