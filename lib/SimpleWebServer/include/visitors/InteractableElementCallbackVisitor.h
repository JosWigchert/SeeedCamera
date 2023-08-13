#pragma once

#include <ArduinoJson.h>

#include "visitors/BaseElementVisitor.h"

class InteractableElementCallbackVisitor : public BaseElementVisitor
{
public:
    InteractableElementCallbackVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue);

    virtual void visit(InteractableElement<void> &element) override;
    virtual void visit(InteractableElement<int> &element) override;
    virtual void visit(InteractableElement<String> &element) override;
    virtual void visit(InteractableElement<bool> &element) override;
    virtual void visit(InteractableElement<float> &element) override;
    virtual void visit(InteractableElement<double> &element) override;
    virtual void visit(BaseElement &element) override;

private:
    ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue;
};
