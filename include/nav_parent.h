#pragma once
#include "nav.h"
#include "display.h"

class Nav::MenuEntry::Parent : public Nav::MenuEntry {
public:
    Parent(const String &name, MenuEntry *parent=nullptr);

    const String name() const override { return _name; }
    Parent *addChild(MenuEntry *child);
    void draw() override;

    void left(Buttons::Mode mode) override;
    void right(Buttons::Mode mode) override;
    void up(Buttons::Mode mode) override;
    void down(Buttons::Mode mode);
    void center(Buttons::Mode mode) override;
private:
    bool hasPrev() const { return currentIndex > 0; }
    bool hasNext() const { return currentIndex < children.size()-1; }
    std::vector<MenuEntry*> children;
    const String _name;
    uint8_t currentIndex = 0;
    MenuEntry *parent;
};

