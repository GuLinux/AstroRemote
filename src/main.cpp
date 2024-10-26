#include <Arduino.h>
#include <WiFiMulti.h>
#include <ArduinoLog.h>
#include <arduinoota-manager.h>
#include <ArduinoOTA.h>
#include <TaskScheduler.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <asyncbufferedtcplogger.h>

#include "pinout.h"
#include <Wire.h>


#include "myfp2client.h"
#include "nav.h"
#include "display.h"
#include "wifi-setup.h"
#include "settings.h"
#include "buttons.h"
#include "globals.h"
#include <wifimanager.h>

using namespace GuLinux;

void serialNav(char c);
WiFiMulti wifiMulti;
AsyncBufferedTCPLogger bufferedLogger(9911, 100, [](void*, AsyncClient*, void *data, size_t len){
  serialNav(static_cast<char*>(data)[0]);
});
Scheduler Global::scheduler;


#define BTN_PULLUP true
#define BTN_ACTIVELOW true

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
  //while(!Serial);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  bufferedLogger.setup();
  Log.addHandler(&bufferedLogger);

  Log.infoln("Starting setup");
  Wire.begin(I2C_SDA, I2C_SCL);
//   for(int8_t address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     if(Wire.endTransmission() == 0) {
//         Log.traceln("I2C Device found at address 0x%x", address);
//     }
//   }
  Display::Instance.setup();
  delay(5000);
  print_wakeup_reason();

  LittleFS.begin();
  Settings::Instance.setup();
  Nav::Instance.setup();
  WiFiManager::Instance.setup(Global::scheduler, &Settings::Instance.wifiSettings());
 
//   uint8_t wifiSuccessful = setupWiFi(wifiMulti);
//   Log.infoln("WiFi connected: %d, setting up OTA and buttons", wifiSuccessful);
  ArduinoOTAManager::Instance.setup();
  Buttons::Instance.setup();


  Log.infoln("Setup complete");
}

void loop() {
  WiFiManager::Instance.loop();
  ArduinoOTAManager::Instance.loop();
  Global::scheduler.execute();
  Buttons::Instance.loop();
  if(Serial.available()) {
      char c = Serial.read();
      serialNav(c);
  }
}

void serialNav(char c) {
    static const std::unordered_map<char, std::pair<Buttons::Button, Buttons::Mode>> buttonsMap {
      {'w', {Buttons::Up, Buttons::Single}},
      {'s', {Buttons::Down, Buttons::Single}},
      {'a', {Buttons::Left, Buttons::Single}},
      {'d', {Buttons::Right, Buttons::Single}},
      {'e', {Buttons::Center, Buttons::Single}},
      
      {'o', {Buttons::Center, Buttons::Double}},
      {'i', {Buttons::Up, Buttons::Double}},
      {'k', {Buttons::Down, Buttons::Double}},
      {'j', {Buttons::Left, Buttons::Double}},
      {'l', {Buttons::Right, Buttons::Double}},
    };
    if(buttonsMap.count(c)) {
      Nav::Instance.onButton(buttonsMap.at(c).first, buttonsMap.at(c).second);
    } else {
      Log.infoln("[SerialNav] Serial Keyboard Navigation:");
      Log.infoln("[SerialNav] Short click: w(up), a(left), d(right), s(down), e(center)");
      Log.infoln("[SerialNav] Long click: i(up), j(left), l(right), k(down), i(center)");
    }
}