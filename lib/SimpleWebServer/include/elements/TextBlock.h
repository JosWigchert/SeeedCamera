#pragma once

#include "elements/BaseElement.h"

class TextBlock : public BaseElement
{
public:
    TextBlock(int size, String identifier, String label);
    virtual String toString() override;
    virtual void accept(BaseElementVisitor &visitor) override;
};
