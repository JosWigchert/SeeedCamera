#include "elements/TextBlock.h"
#include "ElementVisitor.h"

TextBlock::TextBlock(int size, String identifier, String label)
    : BaseElement(size, identifier, label)
{
}

String TextBlock::toString()
{
    return "<label for=\"" + identifier + "\">" + label + ": </label> <p id=\"" + identifier + "\">Loading...</p>";
}

void TextBlock::accept(ElementVisitor &visitor)
{
    visitor.visit(*this);
}