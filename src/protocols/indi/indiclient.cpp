#include "indiclient.h"
#include <ArduinoLog.h>
#include <IPAddress.h>

#define LOGSCOPE "INDI: "
using namespace std::placeholders;

INDIClient::INDIClient() {
    client = std::make_unique<AsyncClient>();
    client->onConnect([this](void*, AsyncClient*){
        Log.traceln(LOGSCOPE "connected to %s:%d", IPAddress(client->getRemoteAddress()).toString().c_str(), client->getRemotePort());
        if(this->_onConnected) {
            this->_onConnected();
        }
    });
    client->onData(std::bind(&INDIClient::onData, this, _1, _2, _3, _4));
}

void INDIClient::connect(const Settings::INDIServer &indiServerSettings) {
    client->connect(indiServerSettings.address.c_str(), indiServerSettings.port);
}

void INDIClient::getDevices() {
    if (!client->connected()) {
        Log.errorln(LOGSCOPE "getDevices error: client not connected");
        return;
    }
    client->write("<getProperties version='1.7'/>\n");
    this->dataCb = std::bind(&INDIClient::parseDevices, this, _1, _2);
}

void INDIClient::onData(void *, AsyncClient *, void *data, size_t len) {
    Log.traceln(LOGSCOPE "onData[%d bytes]: %s", len, (char*) data);
    if(dataCb) dataCb((const char*) data, len);
}

void INDIClient::parseDevices(const char *data, size_t len) {
    Log.traceln(LOGSCOPE "**** Parsing devices list");
    std::list<std::string> devices = parser.parseDevices(data, len);
    // tinyxml2::XMLDocument xmlDoc;
    // xmlDoc.Parse(data);
    // tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
    // while(element) {
    //     Log.traceln("Element: %s", element->Name());
    //     element = element->NextSiblingElement();
    // }
}
