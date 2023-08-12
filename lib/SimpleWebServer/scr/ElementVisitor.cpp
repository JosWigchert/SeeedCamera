#include "ElementVisitor.h"

#include "elements/BaseElement.h"
#include "elements/InteractableElement.hpp"

ElementVisitor::ElementVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue)
    : jsonValue(jsonValue)
{
}

void ElementVisitor::visit(InteractableElement<void> &element)
{
    element.executeCallback();
    Serial.print("Callback for void");
}

void ElementVisitor::visit(InteractableElement<int> &element)
{
    int value = jsonValue.as<int>();
    element.executeCallback(value);
    Serial.print("Callback for int with value: ");
    Serial.println(value);
}

void ElementVisitor::visit(InteractableElement<String> &element)
{
    String value = jsonValue.as<String>();
    element.executeCallback(value);
    Serial.print("Callback for String with value: ");
    Serial.println(value);
}

void ElementVisitor::visit(InteractableElement<bool> &element)
{
    bool value = jsonValue.as<bool>();
    element.executeCallback(value);
    Serial.print("Callback for bool with value: ");
    Serial.println(value);
}

void ElementVisitor::visit(InteractableElement<float> &element)
{
    float value = jsonValue.as<float>();
    element.executeCallback(value);
    Serial.print("Callback for float with value: ");
    Serial.println(value);
}

void ElementVisitor::visit(InteractableElement<double> &element)
{
    double value = jsonValue.as<double>();
    element.executeCallback(value);
    Serial.print("Callback for double with value: ");
    Serial.println(value);
}

void ElementVisitor::visit(BaseElement &element)
{
    return;
}