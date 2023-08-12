#include <WiFi.h> //https://github.com/esp8266/Arduino
#include <SPIFFS.h>

#include "SimpleWebServer.h"
#include "elements/Elements.h"

SimpleWebServer webServer;

void handleButtonPress()
{
    // Your code to handle button press goes here
    Serial.println("Button Pressed!");
}

void handleInputValue(String value)
{
    // Your code to handle the input value goes here
    Serial.print("Received Input Value: ");
    Serial.println(value);
}

void handleToggleChange(bool value)
{
    // Your code to handle the input value goes here
    Serial.print("Received Toggle Value: ");
    Serial.println(value);
}

void setup()
{
    // Wait for serial monitor to start
    delay(5000);

    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Starting ESP");

    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }
    Serial.println("File system mounted successfully");

    // Get the unique chip ID as a string
    String chipID = String((uint32_t)ESP.getEfuseMac(), HEX);

    // Create the SSID with the chip ID
    String ssid = "ESP_" + chipID;

    // Start the access point
    const char *password = ""; // The password for the access point
    WiFi.softAP(ssid.c_str(), password);

    Serial.println("Access Point started");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    webServer.begin();
    // add elements to the webserver
    webServer.addHTMLElement(Position(0, 1), new TextBlock(4, "textblock1", "Test"));
    webServer.addHTMLElement(Position(2, 0), new Toggle(4, "toggle1", "Toggle", handleToggleChange));
    webServer.addHTMLElement(Position(1, 2), new Button(4, "button1", "Button", handleButtonPress));
    webServer.addHTMLElement(Position(3, 2), new TextInput(4, "textinput1", "TextInput", "placeholder", handleInputValue));
}

void loop()
{
    webServer.handleClient();
    // Serial.println("Test");
    // delay(1000);
}