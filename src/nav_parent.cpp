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
        Display::Instance.drawFooter("Press Up to go back, down/enter activates.");
    } else {
        Display::Instance.drawOption(name(), "This menu is empty", false, false);
        Display::Instance.drawFooter("Press Up to go back");
    }
}

void Nav::MenuEntry::Parent::left() {
    if(hasPrev())
        currentIndex--;
    draw();
}

void Nav::MenuEntry::Parent::right() {
    if(hasNext())
        currentIndex++;
    draw();
}

void Nav::MenuEntry::Parent::up() {
    if(parent) {
        Nav::Instance.navigate(parent);
    }
}

void Nav::MenuEntry::Parent::down() {
    center();
}

void Nav::MenuEntry::Parent::center() {
    Nav::Instance.navigate(children[currentIndex]);
}
