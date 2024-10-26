#include "nav_functional.h"

Nav::MenuEntry::Functional::Functional(const String &name, MenuEntry *parent) : MenuEntry{name, parent} {
}

void Nav::MenuEntry::Functional::draw()
{
    invoke(Draw);
}

void Nav::MenuEntry::Functional::onButton(Buttons::Button button, Buttons::Mode mode) {
    switch (button) {
        case Buttons::Up:
            invoke(Up, mode);
            break;
        case Buttons::Down:
            invoke(Down, mode);
            break;
        case Buttons::Left:
            invoke(Left, mode);
            break;
        case Buttons::Right:
            invoke(Right, mode);
            break;
        case Buttons::Center:
            invoke(Center, mode);
            break;
    }
}

void Nav::MenuEntry::Functional::onEnter() {
    invoke(OnEnter);
}

void Nav::MenuEntry::Functional::onExit() {
    invoke(OnExit);
}

Nav::MenuEntry::Functional *Nav::MenuEntry::Functional::on(const Callback &callback, Action action, Buttons::Mode mode) {
    callbacks[getKey(action, mode)] = callback;
    return this;
}

inline uint16_t Nav::MenuEntry::Functional::getKey(Action action, Buttons::Mode mode) const {
    return static_cast<uint16_t>(action) * 100 + static_cast<uint16_t>(mode);
}

void Nav::MenuEntry::Functional::invoke(Action action, Buttons::Mode mode){
    optionalCall(callbacks[getKey(action, mode)]);
}
