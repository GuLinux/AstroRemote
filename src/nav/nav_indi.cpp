#include "nav_indi.h"
#include <ArduinoLog.h>

#define LOGSCOPE "NavIndi: "

Nav::MenuEntry::INDI::INDI(const Settings::INDIServer &indiServer) : MenuEntry{indiServer.name}, indiServerSettings{indiServer} {
}

void Nav::MenuEntry::INDI::draw() {
}

void Nav::MenuEntry::INDI::onButton(Buttons::Button button, Buttons::Mode mode) {
}

void Nav::MenuEntry::INDI::onEnter() {
    Log.infoln(LOGSCOPE "connecting to %s:%d", this->indiServerSettings.address.c_str(), this->indiServerSettings.port);
    indiClient.onConnected([this](){
        this->indiClient.getDevices();
    });
    indiClient.connect(indiServerSettings);
}
