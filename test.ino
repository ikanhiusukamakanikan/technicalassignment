#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// WiFi credentials
const char* ssid = "TheHill";
const char* password = "thehill2023";

// Server URL
const char* serverName = "http://192.168.1.218:2207";

// Define the DHT sensor type and pin
#define DHTTYPE DHT11
#define DHT_PIN 27

DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi network
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Delay between sensor readings
  delay(2000);

  // Read temperature and humidity from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check for failed readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print sensor values to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Create JSON payload
  String jsonPayload = "{\"temperature\":";
  jsonPayload += temperature;
  jsonPayload += ",\"humidity\":";
  jsonPayload += humidity;
  jsonPayload += "}";

  // Send data to the server if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
