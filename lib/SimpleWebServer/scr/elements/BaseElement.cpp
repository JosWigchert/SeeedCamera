#include "elements/BaseElement.h"

BaseElement::BaseElement(int size, String identifier, String label) : size(size), identifier(identifier), label(label)
{
}

bool BaseElement::hasIdentifier(String identifier)
{
    return this->identifier == identifier;
}

String BaseElement::getIdentifier()
{
    return this->identifier;
}

String BaseElement::getLabel()
{
    return this->label;
}

void BaseElement::accept(BaseElementVisitor &visitor)
{
    visitor.visit(*this);
}