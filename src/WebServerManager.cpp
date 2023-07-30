#include "WebServerManager.h"

WebServerManager::WebServerManager() : buttonCount(0), inputCount(0), toggleCount(0) {}

void WebServerManager::begin() {
  WiFi.begin(); // Connect to Wi-Fi network; credentials should be set up before calling this function.
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  server.begin();
  server.on("/", [this]() {
    handleRoot();
  });
  server.on("/style.css", [this]() {
    handleStyle();
  });
  server.on("/script.js", HTTP_GET, [this]() {
    handleScript();
  });
}

void WebServerManager::handleClient() {
  server.handleClient();
}

void WebServerManager::addButton(const char* buttonName, const char* buttonText, const char* buttonColor, void (*buttonCallback)()) {
  if (buttonCount >= maxCallbacks) {
    Serial.println("Maximum number of buttons reached!");
    return;
  }

  // Add the button HTML to the response
  String buttonHTML = "<button name='" + String(buttonName) + "' style='background-color:" + String(buttonColor) + ";'>" + String(buttonText) + "</button>";
  responseContent += buttonHTML;

  // Store the callback function
  buttonCallbacks[buttonCount++] = buttonCallback;
}

void WebServerManager::addTextInput(const char* inputName, const char* inputLabel, const char* inputPlaceholder, void (*inputCallback)(const char* value)) {
  if (inputCount >= maxCallbacks) {
    Serial.println("Maximum number of text inputs reached!");
    return;
  }

  // Add the text input HTML to the response
  String inputHTML = "<label for='" + String(inputName) + "'>" + String(inputLabel) + ":</label>";
  inputHTML += "<input type='text' id='" + String(inputName) + "' name='" + String(inputName) + "' placeholder='" + String(inputPlaceholder) + "'>";
  responseContent += inputHTML;

  // Store the callback function
  inputCallbacks[inputCount++] = inputCallback;
}

void WebServerManager::addToggle(const char* toggleLabel, void (*toggleCallback)(bool state)) {
  if (toggleCount >= maxCallbacks) {
    Serial.println("Maximum number of toggles reached!");
    return;
  }

  // Add the toggle HTML to the response
  String toggleHTML = "<label class='toggle-label'>" + String(toggleLabel) + "</label>";
  toggleHTML += "<label class='toggle-switch'><input type='checkbox'><span class='slider'></span></label>";
  responseContent += toggleHTML;

  // Store the callback function
  toggleCallbacks[toggleCount++] = toggleCallback;
}

String WebServerManager::generateHTML() {
  String htmlContent = "<!DOCTYPE html><html><head><title>ESP32 Web Server</title>";
  htmlContent += "<link rel='stylesheet' type='text/css' href='/style.css'>";
  htmlContent += "</head><body>";

  // Add the title to the HTML content (centered and at the top)
  htmlContent += "<div class='container'>";
  htmlContent += "<h1>ESP32 Web Server</h1>";

  // Add a div to toggle between dark mode and light mode
  htmlContent += "<div class='toggle-container'>";
  htmlContent += "<label class='toggle'><input type='checkbox'><span class='slider'></span></label>";
  htmlContent += "</div>";

  // Add buttons and text inputs to the HTML content
  htmlContent += responseContent;

  htmlContent += "</div>"; // Close the .container div

  // Add a script tag to link the script.js file
  htmlContent += "<script src='/script.js'></script>";

  htmlContent += "</body></html>";
  // responseContent = ""; // Reset the response content for the next request
  return htmlContent;
}

void WebServerManager::handleRoot() {
  String response = generateHTML();
  server.send(200, "text/html", response);
}

void WebServerManager::handleStyle() {
  // Add your CSS styling here
  String cssContent = "body { font-family: Arial, sans-serif; display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  cssContent += ".container { text-align: center; }";
  cssContent += "h1 { margin-bottom: 20px; }";
  cssContent += "button, input[type='text'], .toggle-label { display: block; margin: 10px auto; }";
  cssContent += ".toggle-switch { display: block; margin: 0 auto; }";
  cssContent += ".toggle-label { margin-top: 20px; }";
  cssContent += ".toggle-switch input[type='checkbox'] { display: none; }";
  cssContent += ".slider { position: relative; display: inline-block; width: 60px; height: 30px; }";
  cssContent += ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 2px; bottom: 2px; background-color: white; border-radius: 50%; transition: 0.4s; }";
  cssContent += "input[type='checkbox']:checked + .slider:before { transform: translateX(30px); }";
  // Add more CSS rules as needed

  server.send(200, "text/css", cssContent);
}

void WebServerManager::handleScript() {
    // Add your JavaScript code here
    String scriptContent = "document.addEventListener('DOMContentLoaded', function() {";
    scriptContent += "const toggleSwitches = document.querySelectorAll('.toggle-switch input');";
    scriptContent += "toggleSwitches.forEach(function(toggle) {";
    scriptContent += "toggle.addEventListener('change', function() {";
    scriptContent += "const isChecked = toggle.checked;";
    scriptContent += "const toggleLabel = toggle.parentNode.previousElementSibling.innerHTML;";
    scriptContent += "toggleCallbacks.forEach(function(callback) {";
    scriptContent += "callback(toggleLabel, isChecked);";
    scriptContent += "});";
    scriptContent += "});";
    scriptContent += "});";
    scriptContent += "});";

    server.send(200, "application/javascript", scriptContent);
}
