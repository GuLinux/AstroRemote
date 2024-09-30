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
#include <esp_sleep.h>

#include "myfp2client.h"
#include "display.h"
#include "wifi-setup.h"
#include "settings.h"
#include "buttons.h"

using namespace GuLinux;

WiFiMulti wifiMulti;
AsyncBufferedTCPLogger bufferedLogger(9911, 100);
Scheduler scheduler;
Display display;

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
  Buttons::Instance.setup();
  display.setup();

  Log.infoln("Setup complete");
  MyFP2Client::Instance.onAckReceived([](){ Log.infoln("Ack received!");}).onPositionReceived([](int32_t position){ Log.infoln("Received position: %d", position); });
  MyFP2Client::Instance.connect("192.168.1.195");
  
}

void loop() {
  static uint64_t lastPosQuery = 0;
  if(millis() - lastPosQuery > 10000 && MyFP2Client::Instance.connected()) {
    lastPosQuery = millis();
    MyFP2Client::Instance.getPosition();
    MyFP2Client::Instance.getMovingStatus();
  }
  
  ArduinoOTAManager::Instance.loop();
  scheduler.execute();
  Buttons::Instance.loop();
}
