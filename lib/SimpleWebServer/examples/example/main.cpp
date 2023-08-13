#include <WiFi.h> //https://github.com/esp8266/Arduino
// #include <ArduinoOTA.h>
#include <SPIFFS.h>

#include "SimpleWebServer.h"
#include "elements/Elements.h"

SimpleWebServer webServer;

void handleButtonPress();
void handleInputValue(String value);
void handleToggleChange(bool value);

int buttonClickCount = 0;

const char *dummyStatuses[] = {
    "Status A",
    "Status B",
    "Status C",
    "Status D",
    "Status E",
    "Status F",
    "Status G",
    "Status H"};
const int numStatuses = sizeof(dummyStatuses) / sizeof(dummyStatuses[0]);

String status = "";

void setup()
{
    // Wait for serial monitor to start
    delay(5000);

    // Setup random
    randomSeed(analogRead(0)); // Initialize random number generator

    // Setup Serial Monitor
    Serial.begin(115200);
    Serial.println("Starting ESP");

    // Initialize SPIFFS to get all the javascript and css files from the filesystem
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
    webServer.addHTMLElement(Position(1, 1), new TextBlock(4, "textblock2", "Button clicked"));
    webServer.addHTMLElement(Position(2, 1), new TextBlock(4, "textblock3", "Status"));

    // Order doesn't matter
    webServer.addHTMLElement(Position(2, 0), new Toggle(4, "toggle1", "Toggle", handleToggleChange));
    webServer.addHTMLElement(Position(1, 2), new Button(4, "button1", "Button", handleButtonPress));
    webServer.addHTMLElement(Position(3, 2), new TextInput(4, "textinput1", "TextInput", "placeholder", handleInputValue));

    // Add values to watch when the page asks for data
    webServer.addValueWatch("textblock1", new std::string("test"), ValueType::STD_STRING_TYPE);
    webServer.addValueWatch("textblock2", &buttonClickCount, ValueType::INT_TYPE);
    webServer.addValueWatch("textblock3", &status, ValueType::STRING_TYPE);
}

unsigned long previousMillis = 0; // Store the last time the timer was triggered

void loop()
{
    // ArduinoOTA.handle();
    webServer.handleClient();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000)
    {
        int randomIndex = random(0, numStatuses);
        const char *selectedStatus = dummyStatuses[randomIndex];

        status = selectedStatus;
        Serial.print("Current Status: ");
        Serial.println(selectedStatus);

        previousMillis = currentMillis;
    }
}

void handleButtonPress()
{
    // Your code to handle button press goes here
    Serial.println("Button Pressed! Pushed update");
    buttonClickCount++;
    webServer.pushUpdate();
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