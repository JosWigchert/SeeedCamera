#include "SimpleWebServer.h"

#include "ElementVisitor.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>

SimpleWebServer::SimpleWebServer() {}

void SimpleWebServer::begin()
{
    server.begin();
    server.on("/", [this]()
              { handleRoot(); });
    server.on("/api/values", [this]()
              { handleValues(); });
    server.on("/api/callback", [this]()
              { handleCallback(); });
    server.onNotFound([this]()
                      { handleFile(); });
}

void SimpleWebServer::handleClient()
{
    server.handleClient();
}

void SimpleWebServer::addHTMLElement(Position position, BaseElement *element)
{
    elements.emplace(position, element);
}

void SimpleWebServer::removeHTMLElement(Position position)
{
    if (elements.find(position) != elements.end())
    {
        delete elements[position];
        elements.erase(position);
    }
}

void SimpleWebServer::handleRoot()
{
    String response = generateHTML();
    server.send(200, "text/html", response);
}

void SimpleWebServer::handleValues()
{
    String response = "{}";
    server.send(200, "application/json", response);
}

void SimpleWebServer::handleCallback()
{
    // Check if it's a POST request
    if (server.method() != HTTP_POST)
    {
        server.send(400, "text/plain", "Bad Request");
        return;
    }

    // Check if there is any data available in the request
    if (server.args() == 0)
    {
        server.send(400, "text/plain", "No JSON data");
        return;
    }

    // Get the JSON data from the request
    String jsonString = server.arg("plain");
    Serial.println("Received JSON data:");
    Serial.println(jsonString);

    // Parse the JSON data
    DynamicJsonDocument jsonDocument(1024); // Adjust the size according to your JSON data
    DeserializationError error = deserializeJson(jsonDocument, jsonString);

    // Check for parsing errors
    if (error)
    {
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());
        server.send(400, "text/plain", "JSON Parsing Error");
        return;
    }

    // Access the JSON data fields
    const char *identifier = jsonDocument["identifier"];
    auto value = jsonDocument["value"];

    ElementVisitor visitor(value);

    for (const auto &pair : elements)
    {
        if (pair.second->hasIdentifier(identifier))
        {
            pair.second->accept(visitor);
        }
    }

    server.send(200, "text/plain", "Success");
}

void SimpleWebServer::handleFile()
{
    String path = server.uri();

    // Check if the file exists in SPIFFS
    if (SPIFFS.exists(path))
    {
        // Determine the content type based on the file extension
        String contentType;
        if (path.endsWith(".html"))
        {
            contentType = "text/html";
        }
        else if (path.endsWith(".css"))
        {
            contentType = "text/css";
        }
        else if (path.endsWith(".js"))
        {
            contentType = "application/javascript";
        }
        else
        {
            contentType = "text/plain";
        }

        // Serve the file
        File file = SPIFFS.open(path, "r");
        if (file)
        {
            server.streamFile(file, contentType);
            file.close();
            return;
        }
    }

    // If the file does not exist, return a 404 Not Found error
    server.send(404, "text/plain", "File Not Found");
}

String SimpleWebServer::generateHTML()
{
    String htmlContent = "<!DOCTYPE html>\n";
    htmlContent += "<html lang=\"en\">\n";
    htmlContent += "<head>\n";
    htmlContent += "  <meta charset=\"utf-8\">\n";
    htmlContent += "  <title>Arduino Webserver</title>\n";
    htmlContent += "  <meta name=\"description\" content=\"Arduino Webserver\">\n";
    htmlContent += "  <meta name=\"author\" content=\"\">\n";
    htmlContent += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    htmlContent += "  <link rel=\"stylesheet\" href=\"/css/font.min.css\">\n";
    htmlContent += "  <link rel=\"stylesheet\" href=\"/css/normalize.min.css\">\n";
    htmlContent += "  <link rel=\"stylesheet\" href=\"/css/skeleton.min.css\">\n";
    htmlContent += "  <link rel=\"stylesheet\" href=\"/css/custom.min.css\">\n";
    htmlContent += "  <link rel=\"icon\" href=\"favicon.gif\" type=\"image/gif\">\n";
    htmlContent += "</head>\n";
    htmlContent += "<body>\n";

    // Initialize the container where all the rows will sit
    htmlContent += "<div class=\"container\">";

    // Add the title to the HTML content (centered and at the top)
    htmlContent += "<div class=\"row\">";
    htmlContent += "<div class=\"column\" style=\"margin-top: 25%\">";
    htmlContent += "<h1>Arduino Web Server</h1>";
    htmlContent += "</div>";

    // Add buttons and text inputs to the HTML content
    int currentRow = -1;
    int currentColumn = 0;

    for (const auto &pair : elements)
    {
        while (pair.first.getRow() > currentRow)
        {
            htmlContent += "</div>";
            htmlContent += "<div class=\"row\" style=\"margin-top: 8px\">";
            currentRow++;
            currentColumn = 0;
        }
        htmlContent += "<div class=\"four columns\">";
        while (pair.first.getColumn() != currentColumn)
        {
            htmlContent += "</div>";
            htmlContent += "<div class=\"four columns\">";
            currentColumn++;
        }

        htmlContent += pair.second->toString();
        htmlContent += "</div>";
    }

    htmlContent += "</div>"; // Close the .container div

    // Add a script tag to link the script.js file
    htmlContent += "<script src='/script.js'></script>";

    htmlContent += "</body></html>";
    // responseContent = ""; // Reset the response content for the next request
    return htmlContent;
}
