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

using namespace GuLinux;

WiFiMulti wifiMulti;
AsyncBufferedTCPLogger bufferedLogger(9911, 100);
Scheduler scheduler;

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
  while(!Serial);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  bufferedLogger.setup();
  Log.addHandler(&bufferedLogger);

  Log.infoln("Starting setup");
  Wire.begin(I2C_SDA, I2C_SCL);
  Display::Instance.setup();
  delay(5000);
  print_wakeup_reason();

  LittleFS.begin();
  Settings::Instance.setup();
  Nav::Instance.setup();
 
  uint8_t wifiSuccessful = setupWiFi(wifiMulti);
  Log.infoln("WiFi connected: %d, setting up OTA and buttons", wifiSuccessful);
  ArduinoOTAManager::Instance.setup();
  Buttons::Instance.setup();


  Log.infoln("Setup complete");
}

void loop() {
  ArduinoOTAManager::Instance.loop();
  scheduler.execute();
  Buttons::Instance.loop();
}
