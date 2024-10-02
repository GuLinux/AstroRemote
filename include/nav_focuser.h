#pragma once
#include "nav.h"
#include "display.h"
#include "myfp2client.h"

#include <WString.h>
#include <unistd.h>

class Nav::MenuEntry::Focuser: public Nav::MenuEntry {
public:
    Focuser(const char *name, const char *address, uint16_t port, MenuEntry *parent);
    const char *name() const override { return _name; }
    void draw() override;
    void left() override;
    void right() override;
    void up() override;
    void down() override;
    void center() override;
    void onEnter();
    void onExit();
private:
    const char *_name;
    MyFP2Client client;
    Nav::MenuEntry *parent;

};
