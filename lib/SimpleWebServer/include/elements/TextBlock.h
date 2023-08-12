#pragma once

#include "BaseElement.h"
#include "ElementVisitor.h"

class TextBlock : public BaseElement
{
public:
    TextBlock(int size, String identifier, String label);
    virtual String toString() override;
    virtual void accept(ElementVisitor &visitor) override;
};

