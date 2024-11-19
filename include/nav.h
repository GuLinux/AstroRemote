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
    void onButton(Buttons::Button button, Buttons::Mode mode);
    void setup();
private:
    class MenuEntry {
    public:
        const String name() const { return _name; };
        MenuEntry *parent() const { return _parent; };
        void setParent(MenuEntry *parent) { this->_parent = parent; }
        virtual void draw() = 0;
        virtual void onButton(Buttons::Button button, Buttons::Mode mode) = 0;
        virtual void onEnter() {}
        virtual void onExit() {}
        class Parent;
        class Focuser;
        class Functional;
        class INDI;
    protected:
        MenuEntry(const String &name) : _name{name} {}
    private:
        const String _name;
        MenuEntry *_parent = nullptr;
    };
    MenuEntry *menuEntry = nullptr;
    MenuEntry::Parent * root;
    void navigate(MenuEntry *menuEntry);
    std::unique_ptr<Task> oneshotTask;
    std::unique_ptr<Task> sleepTask;
    void sleep();
    void autoSleep();
};

#endif

