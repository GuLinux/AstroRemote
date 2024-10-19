#include "nav_parent.h"



Nav::MenuEntry::Parent::Parent(const String &name, MenuEntry * parent) : _name{name}, parent{parent} {
}

Nav::MenuEntry::Parent *Nav::MenuEntry::Parent::addChild(MenuEntry *child) {
    this->children.push_back(child);
    return this;
}

void Nav::MenuEntry::Parent::draw() {
    if(children.size() > 0) {
        Display::Instance.drawOption(_name.c_str(), children[currentIndex]->name().c_str(), hasPrev(), hasNext());
        Display::Instance.drawFooter("Press Up to go back, down/enter activates.");
    } else {
        Display::Instance.drawOption(_name.c_str(), "This menu is empty", false, false);
        Display::Instance.drawFooter("Press Up to go back");
    }
}

void Nav::MenuEntry::Parent::left(Buttons::Mode mode) {
    if(hasPrev())
        currentIndex--;
    draw();
}

void Nav::MenuEntry::Parent::right(Buttons::Mode mode) {
    if(hasNext())
        currentIndex++;
    draw();
}

void Nav::MenuEntry::Parent::up(Buttons::Mode mode) {
    if(parent) {
        Nav::Instance.navigate(parent);
    }
}

void Nav::MenuEntry::Parent::down(Buttons::Mode mode) {
    center(mode);
}

void Nav::MenuEntry::Parent::center(Buttons::Mode mode) {
    Nav::Instance.navigate(children[currentIndex]);
}
