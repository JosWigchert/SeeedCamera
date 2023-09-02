#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <map>

#include "elements/BaseElement.h"
#include "Position.h"
#include "ValueInfo.h"

class SimpleWebServer
{
public:
    SimpleWebServer();
    ~SimpleWebServer();
    void begin();
    void stop();
    void handleClient();
    void addHTMLElement(Position position, BaseElement *element);
    void removeHTMLElement(Position position);
    void addValueWatch(const char *id, ValueInfo valueInfo);
    void addValueWatch(const char *id, void *valuePtr, ValueType valueType);
    void removeValueWatch(const char *id);

    void pushUpdate();

private:
    const uint16_t CLIENT_COUNT = 10;

    WebServer server;
    WiFiServer *socket;
    WiFiClient *clients;
    String responseContent;

    std::map<Position, BaseElement *> elements;
    std::map<const char *, ValueInfo> values;

    void addClient(WiFiClient client);
    void handleRoot();
    void handleValues();
    void handleCallback();
    void handleFile();
    String generateHTML();
};
