#include "nav_indi.h"
#include <ArduinoLog.h>
#include "hardware/display.h"
#include "nav_functional.h"

#define LOGSCOPE "NavIndi: "

using namespace std::placeholders;

Nav::MenuEntry::INDI::INDI(const Settings::INDIServer &indiServer) : MenuEntry{indiServer.name}, indiServerSettings{indiServer} {
}

void Nav::MenuEntry::INDI::draw() {
    if(parent) {
        parent->draw();
    }
}

void Nav::MenuEntry::INDI::onButton(Buttons::Button button, Buttons::Mode mode) {
    if(parent) {
        parent->onButton(button, mode);
    }
}

void Nav::MenuEntry::INDI::onEnter() {
    Log.infoln(LOGSCOPE "connecting to %s:%d", this->indiServerSettings.address.c_str(), this->indiServerSettings.port);
    Display::Instance.draw()
        .title(indiServerSettings.name.c_str())
        .message({"Connecting..."});
    indiClient.onConnected([this](){
        Display::Instance.draw()
            .title(indiServerSettings.name.c_str())
            .message({"Connected.", "Getting devices..."});
        this->indiClient.getDevices(std::bind(&INDI::onDevicesReceived, this, _1));
    });
    indiClient.connect(indiServerSettings);
}

void Nav::MenuEntry::INDI::onExit() {
    indiClient.disconnect();
}

void Nav::MenuEntry::INDI::onDevicesReceived(const std::list<INDIDevice> &devices) {
    Log.infoln(LOGSCOPE "onDevicesReceived: received %d devices", devices.size());
    parent = std::make_unique<Parent>(indiServerSettings.name);
    for(const auto &device: devices) {
        String interfaces;
        for(const auto &interface: device.interfaces()) {
            if(interfaces.isEmpty()) {
                interfaces += ", ";
                interfaces += interface._to_string();
            }
        }
        Log.infoln(LOGSCOPE "onDevicesReceived: device: name=%s", device.name.c_str(), interfaces);
        parent->addChild(new Functional(device.name.c_str()));
    }
    parent->draw();
}
