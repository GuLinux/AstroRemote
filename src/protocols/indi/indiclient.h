#ifndef INDI_CLIENT_PROTOCOL
#define INDI_CLIENT_PROTOCOL
#include <AsyncTCP.h>
#include "settings.h"
#include "indiparser.h"
#include <memory>
#include "indidevice.h"
#include <list>

class INDIClient {
public:
    using OnConnected = std::function<void()>;
    using OnDevicesReceived = std::function<void(const std::list<INDIDevice> &)>;
    INDIClient();
    bool connected() const { return client->connected(); }
    void connect(const Settings::INDIServer &indiServerSettings);
    void getDevices(const OnDevicesReceived &callback = {});
    void onConnected(const OnConnected &onConnected) { this->_onConnected = onConnected; };
    void disconnect();
private:
    INDIParser parser;
    using DataCb = std::function<void(const char *data, size_t len)>;
    std::unique_ptr<AsyncClient> client;
    OnConnected _onConnected;
    void onData(void*, AsyncClient*, void *data, size_t len);
    DataCb dataCb;
    std::list<INDIDevice> _devices;
};
#endif