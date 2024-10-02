#pragma once
#include "nav.h"
#include "display.h"
#include <WString.h>
#include <unistd.h>

class Nav::MenuEntry::Focuser: public Nav::MenuEntry {
public:
    Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent);
    const char *name() const override { return _name.c_str(); }
    void draw() override;
    void left() override;
    void right() override;
    void up() override;
    void down() override;
    void center() override;
    void onEnter();
    void onExit();
private:
    const String _name;
    Nav::MenuEntry *parent;
};
