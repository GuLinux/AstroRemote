#include "nav.h"
#include "myfp2client.h"
#include "display.h"
#include "pinout.h"

#include <ArduinoLog.h>
#include <esp_sleep.h>

Nav &Nav::Instance = *new Nav();

void Nav::left(PressMode mode) {
    Log.infoln("LEFT clicked");
    MyFP2Client::Instance.relativeMove(-20);
}

void Nav::right(PressMode mode) {
    Log.infoln("RIGHT clicked");
    MyFP2Client::Instance.relativeMove(20);
}

void Nav::up(PressMode mode) {
    Log.infoln("UP clicked");
    MyFP2Client::Instance.relativeMove(200);
}

void Nav::down(PressMode mode) {
    Log.infoln("DOWN clicked");
    MyFP2Client::Instance.relativeMove(-200);
}

void Nav::center(PressMode mode) {
    if(mode == Single) {
      Log.infoln("CENTER clicked");
      MyFP2Client::Instance.abort();
      return;
    }
    if(mode == Long) {
      Log.traceln("CENTERAL long press - Preparing to sleep. Pin 3 readout: %d", digitalRead(CENTRALBTN));
      esp_deep_sleep_enable_gpio_wakeup(BIT(CENTRALBTN), ESP_GPIO_WAKEUP_GPIO_HIGH);
      delay(100);
      Display::Instance.sleep();
      esp_deep_sleep_start();
    }
}

