#include "nav_parent.h"



Nav::MenuEntry::Parent::Parent(const char * name, MenuEntry * parent) : _name{name}, parent{parent} {
}

Nav::MenuEntry::Parent *Nav::MenuEntry::Parent::addChild(MenuEntry *child) {
    this->children.push_back(child);
    return this;
}

void Nav::MenuEntry::Parent::draw() {
    if(children.size() > 0) {
        Display::Instance.drawOption(name(), children[currentIndex]->name(), hasPrev(), hasNext());
    } else {
        Display::Instance.drawOption(name(), "This menu is empty", false, false);
    }
}

void Nav::MenuEntry::Parent::left() {
    if(parent) {
        Nav::Instance.navigate(parent);
    }
}

void Nav::MenuEntry::Parent::right() {
    center();
}

void Nav::MenuEntry::Parent::up() {
    if(hasPrev())
        currentIndex--;
    draw();
}

void Nav::MenuEntry::Parent::down() {
    if(hasNext())
        currentIndex++;
    draw();
}

void Nav::MenuEntry::Parent::center() {
    Nav::Instance.navigate(children[currentIndex]);
}
