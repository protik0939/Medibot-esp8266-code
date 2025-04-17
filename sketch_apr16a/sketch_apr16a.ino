#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>

const char* ssid = "Cool Dude";
const char* password = "SiyumRafiJarif";
const char* serverUrl = "https://medibot-backend-nine.vercel.app/api/notify/";

WiFiClientSecure client;
SoftwareSerial espSerial(D1, D2); // RX, TX (adjust pins as per wiring)

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to WiFi!");
  Serial.println(WiFi.localIP());

  client.setInsecure(); // For testing only (skip SSL cert)
}

void loop() {
  if (espSerial.available()) {
    String input = espSerial.readStringUntil('\n');
    input.trim();

    if (input == "NOTIFY") {
      sendNotification();
    }
  }
}

void sendNotification() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    String json = "{\"message\":\"Medicine delivered!\"}";
    Serial.println("📤 Sending JSON: " + json);

    int responseCode = http.POST(json);

    if (responseCode > 0) {
      Serial.printf("✅ HTTP %d\n", responseCode);
      Serial.println("🔁 Response: " + http.getString());
    } else {
      Serial.printf("❌ Error: %s\n", http.errorToString(responseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("❌ WiFi Disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
  }
}
