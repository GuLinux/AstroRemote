#pragma once
#include "nav.h"
#include "protocols/myfp2client.h"

#include <WString.h>
#include <unistd.h>
#include <array>

class Nav::MenuEntry::Focuser: public Nav::MenuEntry {
public:
    Focuser(const String &name, const String &address, uint16_t port);
    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode) override;
    void onEnter() override;
    void onExit() override;
private:
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
    void decreaseStepsSize();
    void increaseStepsSize();
    enum Direction { In = -1, Out = 1 };
    void move(Direction direction, uint8_t multiplier=1);
    void abort();
    void center(Buttons::Mode mode);
};
