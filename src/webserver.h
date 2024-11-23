#ifndef SRC_WEBSERVER_H
#define SRC_WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <AsyncWebServerBase.h>

class WebServer : public AsyncWebServerBase {
public:
    WebServer();
    void setup();
private:
    void onGetConfig(AsyncWebServerRequest *request);
};

#endif