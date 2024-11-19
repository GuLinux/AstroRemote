#include "nav_indi.h"

Nav::MenuEntry::INDI::INDI(const Settings::INDIServer &indiServer) : MenuEntry{indiServer.name}, indiServer{indiServer} {
}

void Nav::MenuEntry::INDI::draw()
{
}

void Nav::MenuEntry::INDI::onButton(Buttons::Button button, Buttons::Mode mode) {
}
