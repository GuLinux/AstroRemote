#pragma once
#include "nav.h"
#include "settings.h"

class Nav::MenuEntry::INDI : public Nav::MenuEntry {
public:
    INDI(const Settings::INDIServer &indiServer);
    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode) override;

private:
    const Settings::INDIServer indiServer;
};