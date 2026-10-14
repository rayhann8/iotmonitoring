#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// WiFi Wokwi
const char* ssid = "LABTJP1";
const char* password = "123454321";

// Supabase
const char* endpoint = "https://hastxlaqrxmxsuyzxmgg.supabase.co/rest/v1/sensor_data";

// Publishable Key
const char* supabaseKey = "sb_publishable_EfcppTA0c0TxEp7nNNyOOw_q5GTRL7y";

void setup() {
  Serial.begin(115200);

  dht.begin();

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Terhubung!");
  Serial.println(WiFi.localIP());
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca DHT11!");
    delay(1000);
    return;
  }

  Serial.println("========================");

  Serial.print("Suhu       : ");
  Serial.print(temperature, 1);
  Serial.println(" °C");

  Serial.print("Kelembapan : ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.setTimeout(15000);

    if (!http.begin(client, endpoint)) {
      Serial.println("Gagal membuka koneksi HTTPS!");
      delay(5000);
      return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabaseKey);
    http.addHeader("Authorization", String("Bearer ") + supabaseKey);

    // Tidak meminta Supabase mengembalikan row
    http.addHeader("Prefer", "return=minimal");

    String jsonData = "{";
    jsonData += "\"temperature\":" + String(temperature, 1) + ",";
    jsonData += "\"humidity\":" + String(humidity, 1);
    jsonData += "}";

    Serial.println("Mengirim ke Supabase...");
    Serial.println(jsonData);

    int httpCode = http.POST(jsonData);

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    Serial.println("Response:");
    Serial.println(http.getString());

    http.end();
  }

  delay(5000);
}