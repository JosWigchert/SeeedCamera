#pragma once

#include <Arduino.h>

template <typename T>
class InteractableElement;
class BaseElement;

class BaseElementVisitor
{
public:
    virtual void visit(InteractableElement<void> &element) = 0;
    virtual void visit(InteractableElement<int> &element) = 0;
    virtual void visit(InteractableElement<String> &element) = 0;
    virtual void visit(InteractableElement<bool> &element) = 0;
    virtual void visit(InteractableElement<float> &element) = 0;
    virtual void visit(InteractableElement<double> &element) = 0;
    virtual void visit(BaseElement &element) = 0;
};