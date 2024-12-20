#pragma once
#include "nav.h"

class Nav::MenuEntry::Parent : public Nav::MenuEntry {
public:
    Parent(const String &name);

    Parent *addChild(MenuEntry *child);

    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode) override;
private:
    bool hasPrev() const { return currentIndex > 0; }
    bool hasNext() const { return currentIndex < children.size()-1; }
    std::vector<MenuEntry*> children;
    uint8_t currentIndex = 0;
};

