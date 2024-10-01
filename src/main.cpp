#include <Arduino.h>
#include <WiFiMulti.h>
#include <ArduinoLog.h>
#include <arduinoota-manager.h>
#include <ArduinoOTA.h>
#include <TaskScheduler.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <asyncbufferedtcplogger.h>
#include <OneButton.h>
#include "pinout.h"
#include <Wire.h>
#include <esp_sleep.h>

#include "myfp2client.h"
#include "display.h"
#include "wifi-setup.h"
#include "settings.h"

using namespace GuLinux;
MyFP2Client myFP2;

WiFiMulti wifiMulti;
AsyncBufferedTCPLogger bufferedLogger(9911, 100);
Scheduler scheduler;
Display display;

#define BTN_PULLUP true
#define BTN_ACTIVELOW true

OneButton upButton;
OneButton downButton;
OneButton leftButton;
OneButton centralButton;
OneButton rightButton;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Log.infoln("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Log.infoln("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Log.infoln("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Log.infoln("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Log.infoln("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_GPIO: Log.infoln("Wakeup caused by GPIO"); break;
    default : Log.infoln("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(5000);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  bufferedLogger.setup();
  Log.addHandler(&bufferedLogger);

  print_wakeup_reason();

  Log.infoln("Starting setup");
  LittleFS.begin();
  Settings::Instance.setup();
  Wire.begin(I2C_SDA, I2C_SCL);
 
  uint8_t wifiSuccessful = setupWiFi(wifiMulti);
  Log.infoln("WiFi connected: %d, setting up OTA and buttons", wifiSuccessful);
  ArduinoOTAManager::Instance.setup();

  upButton.setup(UPBTN, INPUT_PULLDOWN, false);
  downButton.setup(DOWNBTN, INPUT_PULLDOWN, false);
  leftButton.setup(LEFTBTN, INPUT_PULLDOWN, false);
  rightButton.setup(RIGHTBTN, INPUT_PULLDOWN, false);
  centralButton.setup(CENTRALBTN, INPUT_PULLDOWN, false);

  upButton.attachClick([](){ Log.infoln("UP clicked"); myFP2.relativeMove(200);});
  downButton.attachClick([](){ Log.infoln("DOWN clicked"); myFP2.relativeMove(-200);});
  centralButton.attachClick([](){ Log.infoln("CENTRAL clicked"); myFP2.abort(); });
  centralButton.attachLongPressStart([](){ Log.infoln("CENTRAL longPress: will deep sleep"); });
  centralButton.attachLongPressStop([](){
          Log.traceln("Preparing to sleep. Pin 3 readout: %d", digitalRead(CENTRALBTN));
          esp_deep_sleep_enable_gpio_wakeup(BIT(CENTRALBTN), ESP_GPIO_WAKEUP_GPIO_HIGH);
          delay(100);
          display.sleep();
          esp_deep_sleep_start();
  });
  leftButton.attachClick([](){ Log.infoln("LEFT clicked"); myFP2.relativeMove(-20); });
  rightButton.attachClick([](){ Log.infoln("RIGHT clicked"); myFP2.relativeMove(20); });
  display.setup();

  Log.infoln("Setup complete");
  myFP2.onAckReceived([](){ Log.infoln("Ack received!");}).onPositionReceived([](int32_t position){ Log.infoln("Received position: %d", position); });
  myFP2.connect("192.168.1.195");
  
}

void loop() {
  static uint64_t lastPosQuery = 0;
  if(millis() - lastPosQuery > 10000 && myFP2.connected()) {
    lastPosQuery = millis();
    myFP2.getPosition();
    myFP2.getMovingStatus();
  }
  
  ArduinoOTAManager::Instance.loop();
  scheduler.execute();
  upButton.tick();
  downButton.tick();
  leftButton.tick();
  centralButton.tick();
  rightButton.tick();
}
