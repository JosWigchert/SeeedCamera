#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <map>

#include "elements/BaseElement.h"
#include "Position.h"

class SimpleWebServer
{
public:
    SimpleWebServer();
    void begin();
    void handleClient();
    void addHTMLElement(Position position, BaseElement *element);
    void removeHTMLElement(Position position);

private:
    WebServer server;
    String responseContent;
    const static int maxCallbacks = 5; // Maximum number of callbacks for buttons and text inputs

    std::map<Position, BaseElement *> elements;

    void handleRoot();
    void handleValues();
    void handleCallback();
    void handleFile();
    String generateHTML();
};


