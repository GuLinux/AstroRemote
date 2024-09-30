#include "myfp2client.h"
#include <ArduinoLog.h>

using namespace std::placeholders;

MyFP2Client &MyFP2Client::Instance = *new MyFP2Client();

void MyFP2Client::connect(const char *hostname, uint16_t port) {
    client.onData(std::bind(&MyFP2Client::onReceive, this, _1, _2, _3, _4));
    client.onConnect([hostname, port, this](void* arg, AsyncClient*){
        Log.infoln("Client connected to %s:%d!", hostname, port);
        ack();
    });
    client.onError([hostname, port](void*, AsyncClient*, int8_t error){
        Log.errorln("Error connecting to %s:%d: %d", hostname, port, error);
    });
    Log.infoln("Connecting to %s:%d", hostname, port);
    client.connect(hostname, port);
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
}

void MyFP2Client::getCoilPower() {
}

void MyFP2Client::setCoilPower(bool coilPower) {
}

void MyFP2Client::getMotorSpeed() {
}

void MyFP2Client::setMotorSpeed(uint8_t speed) {
}

void MyFP2Client::getVersion() {
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

void MyFP2Client::onReceive(void* arg, AsyncClient* client, void *data, size_t len) {
    Log.traceln("Received reply: len=%d", len);
    char *reply = (char*) data;
    for(size_t index=0; index<len; index++) {
        char dbgBuf[40];

        sprintf(dbgBuf, "[%05d] %03d 0x%02x %c", index, reply[index], reply[index], reply[index]);
        Log.traceln(dbgBuf);
    }

    if(reply[0] == 'E') {
        if(strcmp(reply, "EOK#") == 0) {
            if(!_connected) {
                _connected = true;
            }
            if(_onAckReceived) _onAckReceived();
        }
    }
    if(reply[0] == 'P') {
        int position;
        Log.traceln("Received position string command: %s", reply);
        if(auto result = sscanf(reply, "P%d#", &position) != EOF) {
            Log.traceln("get position detected: %d (%d)", position, result);
            //if(_onPositionReceived) _onPositionReceived(position);
        }
    }
}

