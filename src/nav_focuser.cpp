#include "nav_focuser.h"
#include <TaskSchedulerDeclarations.h>
#include "globals.h"


#include <ArduinoLog.h>

#define LOGPREFIX "[Nav::MenuEntry] "

Nav::MenuEntry::Focuser::Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent)
    : _name{name}, client{address, port}, parent{parent} {
        Log.infoln(LOGPREFIX "Creating focuser: name=%s, address=%s:%d", _name.c_str(), address, port);
        client
            .onConnected([this](){ draw(); })
            .onDisconnected([this](){
                if(client.status() != MyFP2Client::Disconnected) {
                    Display::Instance.drawMessage(_name.c_str(), "Disconnected!", "<center> to connect");
                }
            })
            .onError([this](uint8_t, const char *error){
                char message[256];
                sprintf(message, "Connection error:\n%s", error);
                Display::Instance.drawMessage(_name.c_str(), message, "<center> to connect");
            })
            .onPositionReceived([this](uint32_t position){ this->position = position; waitingPosition = false; draw(); })
            .onMovingStatusReceived([this](bool moving){
                this->moving = moving;
                waitingStatus = false;
                draw();
            })
        ;
        new Task(1000, TASK_FOREVER, [this](){
            if(client.status() != MyFP2Client::Connected) {
                return;
            }
            if(!waitingStatus) {
                Log.traceln(LOGPREFIX "Fetching moving status");
                waitingStatus = true;
                client.getMovingStatus();
            }
            if(!waitingPosition) {
                Log.traceln(LOGPREFIX "Fetching focuser position");
                waitingPosition = true;
                client.getPosition();
            }
        }, &Global::scheduler, true);
}

void Nav::MenuEntry::Focuser::draw()
{
    Log.traceln(LOGPREFIX "draw: %s", _name.c_str());
    if(client.status() == MyFP2Client::Connected) {
        Display::Instance.drawFocuser(_name.c_str(), position, steps(), moving);
    }
}

void Nav::MenuEntry::Focuser::left(Nav::PressMode mode) {
    Log.infoln(LOGPREFIX "focuser: left, %d", mode);
    if(mode == Single && client.status() == MyFP2Client::Connected && stepsIndex > 0) {
        stepsIndex--;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::right(Nav::PressMode mode) {
    Log.infoln(LOGPREFIX "focuser: right, %d", mode);
    if(mode == Single && client.status() == MyFP2Client::Connected && stepsIndex < allowedSteps.size()-1) {
        stepsIndex++;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::up(Nav::PressMode mode) {
    Log.infoln(LOGPREFIX "focuser: up, %d", mode);
    if(mode == Single && client.status() == MyFP2Client::Connected) {
        Log.infoln(LOGPREFIX "Moving focuser by %d steps - current position: %d", position, -steps());
        client.relativeMove(-steps());
    }
    if(mode == Long) {
        Nav::Instance.navigate(parent);
    }
}

void Nav::MenuEntry::Focuser::down(Nav::PressMode mode) {
    Log.infoln(LOGPREFIX "focuser: down, %d", mode);
    if(mode == Single && client.status() == MyFP2Client::Connected) {
        Log.infoln(LOGPREFIX "Moving focuser by %d steps - current position: %d", position, steps());
        client.relativeMove(steps());
    }
}

void Nav::MenuEntry::Focuser::center(Nav::PressMode mode) {
    Log.infoln(LOGPREFIX "focuser: center, %d", mode);
    if(mode == Single) {
        if(client.status() == MyFP2Client::Connected) {
            Log.infoln(LOGPREFIX "Aborting focuser motion");
            client.abort();
        }
        if(client.status() == MyFP2Client::Disconnected) {
            Log.infoln(LOGPREFIX "reconnecting");
            connect();
        }
    }
}

void Nav::MenuEntry::Focuser::onEnter() {
    Log.infoln(LOGPREFIX "focuser: onEnter");
    connect();
}

void Nav::MenuEntry::Focuser::onExit() {
    Log.infoln(LOGPREFIX "focuser: onExit");
    client.onDisconnected({});
    client.disconnect();
}

void Nav::MenuEntry::Focuser::connect()
{
    if(client.status() == MyFP2Client::Disconnected) {
        Display::Instance.drawMessage(_name.c_str(), "Connecting...");
        client.connect();
    }
}
