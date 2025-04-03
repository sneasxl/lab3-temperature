#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

const char* ssid = "Lag-o-less";
const char* password = "7RHRapZi0";

const char* serverName = "http://ec2-18-218-218-206.us-east-2.compute.amazonaws.com:5000/sensordata";

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Wire.begin(); 
  delay(100);  
  
  Serial.println("Initializing AHT20 sensor...");
  if (!aht.begin()) {
    Serial.println("Failed to initialize AHT20 sensor!");
    while (1) { delay(100); }
  }
  Serial.println("AHT20 sensor initialized successfully!");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // Read sensor data
  
  float temperature = temp.temperature;
  float humValue = humidity.relative_humidity;
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humValue);
  Serial.println(" %");

  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    
    String postData = "temperature=" + String(temperature) + "&humidity=" + String(humValue);
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Server response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on POST: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
  
  delay(5000);
}
