#pragma once

#include "InteractableElement.hpp"

class Toggle : public InteractableElement<bool>
{
public:
    Toggle(int size, String identifier, String label, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(ElementVisitor &visitor) override;
};

