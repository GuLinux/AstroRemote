#ifndef SRC_WEBSERVER_H
#define SRC_WEBSERVER_H
#include <ESPAsyncWebServer.h>

class WebServer {
public:
    WebServer();
    void setup();
private:
    AsyncWebServer webserver;
};

#endif