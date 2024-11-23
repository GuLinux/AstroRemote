#ifndef INDI_CLIENT_PROTOCOL
#define INDI_CLIENT_PROTOCOL
#include <AsyncTCP.h>
#include "settings.h"
#include "indiparser.h"
#include <memory>

class INDIClient {
public:
    using OnConnected = std::function<void()>;
    INDIClient();
    bool connected() const { return client->connected(); }
    void connect(const Settings::INDIServer &indiServerSettings);
    void getDevices();
    void onConnected(const OnConnected &onConnected) { this->_onConnected = onConnected; };
private:
    INDIParser parser;
    using DataCb = std::function<void(const char *data, size_t len)>;
    std::unique_ptr<AsyncClient> client;
    OnConnected _onConnected;
    void onData(void*, AsyncClient*, void *data, size_t len);
    void parseDevices(const char *data, size_t len);
    DataCb dataCb;
};
#endif