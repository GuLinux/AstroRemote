#ifndef NAV_FUNCTIONAL_H
#define NAV_FUNCTIONAL_H
#include "nav.h"
#include <unordered_map>
#include <functional>
#include "utils.h"

class Nav::MenuEntry::Functional : public Nav::MenuEntry {
public:
    Functional(const String &name, MenuEntry *parent);
    using Callback = std::function<void()>;
    void draw() override;
    void onButton(Buttons::Button button, Buttons::Mode mode);
    void onEnter() override;
    void onExit() override;

    enum Action { Draw, OnEnter, OnExit, Left, Right, Up, Down, Center };
    Functional *on(const Callback &callback, Action action, Buttons::Mode mode = Buttons::Single);
private:
    const String _name;
    inline uint16_t getKey(Action action, Buttons::Mode mode = Buttons::Single) const;
    void invoke(Action action, Buttons::Mode mode = Buttons::Single);
    std::unordered_map<uint16_t, Callback> callbacks;
};


#endif