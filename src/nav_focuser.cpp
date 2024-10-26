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
                    Display::Instance.draw().title(_name.c_str()).message({"Disconnected!"}).footer({"<center> to connect"});
                }
            })
            .onError([this](uint8_t, const char *error){
                Display::Instance.draw().title(_name.c_str()).message({"Connection error", error}).footer({"<center> to connect"});
            })
            .onPositionReceived([this](uint32_t position){
                Log.traceln(LOGPREFIX "on position received: position=%d", position);
                this->position = position;
                waitingPosition = false;
                draw();
            })
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
        Display::Instance.draw().title(_name.c_str()).focuser(position, steps(), moving);
    }
}

void Nav::MenuEntry::Focuser::left(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: left, %d", mode);
    if(mode == Buttons::Single && client.status() == MyFP2Client::Connected && stepsIndex > 0) {
        stepsIndex--;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::right(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: right, %d", mode);
    if(mode == Buttons::Single && client.status() == MyFP2Client::Connected && stepsIndex < allowedSteps.size()-1) {
        stepsIndex++;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::up(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: up, %d", mode);
    if(mode == Buttons::Single && client.status() == MyFP2Client::Connected) {
        Log.infoln(LOGPREFIX "Moving focuser by %d steps - current position: %d", position, -steps());
        client.relativeMove(-steps());
    }
    if(mode == Buttons::Long) {
        Nav::Instance.navigate(parent);
    }
}

void Nav::MenuEntry::Focuser::down(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: down, %d", mode);
    if(mode == Buttons::Single && client.status() == MyFP2Client::Connected) {
        Log.infoln(LOGPREFIX "Moving focuser by %d steps - current position: %d", position, steps());
        client.relativeMove(steps());
    }
}

void Nav::MenuEntry::Focuser::center(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: center, %d", mode);
    if(mode == Buttons::Single) {
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
        Display::Instance.draw().title(_name.c_str()).message({"Connecting..."});
        client.connect();
    }
}
