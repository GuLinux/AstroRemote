#ifndef MYFP2_CLIENT_H
#define MYFP2_CLIENT_H
#include <unistd.h>
#include <functional>
#include <WString.h>
#include <AsyncTCP.h>

class MyFP2Client {
public:
    using OnPositionReceived = std::function<void(int32_t)>;
    using OnAckReceived = std::function<void()>;
    using OnMovingStatusReceived = std::function<void(bool)>;
    using OnCoilPowerReceived = std::function<void(bool)>;
    using OnMotorSpeedReceived = std::function<void(uint8_t)>;
    using OnVersionReceived = std::function<void(String)>;

    void connect(const char *hostname, uint16_t port=2020);

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

    inline bool connected() const { return _connected; }

    MyFP2Client &onPositionReceived(const OnPositionReceived &cb) { _onPositionReceived = cb; return *this; }
    MyFP2Client &onAckReceived(const OnAckReceived &cb) { _onAckReceived = cb; return *this; }
    MyFP2Client &onMovingStatusReceived(const OnMovingStatusReceived &cb) { _onMovingStatusReceived = cb; return *this; }
    MyFP2Client &onCoilPowerReceived(const OnCoilPowerReceived &cb) { _onCoilPowerReceived = cb; return *this; }
    MyFP2Client &onMotorSpeedReceived(const OnMotorSpeedReceived &cb) { _onMotorSpeedReceived = cb; return *this; }
    MyFP2Client &onVersionReceived(const OnVersionReceived&cb) { _onVersionReceived = cb; return *this; }

private:
    OnPositionReceived _onPositionReceived;
    OnAckReceived _onAckReceived;
    OnMovingStatusReceived _onMovingStatusReceived;
    OnCoilPowerReceived _onCoilPowerReceived;
    OnMotorSpeedReceived _onMotorSpeedReceived;
    OnVersionReceived _onVersionReceived;

    void sendCommand(const char *format, ...);
    void onReceive(void* arg, AsyncClient* client, void *data, size_t len);
    AsyncClient client;
    bool _connected = false;
};
#endif
