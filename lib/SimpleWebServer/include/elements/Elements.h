#pragma once

#include "BaseElement.h"
#include "InteractableElement.hpp"

#include "Button.h"
#include "TextBlock.h"
#include "TextInput.h"
#include "Toggle.h"


// #ifndef ELEMENTS_H
// #define ELEMENTS_H

// #include <Arduino.h>
// #include <ArduinoJson.h>

// class ElementVisitor;

// class BaseElement
// {
// protected:
//     int size;

//     String identifier;
//     String label;

// public:
//     BaseElement(int size, String id, String label);
//     bool hasIdentifier(String identifier);
//     virtual String toString() = 0;
//     virtual void accept(ElementVisitor &visitor) = 0;
// };

// //---------------------------------------------------------

// template <typename T>
// class InteractableElement : public BaseElement
// {
// protected:
//     typedef void (*ElementCallback)(T);
//     ElementCallback callback;

// public:
//     InteractableElement(int size, String identifier, String label, ElementCallback callback);
//     void setCallback(ElementCallback callback);
//     void executeCallback(const T &value) const;

//     virtual String toString() = 0;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// // Specialization for InteractableElement with T = void
// template <>
// class InteractableElement<void> : public BaseElement
// {
// protected:
//     typedef void (*ElementCallback)();
//     ElementCallback callback;

// public:
//     InteractableElement(int size, String identifier, String label, ElementCallback callback);
//     void setCallback(ElementCallback callback);
//     void executeCallback() const;

//     virtual String toString() = 0;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// //---------------------------------------------------------

// class Button : public InteractableElement<void>
// {
// public:
//     Button(int size, String identifier, String label, ElementCallback callback);
//     virtual String toString() override;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// //---------------------------------------------------------

// class TextBlock : public BaseElement
// {
// public:
//     TextBlock(int size, String identifier, String label);
//     virtual String toString() override;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// //---------------------------------------------------------

// class TextInput : public InteractableElement<String>
// {
// private:
//     String placeholder;

// public:
//     TextInput(int size, String identifier, String label, ElementCallback callback);
//     TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback);
//     virtual String toString() override;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// //---------------------------------------------------------

// class Toggle : public InteractableElement<bool>
// {
// public:
//     Toggle(int size, String identifier, String label, ElementCallback callback);
//     virtual String toString() override;
//     virtual void accept(ElementVisitor &visitor) override;
// };

// //---------------------------------------------------------

// class ElementVisitor
// {
// public:
//     ElementVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue);

//     void visit(InteractableElement<void> &element);
//     template <typename T>
//     void visit(InteractableElement<T> &element);
//     void visit(BaseElement &element);

// private: 
//     ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue;
// };

// //---------------------------------------------------------

// ElementVisitor::ElementVisitor(ArduinoJson::V6213PB2::detail::MemberProxy<ArduinoJson::V6213PB2::JsonDocument &, const char *> jsonValue)
//     : jsonValue(jsonValue)
// {
// }

// void ElementVisitor::visit(InteractableElement<void> &element)
// {
//     element.executeCallback();
//     Serial.print("Callback for void");
// }


// template <typename T>
// void ElementVisitor::visit(InteractableElement<T> &element)
// {
//     element.executeCallback(jsonValue.as<T>());
//     Serial.print("Callback for T");
// }


// void ElementVisitor::visit(BaseElement &element)
// {
//     return;
// }

// //---------------------------------------------------------

// BaseElement::BaseElement(int size, String identifier, String label) : size(size), identifier(identifier), label(label) 
// {
// }

// bool BaseElement::hasIdentifier(String identifier)
// {
//     return this->identifier == identifier;   
// }

// void BaseElement::accept(ElementVisitor &visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// // Primary template for InteractableElement
// template <typename T>
// InteractableElement<T>::InteractableElement(int size, String identifier, String label, ElementCallback callback)
//     : BaseElement(size, identifier, label), callback(callback)
// {
// }

// template <typename T>
// void InteractableElement<T>::setCallback(ElementCallback callback)
// {
//     this->callback = callback;
// }

// template <typename T>
// void InteractableElement<T>::executeCallback(const T &value) const
// {
//     if (callback)
//     {
//         callback(value);
//     }
// }

// template <typename T>
// void InteractableElement<T>::accept(ElementVisitor& visitor)
// {
//     visitor.visit(*this);
// }


// // Implementation for the specialization
// InteractableElement<void>::InteractableElement(int size, String identifier, String label, ElementCallback callback)
//     : BaseElement(size, identifier, label), callback(callback)
// {
// }

// void InteractableElement<void>::setCallback(ElementCallback callback)
// {
//     this->callback = callback;
// }

// void InteractableElement<void>::executeCallback() const
// {
//     if (callback)
//     {
//         callback();
//     }
// }

// void InteractableElement<void>::accept(ElementVisitor& visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// Button::Button(int size, String identifier, String label, ElementCallback callback)
//     : InteractableElement(size, identifier, label, callback)
// {
// }

// String Button::toString()
// {
//     return "<button class=\"button-primary\" id=\"" + identifier + "\"  onclick=\"handleButton(event)\">" + label + "</button>";
// }

// void Button::accept(ElementVisitor &visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// TextBlock::TextBlock(int size, String identifier, String label)
//     : BaseElement(size, identifier, label)
// {
// }

// String TextBlock::toString()
// {
//     return "<label for=\"" + identifier + "\">" + label + ": </label> <p id=\"" + identifier + "\">Loading...</p>";
// }

// void TextBlock::accept(ElementVisitor &visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// TextInput::TextInput(int size, String identifier, String label, ElementCallback callback)
//     : InteractableElement(size, identifier, label, callback)
// {
// }

// TextInput::TextInput(int size, String identifier, String label, String placeholder, ElementCallback callback)
//     : InteractableElement(size, identifier, label, callback), placeholder(placeholder)
// {
// }

// String TextInput::toString()
// {
//     return "<label for=\"" + identifier + "\">" + label + "</label> <input type=\"text\" id=\"" + identifier + "\" placeholder=\"" + placeholder + "\" onkeydown=\"handleTextInput(event)\">";
// }

// void TextInput::accept(ElementVisitor &visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// Toggle::Toggle(int size, String identifier, String label, ElementCallback callback)
//     : InteractableElement(size, identifier, label, callback)
// {
// }

// String Toggle::toString()
// {
//     return "<label for=\"" + identifier + "\" class=\"toggle-switch\"> " + label + ": <input type=\"checkbox\" id=\"" + identifier + "\" onchange=\"handleToggle(event)\"><span class=\"slider\"> </span> </label>";
// }

// void Toggle::accept(ElementVisitor &visitor)
// {
//     visitor.visit(*this);
// }

// //---------------------------------------------------------

// #endif