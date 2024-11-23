#pragma once
#include "nav.h"
#include "settings.h"
#include "protocols/indi/indiclient.h"
class Nav::MenuEntry::INDI : public Nav::MenuEntry {
public:
    INDI(const Settings::INDIServer &indiServerSettings);
    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode) override;
    void onEnter();

private:
    const Settings::INDIServer indiServerSettings;
    INDIClient indiClient;
};