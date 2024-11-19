#include "myfp2client.h"
#include <ArduinoLog.h>
#include "utils.h"

using namespace std::placeholders;

MyFP2Client::MyFP2Client(const String &hostname, uint16_t port) : hostname{hostname}, port{port} {
}

void MyFP2Client::connect() {
    _status = Connecting;
    client.onData(std::bind(&MyFP2Client::onReceive, this, _1, _2, _3, _4));
    client.onConnect([this](void* arg, AsyncClient*){
        Log.infoln("Client connected to %s:%d!", hostname.c_str(), port);
        ack();
    });
    client.onDisconnect([this](void *arg, AsyncClient*){
        Log.infoln("Client disconnected from %s:%d!", hostname.c_str(), port);
        _status = Disconnected;
        optionalCall(_onDisconnected);
    });
    client.onError([this](void*, AsyncClient*, int8_t error){
        Log.errorln("Socket connection error on %s:%d: %d", hostname.c_str(), port, error);
        _status = Disconnected;
        optionalCall(_onError, error, client.errorToString(error));
        
    });
    Log.infoln("Connecting to %s:%d", hostname.c_str(), port);
    client.connect(hostname.c_str(), port);
}

void MyFP2Client::disconnect() {
    if(_status != Disconnected) {
        Log.infoln("Disconnecting from %d:%d", hostname.c_str(), port);
        client.close();
        
    }
}

void MyFP2Client::relativeMove(int32_t steps) {
    Log.traceln("relativeMove: %d steps", steps);
    sendCommand(":64%d#", steps);
}

void MyFP2Client::absoluteMove(int32_t steps) {
    Log.traceln("absoluteMove: %d steps", steps);
    sendCommand(":05%d#", steps);
}

void MyFP2Client::abort() {
    sendCommand(":27#");
}

void MyFP2Client::getPosition() {
    sendCommand(":00#");
}

void MyFP2Client::ack() {
    sendCommand(":02#");
}

void MyFP2Client::getMovingStatus() {
    sendCommand(":01#");
}

void MyFP2Client::getCoilPower() {
    sendCommand(":11#");
}

void MyFP2Client::setCoilPower(bool coilPower) {
    sendCommand(":12%d#", coilPower ? 1 : 0);
}

void MyFP2Client::getMotorSpeed() {
    sendCommand(":43#");
}

void MyFP2Client::setMotorSpeed(uint8_t speed) {
    sendCommand(":15%d#", speed);
}

void MyFP2Client::getVersion() {
    sendCommand(":03#");
}

void MyFP2Client::getMaxStep() {
    sendCommand(":08#");
}


void MyFP2Client::sendCommand(const char *format, ...){
    va_list(args);
    va_start(args, format);
    char outBuffer[256];
    size_t outBufferPos = vsprintf(outBuffer, format, args);
    va_end(args);
    Log.traceln("Sending data: %s", outBuffer);
    client.add(outBuffer, outBufferPos);
}

template<typename T, typename F> void parseReply(const char *reply, const char *parsingTemplate, const F &f) {
    T t;
    int result;
    Log.traceln("Parsing result string: %s", reply);
    if(result = sscanf(reply, parsingTemplate, &t) != EOF) {
        optionalCall(f, t);
    } else {
        Log.warningln("Unable to parse %s: %d", reply, result);
    }
}

void MyFP2Client::onReceive(void* arg, AsyncClient* client, void *data, size_t len) {
    Log.traceln("Received reply: len=%d", len);
    char reply[len+1]{0};
    strncpy(reply, (char*)data, len);
    for(size_t index=0; index<len; index++) {
        char dbgBuf[40];

        sprintf(dbgBuf, "[%05d] %03d 0x%02x %c", index, reply[index], reply[index], reply[index]);
        Log.traceln(dbgBuf);
    }
    Log.traceln("Reply parsed: %s, reply command=%c", reply, reply[0]);
    if(reply[0] == 'E') {
        if(strcmp(reply, "EOK#") == 0) {
            if(!_status != Connected) {
                _status = Connected;
                optionalCall(_onConnected);
            }
            optionalCall(_onAckReceived);
        }
    }
    if(reply[0] == 'P') {
        Log.infoln("Received position string command: %s", reply);
        parseReply<uint32_t>(reply, "P%d#", _onPositionReceived);
    }
    if(reply[0] == 'I') {
        Log.infoln("Received moving status command reply: %s", reply);
        parseReply<uint8_t>(reply, "I%d#", _onMovingStatusReceived); 
    }
    if(reply[0] == 'M') {
        Log.infoln("Received max step command reply: %s", reply);
        parseReply<uint32_t>(reply, "M%d#", _onMaxStepReceived);
    }
    if(reply[0] == 'C') {
        Log.infoln("Received get motor speed command reply: %s", reply);
        parseReply<uint8_t>(reply, "C%d#", _onMotorSpeedReceived);
    }
    if(reply[0] == 'O') {
        Log.infoln("Received get coil power command reply: %s", reply);
        parseReply<uint8_t>(reply, "O%d#", _onCoilPowerReceived);
    }

    if(reply[0] == 'F') {
        Log.infoln("Received moving status command reply: %s", reply);
        parseReply<char[20]>(reply, "I%s#", _onVersionReceived);
    }
}

