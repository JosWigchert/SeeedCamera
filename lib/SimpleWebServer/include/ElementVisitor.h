#pragma once

template <typename T>
class InteractableElement;
class BaseElement;

#include <ArduinoJson.h>

class ElementVisitor
{
public:
    ElementVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue);

    void visit(InteractableElement<void> &element);
    void visit(InteractableElement<int> &element);
    void visit(InteractableElement<String> &element);
    void visit(InteractableElement<bool> &element);
    void visit(InteractableElement<float> &element);
    void visit(InteractableElement<double> &element);
    void visit(BaseElement &element);

private: 
    ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue;
};

