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


class Nav::MenuEntry::Parent : public Nav::MenuEntry {
public:
    Parent(const char *name, MenuEntry *parent=nullptr) : _name{name}, parent{parent} {
    }

    const char *name() const override { return _name; }
    Parent *addChild(MenuEntry *child) {
        this->children.push_back(child);
        return this;
    }
    void draw() override {
        if(children.size() > 0) {
            Display::Instance.drawOption(name(), children[currentIndex]->name(), hasPrev(), hasNext());
        } else {
            Display::Instance.drawOption(name(), "This menu is empty", false, false);
        }
    }

    void left() override {
        if(parent) {
            Nav::Instance.navigate(parent);
        }
    }
    void right() override {
        center();
    }
    void up() override {
        if(hasPrev())
            currentIndex--;
        draw();
    }

    void down() {
        if(hasNext())
            currentIndex++;
        draw();
    }
    void center() override {
        Nav::Instance.navigate(children[currentIndex]);
    }
private:
    bool hasPrev() const { return currentIndex > 0; }
    bool hasNext() const { return currentIndex < children.size()-1; }
    std::vector<MenuEntry*> children;
    const char *_name;
    uint8_t currentIndex = 0;
    MenuEntry *parent;
};


class Nav::MenuEntry::Focuser: public Nav::MenuEntry {
public:
    Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent) : _name{name}, parent{parent} {

    }

    const char *name() const override {
        return _name.c_str();
    }
    void draw() override {
        Display::Instance.drawOption(name(), "Pos:12345\nSteps:10", false, false);
    }
    void left() override {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: left");
    }
    void right() override {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: right");
    }
    void up() override {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: up");
    }
    void down() override {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: down");
    }
    void center() override {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: center");
        Nav::Instance.navigate(parent);
    }
    void onEnter() {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: onEnter");
    }
    void onExit() {
        Log.infoln(LOGPREFIXMENUENTRY "focuser: onExit");
    }
private:
    const String _name;
    MenuEntry *parent;
};

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
    menuEntry->left();
}

void Nav::right(PressMode mode) {
    Log.infoln(LOGPREFIX "RIGHT clicked: %d", mode);
    menuEntry->right();
}

void Nav::up(PressMode mode) {
    Log.infoln(LOGPREFIX "UP clicked: %d", mode);
    menuEntry->up();
}

void Nav::down(PressMode mode) {
    Log.infoln(LOGPREFIX "DOWN clicked: %d", mode);
    menuEntry->down();
}

void Nav::center(PressMode mode) {
      Log.infoln(LOGPREFIX "CENTER clicked: %d", mode);
      menuEntry->center();
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

