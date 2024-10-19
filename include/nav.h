#ifndef FOCUSER_NAV_H
#define FOCUSER_NAV_H

#include <unistd.h>
#include <functional>
#include <vector>
#include <WString.h>
#include <TaskSchedulerDeclarations.h>
#include <memory>
#include "buttons.h"


class Nav {
public:
    static Nav &Instance;

    Nav();

    void left(Buttons::Mode mode);
    void right(Buttons::Mode mode);
    void up(Buttons::Mode mode);
    void down(Buttons::Mode mode);
    void center(Buttons::Mode mode);
    void setup();
private:
    class MenuEntry {
    public:
        virtual const String name() const = 0;
        virtual void draw() = 0;
        virtual void left(Buttons::Mode mode) = 0;
        virtual void right(Buttons::Mode mode) = 0;
        virtual void up(Buttons::Mode mode) = 0;
        virtual void down(Buttons::Mode mode) = 0;
        virtual void center(Buttons::Mode mode) = 0;
        virtual void onEnter() {}
        virtual void onExit() {}
        class Parent;
        class Focuser;
        class Functional;
    };
    MenuEntry *menuEntry = nullptr;
    void navigate(MenuEntry *menuEntry);
    std::unique_ptr<Task> oneshotTask;
    std::unique_ptr<Task> sleepTask;
    void sleep();
    void autoSleep();
};

#endif

