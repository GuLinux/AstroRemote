#ifndef FOCUSER_NAV_H
#define FOCUSER_NAV_H

#include <unistd.h>
#include <functional>
#include <vector>


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
    struct MenuEntry;
    using MenuEntries = std::vector<MenuEntry>;
    struct MenuEntry {
        Nav &nav;
        const char *name;
        MenuEntries subEntries;
        MenuEntry *parent = nullptr;
        uint8_t currentSubentryIndex = 0;
        MenuEntry &addEntry(const MenuEntry &entry);
        void draw();

        void left();
        void right();
        void up();
        void down();
        void enter();

        bool hasNext() const;
        bool hasPrev() const;

        std::function<void()> onEnter;
        std::function<void()> onLongPressEnter;
        std::function<void()> onLeft;
        std::function<void()> onRight;
        std::function<void()> onUp;
        std::function<void()> onDown;
    };
    MenuEntry rootMenu;
    MenuEntry &currentMenuEntry;
};

#endif

