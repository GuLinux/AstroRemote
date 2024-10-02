#pragma once
#include "nav.h"
#include "display.h"
#include "myfp2client.h"

#include <WString.h>
#include <unistd.h>
#include <array>

class Nav::MenuEntry::Focuser: public Nav::MenuEntry {
public:
    Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent);
    const String name() const override { return _name; }
    void draw() override;
    void left(Nav::PressMode mode) override;
    void right(Nav::PressMode mode) override;
    void up(Nav::PressMode mode) override;
    void down(Nav::PressMode mode) override;
    void center(Nav::PressMode mode) override;
    void onEnter() override;
    void onExit() override;
private:
    const String _name;
    MyFP2Client client;
    Nav::MenuEntry *parent;
    using AllowedSteps = std::array<uint16_t, 9>;
    AllowedSteps allowedSteps{1, 2, 5, 10, 20, 50, 100, 500, 1000};
    uint16_t steps() const { return allowedSteps[stepsIndex]; }
    uint8_t stepsIndex = 0;
    uint32_t position = 0;
    bool moving = false;
    bool waitingStatus = false;
    bool waitingPosition = false;
    void connect();
};
