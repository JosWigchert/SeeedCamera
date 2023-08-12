#include "elements/Button.h"
#include "ElementVisitor.h"

Button::Button(int size, String identifier, String label, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback)
{
}

String Button::toString()
{
    return "<button class=\"button-primary\" id=\"" + identifier + "\"  onclick=\"handleButton(event)\">" + label + "</button>";
}

void Button::accept(ElementVisitor &visitor)
{
    visitor.visit(*this);
}