#ifndef MYFP2_CLIENT_H
#define MYFP2_CLIENT_H
#include <unistd.h>
#include <functional>
#include <WString.h>
#include <AsyncTCP.h>

class MyFP2Client {
public:
    enum Status { Disconnected, Connecting, Connected };
    using OnPositionReceived = std::function<void(uint32_t)>;
    using OnMaxStepReceived = std::function<void(uint32_t)>;
    using OnAckReceived = std::function<void()>;
    using OnError = std::function<void(int8_t, const char*)>;
    using OnMovingStatusReceived = std::function<void(bool)>;
    using OnCoilPowerReceived = std::function<void(bool)>;
    using OnMotorSpeedReceived = std::function<void(uint8_t)>;
    using OnVersionReceived = std::function<void(String)>;
    using OnConnected = std::function<void()>;
    using OnDisconnected = std::function<void()>;

    MyFP2Client(const String &hostname, uint16_t port=2020);

    void connect();
    void disconnect();

    void relativeMove(int32_t steps);
    void absoluteMove(int32_t steps);
    void abort();
    void getPosition();
    void ack();
    void getMovingStatus();
    void getCoilPower();
    void setCoilPower(bool coilPower);
    void getMotorSpeed();
    void setMotorSpeed(uint8_t speed);
    void getVersion();
    void getMaxStep();

    inline Status status() const { return _status; }

    MyFP2Client &onPositionReceived(const OnPositionReceived &cb) { _onPositionReceived = cb; return *this; }
    MyFP2Client &onAckReceived(const OnAckReceived &cb) { _onAckReceived = cb; return *this; }
    MyFP2Client &onMovingStatusReceived(const OnMovingStatusReceived &cb) { _onMovingStatusReceived = cb; return *this; }
    MyFP2Client &onCoilPowerReceived(const OnCoilPowerReceived &cb) { _onCoilPowerReceived = cb; return *this; }
    MyFP2Client &onMotorSpeedReceived(const OnMotorSpeedReceived &cb) { _onMotorSpeedReceived = cb; return *this; }
    MyFP2Client &onVersionReceived(const OnVersionReceived&cb) { _onVersionReceived = cb; return *this; }
    MyFP2Client &onConnected(const OnConnected&cb) { _onConnected = cb; return *this; }
    MyFP2Client &onDisconnected(const OnDisconnected&cb) { _onDisconnected = cb; return *this; }
    MyFP2Client &onMaxStepReceived(const OnMaxStepReceived&cb) { _onMaxStepReceived = cb; return *this; }
    MyFP2Client &onError(const OnError&cb) { _onError = cb; return *this; }

private:
    const String hostname;
    uint16_t port;
    OnPositionReceived _onPositionReceived;
    OnAckReceived _onAckReceived;
    OnMovingStatusReceived _onMovingStatusReceived;
    OnCoilPowerReceived _onCoilPowerReceived;
    OnMotorSpeedReceived _onMotorSpeedReceived;
    OnVersionReceived _onVersionReceived;
    OnConnected _onConnected;
    OnDisconnected _onDisconnected;
    OnMaxStepReceived _onMaxStepReceived;
    OnError _onError;

    void sendCommand(const char *format, ...);
    void onReceive(void* arg, AsyncClient* client, void *data, size_t len);
    AsyncClient client;
    Status _status = Disconnected;
};
#endif
