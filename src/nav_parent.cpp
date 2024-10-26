#include "nav_parent.h"



Nav::MenuEntry::Parent::Parent(const String &name, MenuEntry * parent) : MenuEntry{name, parent} {
}

Nav::MenuEntry::Parent *Nav::MenuEntry::Parent::addChild(MenuEntry *child) {
    this->children.push_back(child);
    return this;
}

void Nav::MenuEntry::Parent::draw() {
    if(children.size() > 0) {
        Display::Instance.draw()
            .title(_name.c_str())
            .nav(children[currentIndex]->name().c_str(), hasPrev(), hasNext())
            .footer({"Up: exit; Down: enter", "< previous; > next"});
    } else {
        Display::Instance.draw()
            .title(_name.c_str())
            .nav("This menu is empty", false, false)
            .footer({"Press Up to go back"});
    }
}

void Nav::MenuEntry::Parent::onButton(Buttons::Button button, Buttons::Mode mode) {
    switch(button) {
        case Buttons::Left:
        case Buttons::Up:
            if(hasPrev()) currentIndex--;
            draw();
            break;
        case Buttons::Right:
        case Buttons::Down:
            if(hasNext()) currentIndex++;
            draw();
            break;
        case Buttons::Center:
            Nav::Instance.navigate(children[currentIndex]);
            break;
    }
}

