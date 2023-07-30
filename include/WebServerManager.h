#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WiFi.h>
#include <WebServer.h>

class WebServerManager {
public:
  WebServerManager();
  void begin();
  void handleClient();
  void addButton(const char* buttonName, const char* buttonText, const char* buttonColor, void (*buttonCallback)());
  void addTextInput(const char* inputName, const char* inputLabel, const char* inputPlaceholder, void (*inputCallback)(const char* value));
  void addToggle(const char* toggleLabel, void (*toggleCallback)(bool state));
  String generateHTML();

private:
  WebServer server;
  String responseContent;
  const static int maxCallbacks = 5; // Maximum number of callbacks for buttons and text inputs
  void (*buttonCallbacks[maxCallbacks])();
  void (*inputCallbacks[maxCallbacks])(const char*);
  void (*toggleCallbacks[maxCallbacks])(bool);
  int buttonCount;
  int inputCount;
  int toggleCount;
  void handleRoot();
  void handleStyle();
  void handleScript();
  String generateHTMLWithStyle();
};

#endif
