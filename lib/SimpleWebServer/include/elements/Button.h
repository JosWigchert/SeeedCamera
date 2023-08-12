#pragma once

#include "InteractableElement.hpp"

class Button : public InteractableElement<void>
{
public:
    Button(int size, String identifier, String label, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(ElementVisitor &visitor) override;
};

