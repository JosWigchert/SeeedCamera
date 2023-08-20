#include "elements/Button.h"

Button::Button(int size, String identifier, String label, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback)
{
}

String Button::toString()
{
    return "<button class=\"button-primary\" id=\"" + identifier + "\">" + label + "</button>";
}

void Button::accept(BaseElementVisitor &visitor)
{
    visitor.visit(*this);
}