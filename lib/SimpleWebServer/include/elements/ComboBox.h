#pragma once

#include "elements/BaseElement.h"
#include "elements/InteractableElement.hpp"

#include <map>

class ComboBox : public InteractableElement<int>
{
public:
    ComboBox(int size, String identifier, String label, std::map<int, String> options, ElementCallback callback);
    ComboBox(int size, String identifier, String label, std::map<int, String> options, int selected, ElementCallback callback);
    virtual String toString() override;
    virtual void accept(BaseElementVisitor &visitor) override;

private:
    std::map<int, String> options;
    int selected;
};
