#include "nav_focuser.h"
#include <ArduinoLog.h>

#define LOGPREFIX "[Nav::MenuEntry] "

Nav::MenuEntry::Focuser::Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent) : _name{name}, parent{parent} {
}

void Nav::MenuEntry::Focuser::draw()
{
    Display::Instance.drawOption(name(), "Pos:12345\nSteps:10", false, false);
}

void Nav::MenuEntry::Focuser::left() {
    Log.infoln(LOGPREFIX "focuser: left");
}

void Nav::MenuEntry::Focuser::right() {
    Log.infoln(LOGPREFIX "focuser: right");
}

void Nav::MenuEntry::Focuser::up() {
    Log.infoln(LOGPREFIX "focuser: up");
}

void Nav::MenuEntry::Focuser::down() {
    Log.infoln(LOGPREFIX "focuser: down");
}

void Nav::MenuEntry::Focuser::center() {
    Log.infoln(LOGPREFIX "focuser: center");
    Nav::Instance.navigate(parent);
}

void Nav::MenuEntry::Focuser::onEnter() {
    Log.infoln(LOGPREFIX "focuser: onEnter");
}

void Nav::MenuEntry::Focuser::onExit() {
    Log.infoln(LOGPREFIX "focuser: onExit");
}
