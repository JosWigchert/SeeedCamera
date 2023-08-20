#include "elements/ComboBox.h"

ComboBox::ComboBox(int size, String identifier, String label, std::map<int, String> options, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), options(options), selected(0)
{
}

ComboBox::ComboBox(int size, String identifier, String label, std::map<int, String> options, int selected, ElementCallback callback)
    : InteractableElement(size, identifier, label, callback), options(options), selected(selected)
{
}

String ComboBox::toString()
{
    String html = "<label for=\"" + identifier + "\">" + label + "</label> <select id=\"" + identifier + "\" class=\"u-three-quart-width\">";

    for (const auto &pair : options)
    {
        html += "<option value=\"" + String(pair.first) + "\" " + String(pair.first == selected ? "selected" : "") + ">" + pair.second + "</option>";
    }

    html += "</select>";

    return html;
}

void ComboBox::accept(BaseElementVisitor &visitor)
{
    visitor.visit(*this);
}