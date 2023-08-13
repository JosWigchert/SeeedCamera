#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
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

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
    void pushUpdate();

private:
    WebServer server;
    WebSocketsServer *webSocket;
    String responseContent;

    std::map<Position, BaseElement *> elements;
    std::map<const char *, ValueInfo> values;

    void handleRoot();
    void handleValues();
    void handleCallback();
    void handleFile();
    String generateHTML();
};
