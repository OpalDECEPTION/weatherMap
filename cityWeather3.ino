#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5        // GPIO pin connected to DIN 5
#define NUMPIXELS 9  // Number of NeoPixels

// Replace with your network credentials
const char* ssid = "EngineeringSubNet";
const char* password = "password";

int temp_f;
float avgTemp;

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

  pixels.begin();
  pixels.clear();
  pixels.show();
}


float getWeather(const String& city_name) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=imperial";

    Serial.printf("\nFetching weather for %s...\n", city_name.c_str());
    http.begin(weatherUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();

      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        http.end();
        return NAN;  // return invalid value
      } else {
        float temp_f = doc["main"]["temp"];
        Serial.printf("%s Temperature: %.1f°F\n", city_name.c_str(), temp_f);
        http.end();
        return temp_f;  // return temperature
      }
    } else {
      Serial.printf("[HTTP] GET failed for %s, error: %s\n", city_name.c_str(), http.errorToString(httpResponseCode).c_str());
      http.end();
      return NAN;
    }
  } else {
    Serial.println("WiFi Disconnected");
    return NAN;
  }
}


void showLED(float cityTemp, float avgTemp, int pixelIndex, const char* cityName) {
  float diff = cityTemp - avgTemp;
  int calcTemp = (int)round(diff * 50);
  calcTemp = constrain(calcTemp, -255, 255);

  if (cityTemp >= avgTemp) {
    pixels.setPixelColor(pixelIndex, pixels.Color(255 - calcTemp, 255, 0));
  } else {
    pixels.setPixelColor(pixelIndex, pixels.Color(255 - abs(calcTemp), 0, 20+(abs(calcTemp))/2));
  }

  Serial.printf("%s LED → calcTemp=%d\n", cityName, calcTemp);
}


void loop() {
  float temp_wor = getWeather("Worcester,MA,US");
  float temp_nan = getWeather("Nantucket,MA,US");
  float temp_ply = getWeather("Plymouth,MA,US");
  float temp_slm = getWeather("Salem,MA,US");
  float temp_sgf = getWeather("Springfield,MA,US");
  float temp_tau = getWeather("Taunton,MA,US");
  float temp_pit = getWeather("Pittsfield,MA,US");
  float temp_bos = getWeather("Boston,MA,US");
  float temp_bns = getWeather("Barnstable,MA,US");

  avgTemp = (temp_wor + temp_nan + temp_ply + temp_slm + temp_sgf + temp_tau + temp_pit + temp_bos + temp_bns) / 9;

  Serial.print("Avg Temp = ");
  Serial.println(avgTemp);

  
  showLED(temp_nan, avgTemp, 0, "Nantucket,MA,US");
  showLED(temp_bns, avgTemp, 1, "Barnstable,MA,US");
  showLED(temp_ply, avgTemp, 2, "Plymouth,MA,US");
  showLED(temp_tau, avgTemp, 3, "Taunton,MA,US");
  showLED(temp_bos, avgTemp, 4, "Boston,MA,US");
  showLED(temp_slm, avgTemp, 5, "Salem,MA,US");
  showLED(temp_wor, avgTemp, 6, "Worcester,MA,US");
  showLED(temp_sgf, avgTemp, 7, "Springfield,MA,US");
  showLED(temp_pit, avgTemp, 8, "Pittsfield,MA,US");

  pixels.show();

  delay(5 * 60 * 1000);
}
