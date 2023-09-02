#include "SimpleWebServer.h"

#include "visitors/InteractableElementCallbackVisitor.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>

SimpleWebServer::SimpleWebServer()
{
    socket = new WiFiServer(81);
    clients = new WiFiClient[CLIENT_COUNT];
}

SimpleWebServer::~SimpleWebServer()
{
    delete socket;

    for (uint16_t i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].connected())
            clients[i].stop();
    }

    delete[] clients;
}

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

    socket->begin();
}

void SimpleWebServer::stop()
{
    server.stop();

    for (uint16_t i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].connected())
            clients[i].stop();
    }

    socket->stopAll();
}

void SimpleWebServer::handleClient()
{
    server.handleClient();

    WiFiClient client = socket->available();
    if (client)
    {
        Serial.println("Client connected");
        if (client.connected())
        {
            addClient(client);
        }
    }

    for (uint16_t i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].connected())
        {
            if (clients[i].available())
            {
                String request = client.readStringUntil('\r');
                Serial.println("Received: " + request);
            }
        }
    }
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

void SimpleWebServer::addValueWatch(const char *id, void *valuePtr, ValueType valueType)
{
    ValueInfo valueInfo = {valuePtr, valueType};
    addValueWatch(id, valueInfo);
}

void SimpleWebServer::addValueWatch(const char *id, ValueInfo valueInfo)
{
    values.emplace(id, valueInfo);
}

void SimpleWebServer::removeValueWatch(const char *id)
{
    values.erase(id);
}

void SimpleWebServer::addClient(WiFiClient client)
{
    for (uint16_t i = 0; i < CLIENT_COUNT; i++)
    {
        if (!clients[i].connected())
        {
            clients[i] = client;
            break;
        }
    }
}

void SimpleWebServer::handleRoot()
{
    String response = generateHTML();
    server.send(200, "text/html", response);
}

void SimpleWebServer::handleValues()
{
    StaticJsonDocument<1024> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();

    for (const auto &pair : values)
    {
        switch (pair.second.valueType)
        {
        case INT_TYPE:
            json[pair.first] = *(int *)(pair.second.valuePtr);
            break;
        case FLOAT_TYPE:
            json[pair.first] = *(float *)(pair.second.valuePtr);
            break;
        case DOUBLE_TYPE:
            json[pair.first] = *(double *)(pair.second.valuePtr);
            break;
        case BOOL_TYPE:
            json[pair.first] = *(bool *)(pair.second.valuePtr);
            break;
        case STRING_TYPE:
            json[pair.first] = *(String *)(pair.second.valuePtr);
            break;
        case STD_STRING_TYPE:
            json[pair.first] = *(std::string *)(pair.second.valuePtr);
            break;
        }
    }

    String jsonString;
    serializeJson(json, jsonString);

    server.send(200, "application/json", jsonString);
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

    InteractableElementCallbackVisitor visitor(value);

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
    htmlContent += "\t<meta charset=\"utf-8\">\n";
    htmlContent += "\t<title>Arduino Webserver</title>\n";
    htmlContent += "\t<meta name=\"description\" content=\"Arduino Webserver\">\n";
    htmlContent += "\t<meta name=\"author\" content=\"\">\n";
    htmlContent += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    htmlContent += "\t<link rel=\"stylesheet\" href=\"/css/font.min.css\">\n";
    htmlContent += "\t<link rel=\"stylesheet\" href=\"/css/normalize.min.css\">\n";
    htmlContent += "\t<link rel=\"stylesheet\" href=\"/css/skeleton.min.css\">\n";
    htmlContent += "\t<link rel=\"stylesheet\" href=\"/css/custom.min.css\">\n";
    htmlContent += "\t<link rel=\"icon\" href=\"favicon.gif\" type=\"image/gif\">\n";
    htmlContent += "</head>\n";
    htmlContent += "<body>\n";

    // Initialize the container where all the rows will sit
    htmlContent += "<div class=\"container\">";

    // Add the title to the HTML content (centered and at the top)
    htmlContent += "\t<div class=\"row\">\n";
    htmlContent += "\t\t<div class=\"column\" style=\"margin-top: 25%\">\n";
    htmlContent += "\t\t<h1>Arduino Web Server</h1>\n";
    htmlContent += "\t</div>\n";

    // Add buttons and text inputs to the HTML content
    int currentRow = 0;
    int currentColumn = 0;

    htmlContent += "\t<div class=\"row\" style=\"margin-top: 8px\">\n";
    for (const auto &pair : elements)
    {
        while (pair.first.getRow() > currentRow)
        {
            htmlContent += "\t</div>\n";
            htmlContent += "\t<div class=\"row\" style=\"margin-top: 8px\">\n";

            currentRow++;
            currentColumn = 0;
        }

        htmlContent += "\t\t<div class=\"four columns\">\n";
        while (pair.first.getColumn() > currentColumn)
        {
            htmlContent += "\t\t</div>\n";
            htmlContent += "\t\t<div class=\"four columns\">\n";
            currentColumn++;
        }

        htmlContent += "\t\t\t";
        htmlContent += pair.second->toString();
        htmlContent += "\n\t\t</div>\n"; // Close the .four .columns div
        currentColumn++;
    }

    htmlContent += "</div>\n"; // Close the .container div

    // Add a script tag to link the script.js file
    htmlContent += "<script src='/script.js'></script>\n";

    htmlContent += "</body></html>\n";

    return htmlContent;
}

void SimpleWebServer::pushUpdate()
{
    StaticJsonDocument<1024> jsonBuffer;
    JsonObject json = jsonBuffer.to<JsonObject>();

    for (const auto &pair : values)
    {
        switch (pair.second.valueType)
        {
        case INT_TYPE:
            json[pair.first] = *(int *)(pair.second.valuePtr);
            break;
        case FLOAT_TYPE:
            json[pair.first] = *(float *)(pair.second.valuePtr);
            break;
        case DOUBLE_TYPE:
            json[pair.first] = *(double *)(pair.second.valuePtr);
            break;
        case BOOL_TYPE:
            json[pair.first] = *(bool *)(pair.second.valuePtr);
            break;
        case STRING_TYPE:
            json[pair.first] = *(String *)(pair.second.valuePtr);
            break;
        case STD_STRING_TYPE:
            json[pair.first] = *(std::string *)(pair.second.valuePtr);
            break;
        }
    }

    String jsonString;
    serializeJson(json, jsonString);

    for (uint16_t i = 0; i < CLIENT_COUNT; i++)
    {
        if (clients[i].connected())
        {
            clients[i].print(jsonString.c_str());
        }
    }
}