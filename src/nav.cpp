#include "nav.h"
#include "myfp2client.h"
#include "display.h"
#include "pinout.h"
#include "settings.h"

#include <ArduinoLog.h>
#include <esp_sleep.h>

#define LOGPREFIX "[Nav] "
#define LOGPREFIXMENUENTRY "[Nav::MenuEntry] "

Nav &Nav::Instance = *new Nav();

Nav::MenuEntry &Nav::MenuEntry::addEntry(const MenuEntry &entry) {
    this->subEntries.push_back(entry);
    this->subEntries.back().parent = this;
    return *this;
}

Nav::Nav() : rootMenu{*this, "Main Menu"}, currentMenuEntry{rootMenu} {
}

void Nav::MenuEntry::draw() {
    Log.traceln(LOGPREFIXMENUENTRY "drawing entry, name=%s, index=%d", name, currentSubentryIndex);
    Log.traceln(LOGPREFIXMENUENTRY "selected submenu entry: name=%s, index=%d", subEntries[currentSubentryIndex].name, currentSubentryIndex);
    Display::Instance.drawOption(name, subEntries[currentSubentryIndex].name, hasPrev(), hasNext());
}

bool Nav::MenuEntry::hasNext() const {
    return currentSubentryIndex < subEntries.size()-1;
}

bool Nav::MenuEntry::hasPrev() const {
    return currentSubentryIndex > 0;
}

void Nav::MenuEntry::left() {
    if(onLeft) {
        onLeft();
        return;
    }
}

void Nav::MenuEntry::right() {
    if(onRight) {
        onRight();
        return;
    }
}

void Nav::MenuEntry::up() {
    if(onUp) {
        onUp();
        return;
    }
    if(!hasPrev()) {
        return;
    }
    this->currentSubentryIndex--;
    draw();
}

void Nav::MenuEntry::down() {
    if(onDown) {
        onDown();
        return;
    }
    if(!hasNext()) {
        return;
    }
    this->currentSubentryIndex++;
    draw();

}

void Nav::MenuEntry::enter() {
    if(onEnter) {
        onEnter();
        return;
    }
}

void Nav::setup() {
    Log.traceln(LOGPREFIX "Setup");
    rootMenu.addEntry({
        *this,
        "Focusers"
    }).addEntry({
        *this,
        "Options"
    });
    Log.traceln(LOGPREFIX "Menu entries added");
    currentMenuEntry.draw();
    Log.traceln(LOGPREFIX "Setup complete");
}


void Nav::left(PressMode mode) {
    Log.infoln(LOGPREFIX "LEFT clicked: %d", mode);
    currentMenuEntry.left();
}

void Nav::right(PressMode mode) {
    Log.infoln(LOGPREFIX "RIGHT clicked: %d", mode);
    currentMenuEntry.right();
}

void Nav::up(PressMode mode) {
    Log.infoln(LOGPREFIX "UP clicked: %d", mode);
    currentMenuEntry.up();
}

void Nav::down(PressMode mode) {
    Log.infoln(LOGPREFIX "DOWN clicked: %d", mode);
    currentMenuEntry.down();
}

void Nav::center(PressMode mode) {
      Log.infoln(LOGPREFIX "CENTER clicked: %d", mode);
      currentMenuEntry.enter();
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

