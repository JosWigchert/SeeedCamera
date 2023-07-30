#include <WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <WebServer.h>
#include <ESP_WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "WebServerManager.h"

WebServerManager webServer;

void handleButtonPress() {
  // Your code to handle button press goes here
  Serial.println("Button Pressed!");
}

void handleInputValue(const char* value) {
  // Your code to handle the input value goes here
  Serial.print("Received Input Value: ");
  Serial.println(value);
}

void handleTogglePress(bool value) {
  // Your code to handle the input value goes here
  Serial.print("Received Input Value: ");
  Serial.println(value);
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Starting ESP");
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    // ESP_WiFiManager wifiManager;
    //reset saved settings
    // wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    // wifiManager.autoConnect();
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi


    // Get the unique chip ID as a string
    String chipID = String((uint32_t)ESP.getEfuseMac(), HEX);

    // Create the SSID with the chip ID
    String ssid = "ESP_" + chipID;

    // Start the access point
    const char* password = ""; // The password for the access point
    WiFi.softAP(ssid.c_str(), password);

    Serial.println("Access Point started");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());


    // Serial.println("Connected to WiFi!");
    // Serial.print("IP Address: ");
    // Serial.println(WiFi.localIP());

    webServer.begin();
    webServer.addButton("btn1", "Click Me", "#4CAF50", handleButtonPress);
    webServer.addTextInput("input1", "Enter Value", "Type something", handleInputValue);
    webServer.addToggle("Toggle", handleTogglePress);
}

void loop() {
    webServer.handleClient();

}