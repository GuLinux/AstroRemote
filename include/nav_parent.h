#pragma once
#include "nav.h"
#include "display.h"

class Nav::MenuEntry::Parent : public Nav::MenuEntry {
public:
    Parent(const char *name, MenuEntry *parent=nullptr);

    const char *name() const override { return _name; }
    Parent *addChild(MenuEntry *child);
    void draw() override;

    void left() override;
    void right() override;
    void up() override;
    void down();
    void center() override;
private:
    bool hasPrev() const { return currentIndex > 0; }
    bool hasNext() const { return currentIndex < children.size()-1; }
    std::vector<MenuEntry*> children;
    const char *_name;
    uint8_t currentIndex = 0;
    MenuEntry *parent;
};

