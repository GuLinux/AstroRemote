#ifndef FOCUSER_NAV_H
#define FOCUSER_NAV_H

#include <unistd.h>
#include <functional>
#include <vector>
#include <WString.h>


class Nav {
public:
    static Nav &Instance;
    enum PressMode { Single, Double, Long };

    Nav();

    void left(PressMode mode);
    void right(PressMode mode);
    void up(PressMode mode);
    void down(PressMode mode);
    void center(PressMode mode);
    void setup();
private:
    class MenuEntry {
    public:
        virtual const String name() const = 0;
        virtual void draw() = 0;
        virtual void left(Nav::PressMode mode) = 0;
        virtual void right(Nav::PressMode mode) = 0;
        virtual void up(Nav::PressMode mode) = 0;
        virtual void down(Nav::PressMode mode) = 0;
        virtual void center(Nav::PressMode mode) = 0;
        virtual void onEnter() {}
        virtual void onExit() {}
        class Parent;
        // class Back;
        class Focuser;
    };
    MenuEntry *menuEntry = nullptr;
    void navigate(MenuEntry *menuEntry);
};

#endif

