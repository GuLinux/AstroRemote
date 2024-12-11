#pragma once
#include "nav.h"
#include "nav_parent.h"
#include "settings.h"
#include "protocols/indi/indiclient.h"
#include <memory>

class Nav::MenuEntry::INDI : public Nav::MenuEntry {
public:
    INDI(const Settings::INDIServer &indiServerSettings);
    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode) override;
    void onEnter() override;
    void onExit() override;

private:
    const Settings::INDIServer indiServerSettings;
    INDIClient indiClient;
    std::unique_ptr<Parent> parent;
    void onDevicesReceived(const std::list<INDIDevice> &devices);
};