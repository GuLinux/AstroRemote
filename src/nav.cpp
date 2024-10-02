#include "nav.h"
#include "myfp2client.h"
#include "display.h"
#include "pinout.h"
#include "settings.h"

#include <ArduinoLog.h>
#include <esp_sleep.h>
#include "nav_parent.h"
#include "nav_focuser.h"

#define LOGPREFIX "[Nav] "

Nav &Nav::Instance = *new Nav();



Nav::Nav() {
}

void Nav::setup() {
    Log.traceln(LOGPREFIX "Setup");
    MenuEntry::Parent *root = new MenuEntry::Parent("Main Menu");
    MenuEntry::Parent *focusers = new MenuEntry::Parent("Focusers",root);
    MenuEntry::Parent *options = new MenuEntry::Parent("Options", root);

    for(const Settings::Focuser &focuser: Settings::Instance.focusers()) {
        focusers->addChild(new MenuEntry::Focuser(focuser.name, focuser.address, focuser.port, focusers));
    }
    root->addChild(focusers)->addChild(options);

    Log.traceln(LOGPREFIX "Menu entries added");
    navigate(root);
    Log.traceln(LOGPREFIX "Setup complete");
}

void Nav::navigate(MenuEntry *menuEntry) {
    if(this->menuEntry) {
        this->menuEntry->onExit();
    }
    this->menuEntry = menuEntry;
    menuEntry->onEnter();
    menuEntry->draw();
}

void Nav::left(PressMode mode) {
    Log.infoln(LOGPREFIX "LEFT clicked: %d", mode);
    menuEntry->left(mode);
}

void Nav::right(PressMode mode) {
    Log.infoln(LOGPREFIX "RIGHT clicked: %d", mode);
    menuEntry->right(mode);
}

void Nav::up(PressMode mode) {
    Log.infoln(LOGPREFIX "UP clicked: %d", mode);
    menuEntry->up(mode);
}

void Nav::down(PressMode mode) {
    Log.infoln(LOGPREFIX "DOWN clicked: %d", mode);
    menuEntry->down(mode);
}

void Nav::center(PressMode mode) {
      Log.infoln(LOGPREFIX "CENTER clicked: %d", mode);
      menuEntry->center(mode);
//    if(mode == Single) {

//      MyFP2Client::Instance.abort();
//      return;
//    }
//    if(mode == Long) {
//      Log.traceln(LOGPREFIX "CENTERAL long press - Preparing to sleep. Pin 3 readout: %d", digitalRead(CENTRALBTN));
//      esp_deep_sleep_enable_gpio_wakeup(BIT(CENTRALBTN), ESP_GPIO_WAKEUP_GPIO_HIGH);
//      delay(100);
//      Display::Instance.sleep();
//      esp_deep_sleep_start();
//    }
}

