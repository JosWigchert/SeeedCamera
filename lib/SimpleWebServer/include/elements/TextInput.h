#pragma once

#include "elements/BaseElement.h"
#include "elements/InteractableElement.hpp"

class TextInput : public InteractableElement<String>
{
public:
    enum class InputType
    {
        TEXT = 0,
        EMAIL,
        PASSWORD,
        URL,
        PHONE_NUMBER,
        NUMBER
    };

private:
    String placeholder;
    InputType type;

public:
    TextInput(int size, String identifier, String label, ElementCallback callback);
    TextInput(int size, String identifier, String label, InputType type, ElementCallback callback);
    TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback);
    TextInput(int size, String identifier, String label, String placeholder, InputType type, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(BaseElementVisitor &visitor) override;

private:
    String getHtmlInputType(InputType type);
};
