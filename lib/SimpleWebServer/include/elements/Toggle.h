#pragma once

#include "elements/BaseElement.h"
#include "elements/InteractableElement.hpp"

class Toggle : public InteractableElement<bool>
{
public:
    Toggle(int size, String identifier, String label, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(BaseElementVisitor &visitor) override;
};
