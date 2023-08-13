#include "elements/TextInput.h"

TextInput::TextInput(int size, String identifier, String label, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback)
{
}

TextInput::TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), placeholder(placeholder)
{
}

String TextInput::toString()
{
    return "<label for=\"" + identifier + "\">" + label + "</label> <input type=\"text\" id=\"" + identifier + "\" placeholder=\"" + placeholder + "\" onkeydown=\"handleTextInput(event)\">";
}

void TextInput::accept(BaseElementVisitor &visitor)
{
    visitor.visit(*this);
}