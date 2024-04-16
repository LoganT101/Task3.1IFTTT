#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include "BH1750FVI.h"

// Please enter your sensitive data in the Secret tab
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

WiFiClient client;

char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "/trigger/sunlight_exposure/with/key/XyXyeRc2r-ZyzabWJUs--"; // Change your EVENT-NAME and YOUR-KEY

// initialise variables for logic
bool sunOut = false;
bool prevSunOut = false;
float lightThreshold = 400;

// initalise sensor with address
BH1750FVI myLux(0x6A);

void setup() {
  // Initialize WiFi connection
  WiFi.begin(ssid, pass);

  Serial.begin(115200);
  while (!Serial);

  // initalise sensor
  Wire.begin();

  myLux.powerOn();
  myLux.setContHighRes();
}

void loop() {
  delay(2000);

  // get lux value and check against threshold
  float val = myLux.getLux();
  sunOut = val > lightThreshold;

  // if sunOut has changed since last time send request
  if (sunOut != prevSunOut) {
    if (client.connect(HOST_NAME, 80)) {
      Serial.println("Connected to server");
      String queryString = "?sunOut=" + String(sunOut ? "true" : "false");
      // Make a HTTP request:
      // Send HTTP header
      client.print("GET " + PATH_NAME + queryString + " HTTP/1.1\r\n");
      client.print("Host: " + String(HOST_NAME) + "\r\n");
      client.print("Connection: close\r\n");
      client.print("\r\n"); // End HTTP header
      Serial.println("Sent");
    } else {
      Serial.println("Connection failed");
    }
    // Update prevSunOut after sending request
    prevSunOut = sunOut;
  }
}
