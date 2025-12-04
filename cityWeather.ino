#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5        // GPIO pin connected to DIN 5
#define NUMPIXELS 2  // Number of NeoPixels


// Replace with your network credentials
const char* ssid = "EngineeringSubNet";
const char* password = "password";

// OpenWeatherMap API key
const String api_key = "42792055e019d1867f4d310c14beb28b";
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Wait a second before making the API call
  delay(1000);
  pixels.begin();
}

void getWeather(const String& city_name) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=imperial";


    Serial.printf("\nFetching weather for %s...\n", city_name.c_str());
    http.begin(weatherUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();

      StaticJsonDocument<512> doc;  // bumped size for safety
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      } else {
        float temp_f = doc["main"]["temp"];
        String description = doc["weather"][0]["main"];
        Serial.printf("%s Temperature: %.1f°F\n", city_name.c_str(), temp_f);
        Serial.printf("%s Sky: %s\n", city_name.c_str(), description.c_str());
      }
    } else {
      Serial.printf("[HTTP] GET failed for %s, error: %s\n",city_name.c_str(), http.errorToString(httpResponseCode).c_str());
       
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  // Fetch weather for each city
  getWeather("Boston");
  getWeather("Worcester");
  getWeather("Nantucket");
  getWeather("Plymouth");
  getWeather("Salem,MA,US");
  getWeather("Springfield,MA,US");
  getWeather("Taunton");
  getWeather("Barnstable");
  getWeather("Pittsfield");

  // Fetch new data every 5 minutes
  delay(5 * 60 * 1000);
}
