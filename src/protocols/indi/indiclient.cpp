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

void INDIClient::disconnect() {
    client->close();
}


void INDIClient::getDevices(const OnDevicesReceived &callback) {
    if (!client->connected()) {
        Log.errorln(LOGSCOPE "getDevices error: client not connected");
        return;
    }
    client->write("<getProperties version='1.7'/>\n");
    this->_devices.clear();
    this->dataCb = [callback, this](const char *data, size_t len) {
        if(parser.parseDevices(data, len, std::back_inserter(this->_devices)) > 0 && callback){
            callback(this->_devices);
        }
    };
}

void INDIClient::onData(void *, AsyncClient *, void *data, size_t len) {
    // Log.traceln(LOGSCOPE "onData[%d bytes]: %s", len, (char*) data);
    if(dataCb) dataCb((const char*) data, len);
}

