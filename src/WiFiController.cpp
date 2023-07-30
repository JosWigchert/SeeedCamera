#include "WiFiController.h"
#include <EEPROM.h>

WiFiController::WiFiController() {}

void WiFiController::begin() {
  // Read WiFi credentials from EEPROM
  ssid = EEPROM.readString(0);
  password = EEPROM.readString(ssid.length() + 1);

  if (ssid.length() > 0) {
    // Attempt to connect to the stored WiFi network
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.println("SSID: " + ssid + " ~ PASSWORD: " + password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(1000);
      attempts++;
    }
  }

  if (!WiFi.isConnected() || ssid.length() <= 0) {
    // If not connected to the stored network or SSID is empty, use default values
    ssid = "EspCam_" + String(ESP.getEfuseMac(), HEX);
    password = "";
    // Connect to the default WiFi network
    WiFi.softAP(ssid.c_str(), password);
    Serial.println("SSID: " + ssid + " ~ PASSWORD: " + password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(1000);
      attempts++;
    }

    // Store the default WiFi credentials in EEPROM
    EEPROM.writeString(0, ssid);
    EEPROM.writeString(ssid.length() + 1, password);
    EEPROM.commit();
  }
}


bool WiFiController::isConnected() {
  return WiFi.isConnected();
}

String WiFiController::getSSID() {
  return ssid;
}

String WiFiController::getPassword() {
  return password;
}
