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
    root = new MenuEntry::Parent("Main Menu");
    MenuEntry::Parent *focusers = new MenuEntry::Parent("Focusers",root);
    MenuEntry::Parent *options = new MenuEntry::Parent("Options", root);

    for(const Settings::Focuser &focuser: Settings::Instance.focusers()) {
        focusers->addChild(new MenuEntry::Focuser(focuser.name, focuser.address, focuser.port, focusers));
    }
    options->addChild((new MenuEntry::Functional("WiFi Info", options))
        ->on([](){
            char ipLine[256];
            char ssidLine[256];
            sprintf(ipLine, "IP: %s", WiFi.localIP().toString().c_str());
            sprintf(ssidLine, "SSID: %s", WiFi.SSID().c_str());
            Display::Instance.draw()
                .title("WiFi Info")
                .message({WiFi.getHostname(), ipLine, ssidLine})
                .footer({"<center> to exit"});
        }, MenuEntry::Functional::OnEnter)
        ->on([this, options](){ this->navigate(options); }, MenuEntry::Functional::Center)
    );
    options->addChild((new MenuEntry::Functional("Restart", options))
        ->on([](){
            Display::Instance.draw()
                .title("Restart")
                .message({"Are you sure?"})
                .footer({"<center> to restart", "<up> to exit"});
        }, MenuEntry::Functional::OnEnter)
        ->on([](){ ESP.restart(); }, MenuEntry::Functional::Center)
        ->on([this, options](){ navigate(options); }, MenuEntry::Functional::Up)
    );
    options->addChild((new MenuEntry::Functional("Sleep", options))
        ->on([this](){
            Log.traceln(LOGPREFIX "Sleeping via menu");
            this->sleep();
        }, MenuEntry::Functional::OnEnter)
    );
    
    MenuEntry::Functional *reconnectWiFi = (new MenuEntry::Functional("Reconnect WiFi", options))
        ->on([this](){
            Display::Instance.draw().title("WiFi Connecting").message({"Please wait..."});
            Log.infoln("Reconnecting WiFi");
            WiFiManager::Instance.reconnect();
        }, MenuEntry::Functional::OnEnter);
    
    options->addChild(reconnectWiFi);
    MenuEntry::Functional *wifiDisconnectedMenu = (new MenuEntry::Functional("WiFi Offline", nullptr))
        ->on([this, reconnectWiFi](){
            this->navigate(reconnectWiFi);
        }, MenuEntry::Functional::Center)
        ->on([](){
            Display::Instance.draw()
                .title("WiFi Disconnected")
                .footer({"<center> connect", "<up> sleep", "<down> restart"});
        }, MenuEntry::Functional::OnEnter)
        ->on([this](){ this->sleep(); }, MenuEntry::Functional::Up)
        ->on([](){ ESP.restart(); }, MenuEntry::Functional::Down);
    
    root->addChild(focusers)->addChild(options);
    WiFiManager::Instance.setOnConnectedCallback([this](){
        Display::Instance.draw().title("WiFi").message({"Connected to", WiFi.SSID().c_str()});
        this->oneshotTask = std::make_unique<Task>(5'000, TASK_ONCE, [this](){ this->navigate(root); }, &Global::scheduler, true);
    });
    WiFiManager::Instance.setOnConnectionFailedCallback([this, wifiDisconnectedMenu](){
        this->navigate(wifiDisconnectedMenu);
    });


    Log.traceln(LOGPREFIX "Menu entries added");
    navigate(reconnectWiFi);
    autoSleep();
    // new Task(500, TASK_FOREVER, [this](){
    //     Log.traceln(LOGPREFIX "Sleep task: interval=%d.%d", sleepTask->getInterval()/1000, sleepTask->getInterval()%1000);
    // }, &Global::scheduler, true);
    Log.traceln(LOGPREFIX "Setup complete");
}

void Nav::navigate(MenuEntry *menuEntry) {
    if(!menuEntry) {
        return;
    }
    if(this->menuEntry) {
        this->menuEntry->onExit();
    }
    this->menuEntry = menuEntry;
    menuEntry->onEnter();
    menuEntry->draw();
}

void Nav::sleep() {
    Log.traceln(LOGPREFIX "Preparing to sleep. Pin <%d> readout: %d", CENTRALBTN, digitalRead(CENTRALBTN));
    Display::Instance.draw().title("Sleep").message({"Turning off"}).footer({"Press <center>", "to wake"});
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

void Nav::onButton(Buttons::Button button, Buttons::Mode mode) {
    if(button == Buttons::UpLeft) {
        navigate(root);
        return;
    }
    if(button == Buttons::UpRight) {
        navigate(menuEntry->parent());
        return;
    }
    menuEntry->onButton(button, mode);
    autoSleep();
}
