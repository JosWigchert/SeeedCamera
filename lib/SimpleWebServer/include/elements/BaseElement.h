#pragma once

#include <Arduino.h>
#include "visitors/BaseElementVisitor.h"

class BaseElement
{
protected:
    int size;

    String identifier;
    String label;

public:
    BaseElement(int size, String id, String label);
    bool hasIdentifier(String identifier);
    String getIdentifier();
    String getLabel();
    virtual String toString() = 0;
    virtual void accept(BaseElementVisitor &visitor) = 0;
};
