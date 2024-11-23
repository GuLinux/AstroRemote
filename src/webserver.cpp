#include "webserver.h"
#include <wifimanager.h>
#include <functional>
#include <jsonresponse.h>
#include "settings.h"
#include <ArduinoLog.h>

using namespace std::placeholders;
using namespace GuLinux;

#define LOGPREFIX "[WebServer] "

WebServer::WebServer(): AsyncWebServerBase{} {

}

void WebServer::setup() {
    setupElegantOTA();
    setupJsonNotFoundPage();
    webserver.on("/api/wifi", HTTP_GET, std::bind(&WiFiManager::onGetWiFiStatus, &WiFiManager::Instance, _1));
    webserver.on("/api/config", HTTP_GET, std::bind(&WebServer::onGetConfig, this, _1));
    webserver.begin();
}

void WebServer::onGetConfig(AsyncWebServerRequest *request) {
    Log.infoln(LOGPREFIX "onGetConfig");
    JsonResponse response(request);
    Settings::Instance.asJson(response.root());
    String responseAsString;
    serializeJson(response.root(), responseAsString);
    Log.infoln(LOGPREFIX "response: %s", responseAsString.c_str());
}
