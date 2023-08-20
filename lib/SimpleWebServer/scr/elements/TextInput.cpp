#include "elements/TextInput.h"

TextInput::TextInput(int size, String identifier, String label, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback)
{
}

TextInput::TextInput(int size, String identifier, String label, InputType type, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), type(type)
{
}

TextInput::TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), placeholder(placeholder)
{
}

TextInput::TextInput(int size, String identifier, String label, String placeholder, InputType type, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), placeholder(placeholder), type(type)
{
}

String TextInput::toString()
{
    return "<label for=\"" + identifier + "\">" + label + "</label> <input type=\"" + getHtmlInputType(type) + "\" id=\"" + identifier + "\" placeholder=\"" + placeholder + "\">";
}

void TextInput::accept(BaseElementVisitor &visitor)
{
    visitor.visit(*this);
}

String TextInput::getHtmlInputType(InputType type)
{
    switch (type)
    {
    case InputType::TEXT:
        return "text";
    case InputType::EMAIL:
        return "email";
    case InputType::PASSWORD:
        return "password";
    case InputType::URL:
        return "url";
    case InputType::PHONE_NUMBER:
        return "tel";
    case InputType::NUMBER:
        return "number";
    default:
        return "text"; // Default to text if an invalid value is provided
    }
}
