#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5        // GPIO pin connected to DIN 5
#define NUMPIXELS 8  // Number of NeoPixels

// Replace with your network credentials
const char* ssid = "EngineeringSubNet";
const char* password = "password";

// OpenWeatherMap API key
const String api_key = "42792055e019d1867f4d310c14beb28b";
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

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
  float temp_f = -999;

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
      } else {
        temp_f = doc["main"]["temp"];
        Serial.printf("%s Temperature: %d°F\n", city_name.c_str(), (int)round(temp_f));
      }
    } else {
      Serial.printf("[HTTP] GET failed for %s, error: %s\n", city_name.c_str(), http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  return temp_f;
}

void loop() {
  // Fetch temps for 8 cities
  float temp_bos = getWeather("Boston");
  float temp_wor = getWeather("Worcester");
  float temp_nan = getWeather("Nantucket");
  float temp_ply = getWeather("Plymouth");
  float temp_slm = getWeather("Salem,MA,US");
  float temp_sgf = getWeather("Springfield,MA,US");
  float temp_tan = getWeather("Taunton,MA,US");
  float temp_pit = getWeather("Pittsfield");

  // Compute average
  float avgTemp = (temp_bos + temp_wor + temp_nan + temp_ply + temp_slm + temp_sgf + temp_tan + temp_pit) / 8;
  Serial.print("Avg Temp = ");
  Serial.println((int)round(avgTemp));

  // Boston → LED 0
  if (temp_bos != -999) {
    if (temp_bos >= avgTemp) {
      float calcBos = (temp_bos - avgTemp) * 12.7;
      calcBos = constrain((int)round(calcBos), 0, 255);
      pixels.setPixelColor(0, pixels.Color(0, 255 - calcBos, calcBos));
    } else {
      float calcBos = (temp_bos - avgTemp) * -12.7;
      calcBos = constrain((int)round(calcBos), 0, 255);
      pixels.setPixelColor(0, pixels.Color(0, calcBos, 255 - calcBos));
    }
  }

  // Worcester → LED 1
  if (temp_wor != -999) {
    if (temp_wor >= avgTemp) {
      float calcWor = (temp_wor - avgTemp) * 12.7;
      calcWor = constrain((int)round(calcWor), 0, 255);
      pixels.setPixelColor(1, pixels.Color(0, 255 - calcWor, calcWor));
    } else {
      float calcWor = (temp_wor - avgTemp) * -12.7;
      calcWor = constrain((int)round(calcWor), 0, 255);
      pixels.setPixelColor(1, pixels.Color(0, calcWor, 255 - calcWor));
    }
  }

  // Nantucket → LED 2
  if (temp_nan != -999) {
    int calcNan = constrain((int)round((temp_nan - avgTemp) * 12.7), -255, 255);
    if (temp_nan >= avgTemp) {
      pixels.setPixelColor(2, pixels.Color(0, 255 - calcNan, calcNan));
    } else {
      calcNan = abs(calcNan);
      pixels.setPixelColor(2, pixels.Color(0, calcNan, 255 - calcNan));
    }
  }

  // Plymouth → LED 3
  if (temp_ply != -999) {
    int calcPly = constrain((int)round((temp_ply - avgTemp) * 12.7), -255, 255);
    if (temp_ply >= avgTemp) {
      pixels.setPixelColor(3, pixels.Color(0, 255 - calcPly, calcPly));
    } else {
      calcPly = abs(calcPly);
      pixels.setPixelColor(3, pixels.Color(0, calcPly, 255 - calcPly));
    }
  }

  // Salem → LED 4
  if (temp_slm != -999) {
    int calcSlm = constrain((int)round((temp_slm - avgTemp) * 12.7), -255, 255);
    if (temp_slm >= avgTemp) {
      pixels.setPixelColor(4, pixels.Color(0, 255 - calcSlm, calcSlm));
    } else {
      calcSlm = abs(calcSlm);
      pixels.setPixelColor(4, pixels.Color(0, calcSlm, 255 - calcSlm));
    }
  }

  // Springfield → LED 5
  if (temp_sgf != -999) {
    int calcSgf = constrain((int)round((temp_sgf - avgTemp) * 12.7), -255, 255);
    if (temp_sgf >= avgTemp) {
      pixels.setPixelColor(5, pixels.Color(0, 255 - calcSgf, calcSgf));
    } else {
      calcSgf = abs(calcSgf);
      pixels.setPixelColor(5, pixels.Color(0, calcSgf, 255 - calcSgf));
    }
  }

  // Taunton → LED 6
  if (temp_tan != -999) {
    int calcTan = constrain((int)round((temp_tan - avgTemp) * 12.7), -255, 255);
    if (temp_tan >= avgTemp) {
      pixels.setPixelColor(6, pixels.Color(0, 255 - calcTan, calcTan));
    } else {
      calcTan = abs(calcTan);
      pixels.setPixelColor(6, pixels.Color(0, calcTan, 255 - calcTan));
    }
  }

  // Pittsfield → LED 7
  if (temp_pit != -999) {
    int calcPit = constrain((int)round((temp_pit - avgTemp) * 12.7), -255, 255);
    if (temp_pit >= avgTemp) {
      pixels.setPixelColor(7, pixels.Color(0, 255 - calcPit, calcPit));
    } else {
      calcPit = abs(calcPit);
      pixels.setPixelColor(7, pixels.Color(0, calcPit, 255 - calcPit));
    }
  }

  pixels.show();

  delay(5 * 60 * 1000); // refresh every 5 minutes
}
