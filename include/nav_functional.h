#ifndef NAV_FUNCTIONAL_H
#define NAV_FUNCTIONAL_H
#include "nav.h"
#include <unordered_map>
#include <functional>
#include "utils.h"

class Nav::MenuEntry::Functional : public Nav::MenuEntry {
public:
    Functional(const String &name) : _name{name} {}
    using Callback = std::function<void()>;
    const String name() const override { return _name; }
    void draw() override { invoke(Draw); }
    void left(Buttons::Mode mode) override { invoke(Left, mode); }
    void right(Buttons::Mode mode) override { invoke(Right, mode); }
    void up(Buttons::Mode mode) override { invoke(Up, mode); }
    void down(Buttons::Mode mode) override { invoke(Down, mode); }
    void center(Buttons::Mode mode) override { invoke(Center, mode); }
    void onEnter() override { invoke(OnEnter); }
    void onExit() override { invoke(OnExit); }

    enum Action { Draw, OnEnter, OnExit, Left, Right, Up, Down, Center };
    Functional *on(const Callback &callback, Action action, Buttons::Mode mode = Buttons::Single) {
        callbacks[getKey(action, mode)] = callback;
        return this;
    }
private:
    const String _name;
    inline uint16_t getKey(Action action, Buttons::Mode mode = Buttons::Single) const {
        return static_cast<uint16_t>(action) * 100 + static_cast<uint16_t>(mode);
    }
    void invoke(Action action, Buttons::Mode mode = Buttons::Single) {
        optionalCall(callbacks[getKey(action, mode)]);
    }
    std::unordered_map<uint16_t, Callback> callbacks;
};


#endif