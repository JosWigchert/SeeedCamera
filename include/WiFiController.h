#pragma once

#include <WiFi.h>

class WiFiController {
public:
  WiFiController();
  void begin();
  bool isConnected();
  String getSSID();
  String getPassword();

private:
  String ssid;
  String password;
};


