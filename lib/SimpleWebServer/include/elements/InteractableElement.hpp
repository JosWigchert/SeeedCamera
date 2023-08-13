#pragma once

#include "elements/BaseElement.h"

template <typename T>
class InteractableElement : public BaseElement
{
protected:
    typedef void (*ElementCallback)(T);
    ElementCallback callback;

public:
    InteractableElement(int size, String identifier, String label, ElementCallback callback)
        : BaseElement(size, identifier, label), callback(callback)
    {
    }

    void setCallback(ElementCallback callback)
    {
        this->callback = callback;
    }

    void executeCallback(const T &value) const
    {
        if (callback)
        {
            callback(value);
        }
    }

    virtual String toString() = 0;
    virtual void accept(BaseElementVisitor &visitor) override
    {
        visitor.visit(*this);
    }
};

// Specialization for InteractableElement with T = void
template <>
class InteractableElement<void> : public BaseElement
{
protected:
    typedef void (*ElementCallback)();
    ElementCallback callback;

public:
    InteractableElement(int size, String identifier, String label, ElementCallback callback)
        : BaseElement(size, identifier, label), callback(callback)
    {
    }

    void setCallback(ElementCallback callback)
    {
        this->callback = callback;
    }

    void executeCallback() const
    {
        if (callback)
        {
            callback();
        }
    }

    virtual String toString() = 0;
};