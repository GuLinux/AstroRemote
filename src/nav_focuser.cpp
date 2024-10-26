#include "nav_focuser.h"
#include <TaskSchedulerDeclarations.h>
#include "globals.h"


#include <ArduinoLog.h>

#define LOGPREFIX "[Nav::MenuEntry] "
#define STEPS_MULTIPLIER 10

Nav::MenuEntry::Focuser::Focuser(const String &name, const String &address, uint16_t port, MenuEntry *parent)
    : MenuEntry{name, parent}, client{address, port} {
        Log.infoln(LOGPREFIX "Creating focuser: name=%s, address=%s:%d", name.c_str(), address, port);
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
        Display::Instance.draw().title(_name.c_str()).focuser(position, steps(), steps()*STEPS_MULTIPLIER, moving);
    }
}

void Nav::MenuEntry::Focuser::onButton(Buttons::Button button, Buttons::Mode mode) {
    switch (button) {
        case Buttons::DownLeft:
            if(mode == Buttons::Single) decreaseStepsSize();
            break;
        case Buttons::DownRight:
            if(mode == Buttons::Single) increaseStepsSize();
            break;
        case Buttons::Up:
            move(In, STEPS_MULTIPLIER);
            break;
        case Buttons::Down:
            move(Out, STEPS_MULTIPLIER);
            break;
        case Buttons::Left:
            move(In);
            break;
        case Buttons::Right:
            move(Out);
            break;
        case Buttons::Center:
            center(mode);
            break;

    }
}

void Nav::MenuEntry::Focuser::decreaseStepsSize() {
    if(client.status() == MyFP2Client::Connected && stepsIndex > 0) {
        stepsIndex--;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::increaseStepsSize() {
    if(client.status() == MyFP2Client::Connected && stepsIndex < allowedSteps.size()-1) {
        stepsIndex++;
        Log.infoln(LOGPREFIX "Setting steps size to %d [index=%d]", steps(), stepsIndex);
    }
    draw();
}

void Nav::MenuEntry::Focuser::move(Direction direction, uint8_t multiplier) {
    if(client.status() == MyFP2Client::Connected) {
        int16_t movingSteps = steps() * multiplier * direction;
        Log.infoln(LOGPREFIX "Moving focuser by %d steps - current position: %d", position, movingSteps);
        client.relativeMove(movingSteps);
    }
}

void Nav::MenuEntry::Focuser::abort() {
    Log.infoln(LOGPREFIX "Aborting focuser motion");
    client.abort();
}

void Nav::MenuEntry::Focuser::center(Buttons::Mode mode) {
    Log.infoln(LOGPREFIX "focuser: center, %d", mode);
    if(mode == Buttons::Single) {
        if(client.status() == MyFP2Client::Connected) {
            abort();
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
