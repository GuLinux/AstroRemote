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
        virtual void draw() = 0;
        virtual void onButton(Buttons::Button button, Buttons::Mode mode) = 0;
        virtual void onEnter() {}
        virtual void onExit() {}
        class Parent;
        class Focuser;
        class Functional;
    protected:
        MenuEntry(const String &name, MenuEntry *parent=nullptr) : _name{name}, _parent{parent} {}
    private:
        const String _name;
        MenuEntry *_parent;
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

