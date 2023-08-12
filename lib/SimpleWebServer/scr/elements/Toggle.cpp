#include "elements/Toggle.h"
#include "ElementVisitor.h"

Toggle::Toggle(int size, String identifier, String label, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback)
{
}

String Toggle::toString()
{
    return "<label for=\"" + identifier + "\" class=\"toggle-switch\"> " + label + ": <input type=\"checkbox\" id=\"" + identifier + "\" onchange=\"handleToggle(event)\"><span class=\"slider\"> </span> </label>";
}

void Toggle::accept(ElementVisitor &visitor)
{
    visitor.visit(*this);
}