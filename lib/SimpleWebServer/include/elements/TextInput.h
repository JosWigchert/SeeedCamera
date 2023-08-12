#pragma once

#include "InteractableElement.hpp"

class TextInput : public InteractableElement<String>
{
private:
    String placeholder;

public:
    TextInput(int size, String identifier, String label, ElementCallback callback);
    TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(ElementVisitor &visitor) override;
};

