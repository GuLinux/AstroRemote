#include "nav.h"
#include "myfp2client.h"
#include "display.h"
#include "pinout.h"
#include "settings.h"

#include <ArduinoLog.h>
#include <esp_sleep.h>
#include "nav_parent.h"
#include "nav_focuser.h"
#include "nav_functional.h"
#include "wifimanager.h"
#include "globals.h"

#define LOGPREFIX "[Nav] "

using namespace GuLinux;

Nav &Nav::Instance = *new Nav();

#define sleepDelay 300'000

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
    options->addChild((new Nav::MenuEntry::Functional("Sleep"))
        ->on([this](){
            Log.traceln(LOGPREFIX "Sleeping via menu");
            this->sleep();
        }, MenuEntry::Functional::OnEnter)
    );
    Nav::MenuEntry::Parent *wifiConnectingMenu = (new Nav::MenuEntry::Parent("WiFi Connecting"))->addChild(new MenuEntry::Functional("Please wait..."));
    Nav::MenuEntry::Functional *reconnectWiFi = (new Nav::MenuEntry::Functional("Reconnect WiFi"))
        ->on([this, wifiConnectingMenu](){
            this->navigate(wifiConnectingMenu);
            Log.infoln("Reconnecting WiFi");
            WiFiManager::Instance.reconnect();
        }, MenuEntry::Functional::OnEnter);
    
    options->addChild(reconnectWiFi);
    Nav::MenuEntry::Parent *wifiDisconnectedMenu = (new Nav::MenuEntry::Parent("WiFi Offline"))->addChild(reconnectWiFi);
    
    root->addChild(focusers)->addChild(options);
    WiFiManager::Instance.setOnConnectedCallback([this, root](){
        Display::Instance.drawMessage("WiFi", "Connection Successful to %s", WiFi.SSID().c_str());
        this->oneshotTask = std::make_unique<Task>(5'000, TASK_ONCE, [this, root](){ this->navigate(root); }, &Global::scheduler, true);
    });
    WiFiManager::Instance.setOnConnectionFailedCallback([this, wifiDisconnectedMenu](){
        Display::Instance.drawMessage("WiFi", "Connection failed");
        delay(2000);
        this->navigate(wifiDisconnectedMenu);
    });


    Log.traceln(LOGPREFIX "Menu entries added");
    navigate(wifiConnectingMenu);
    autoSleep();
    // new Task(500, TASK_FOREVER, [this](){
    //     Log.traceln(LOGPREFIX "Sleep task: interval=%d.%d", sleepTask->getInterval()/1000, sleepTask->getInterval()%1000);
    // }, &Global::scheduler, true);
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

void Nav::sleep() {
    Log.traceln(LOGPREFIX "Preparing to sleep. Pin <%d> readout: %d", CENTRALBTN, digitalRead(CENTRALBTN));
    Display::Instance.drawMessage("Sleep", "Turning off", "Press <center> to wake");
    esp_err_t enableWakeuperr = esp_deep_sleep_enable_gpio_wakeup(BIT(CENTRALBTN), ESP_GPIO_WAKEUP_GPIO_HIGH);
    if(enableWakeuperr != ESP_OK) {
        Log.errorln("Error %d enabling wakeup from pin <%d>: %s", enableWakeuperr, CENTRALBTN, esp_err_to_name(enableWakeuperr));
    }
    delay(2000);
    
    Display::Instance.sleep();
    esp_deep_sleep_start();
}

void Nav::autoSleep() {
    if(sleepTask) {
        sleepTask->cancel();
    }
    sleepTask = std::make_unique<Task>(0, TASK_ONCE, [this](){
        Log.traceln(LOGPREFIX "Sleeping via timer");
        this->sleep();
    }, &Global::scheduler, false);
    sleepTask->enableDelayed(sleepDelay);
}

void Nav::left(PressMode mode) {
    Log.infoln(LOGPREFIX "LEFT clicked: %d", mode);
    menuEntry->left(mode);
    autoSleep();
}

void Nav::right(PressMode mode) {
    Log.infoln(LOGPREFIX "RIGHT clicked: %d", mode);
    menuEntry->right(mode);
    autoSleep();
}

void Nav::up(PressMode mode) {
    Log.infoln(LOGPREFIX "UP clicked: %d", mode);
    menuEntry->up(mode);
    autoSleep();
}

void Nav::down(PressMode mode) {
    Log.infoln(LOGPREFIX "DOWN clicked: %d", mode);
    menuEntry->down(mode);
    autoSleep();
}

void Nav::center(PressMode mode) {
    Log.infoln(LOGPREFIX "CENTER clicked: %d", mode);
    menuEntry->center(mode);
    autoSleep();
}

