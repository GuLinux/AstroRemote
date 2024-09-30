#include "buttons.h"
#include <ArduinoLog.h>
#include "myfp2client.h"
#include "pinout.h"
#include "display.h"

Buttons &Buttons::Instance = *new Buttons();

void Buttons::setup() {
  upButton.setup(UPBTN, INPUT_PULLDOWN, false);
  downButton.setup(DOWNBTN, INPUT_PULLDOWN, false);
  leftButton.setup(LEFTBTN, INPUT_PULLDOWN, false);
  rightButton.setup(RIGHTBTN, INPUT_PULLDOWN, false);
  centralButton.setup(CENTRALBTN, INPUT_PULLDOWN, false);

  upButton.attachClick([](){ Log.infoln("UP clicked"); MyFP2Client::Instance.relativeMove(200);});
  downButton.attachClick([](){ Log.infoln("DOWN clicked"); MyFP2Client::Instance.relativeMove(-200);});
  centralButton.attachClick([](){ Log.infoln("CENTRAL clicked"); MyFP2Client::Instance.abort(); });
  centralButton.attachLongPressStart([](){ Log.infoln("CENTRAL longPress: will deep sleep"); });
  centralButton.attachLongPressStop([](){
          Log.traceln("Preparing to sleep. Pin 3 readout: %d", digitalRead(CENTRALBTN));
          esp_deep_sleep_enable_gpio_wakeup(BIT(CENTRALBTN), ESP_GPIO_WAKEUP_GPIO_HIGH);
          delay(100);
          Display::Instance.sleep();
          esp_deep_sleep_start();
  });
  leftButton.attachClick([](){ Log.infoln("LEFT clicked"); MyFP2Client::Instance.relativeMove(-20); });
  rightButton.attachClick([](){ Log.infoln("RIGHT clicked"); MyFP2Client::Instance.relativeMove(20); });
}

void Buttons::loop() {
  upButton.tick();
  downButton.tick();
  leftButton.tick();
  centralButton.tick();
  rightButton.tick();

}
