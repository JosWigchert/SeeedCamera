#include "visitors/InteractableElementCallbackVisitor.h"

#include "elements/BaseElement.h"
#include "elements/InteractableElement.hpp"

InteractableElementCallbackVisitor::InteractableElementCallbackVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue)
    : jsonValue(jsonValue)
{
}

void InteractableElementCallbackVisitor::visit(InteractableElement<void> &element)
{
    element.executeCallback();
}

void InteractableElementCallbackVisitor::visit(InteractableElement<int> &element)
{
    int value = jsonValue.as<int>();
    element.executeCallback(value);
}

void InteractableElementCallbackVisitor::visit(InteractableElement<String> &element)
{
    String value = jsonValue.as<String>();
    element.executeCallback(value);
}

void InteractableElementCallbackVisitor::visit(InteractableElement<bool> &element)
{
    bool value = jsonValue.as<bool>();
    element.executeCallback(value);
}

void InteractableElementCallbackVisitor::visit(InteractableElement<float> &element)
{
    float value = jsonValue.as<float>();
    element.executeCallback(value);
}

void InteractableElementCallbackVisitor::visit(InteractableElement<double> &element)
{
    double value = jsonValue.as<double>();
    element.executeCallback(value);
}

void InteractableElementCallbackVisitor::visit(BaseElement &element)
{
    return;
}