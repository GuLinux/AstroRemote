#pragma once
#include "nav.h"
#include "display.h"

class Nav::MenuEntry::Parent : public Nav::MenuEntry {
public:
    Parent(const String &name, MenuEntry *parent=nullptr);

    const String name() const override { return _name; }
    Parent *addChild(MenuEntry *child);
    void draw() override;

    void left(Nav::PressMode mode) override;
    void right(Nav::PressMode mode) override;
    void up(Nav::PressMode mode) override;
    void down(Nav::PressMode mode);
    void center(Nav::PressMode mode) override;
private:
    bool hasPrev() const { return currentIndex > 0; }
    bool hasNext() const { return currentIndex < children.size()-1; }
    std::vector<MenuEntry*> children;
    const String _name;
    uint8_t currentIndex = 0;
    MenuEntry *parent;
};

