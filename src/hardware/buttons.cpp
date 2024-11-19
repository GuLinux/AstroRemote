#include <ArduinoLog.h>
#include <unordered_map>

#include "pinout.h"
#include "buttons.h"
#include "display.h"

#include "nav/nav.h"

using namespace std::placeholders;

Buttons &Buttons::Instance = *new Buttons();

Buttons::Buttons() : buttons{{
    { Up, UPBTN},
    { Left, LEFTBTN},
    { Right, RIGHTBTN},
    { Down, DOWNBTN},
    { Center, CENTRALBTN},
    { UpLeft, UPLEFT},
    { UpRight, UPRIGHT},
    { DownLeft, DOWNLEFT},
    { DownRight, DOWNRIGHT}
  }}
{
}

void Buttons::setup() {
  std::for_each(buttons.begin(), buttons.end(), std::bind(&ButtonInfo::setup, _1));
}

void Buttons::ButtonInfo::setup() {
    oneButton.setup(pin, INPUT_PULLDOWN, false);
    oneButton.attachClick([](void* p){ Buttons::onClick(((ButtonInfo*)p)->type, Single); }, this);
    oneButton.attachLongPressStop([](void* p){ Buttons::onClick(((ButtonInfo*)p)->type, Long); }, this);
}


void Buttons::loop() {
  std::for_each(buttons.begin(), buttons.end(), std::bind(&ButtonInfo::tick, _1));
}

void Buttons::onClick(Button button, Mode mode) {
  static const std::unordered_map<Mode, const char*> modes_map {
    { Single, "Single"},
    { Double, "Double" },
    { Long, "Long" },
  };
  static const std::unordered_map<Button, const char*> buttons_map {
    { Up, "Up" },
    { Down, "Down" },
    { Left, "Left " },
    { Right, "Right" },
    { Center, "Center" },
    { UpLeft, "UpLeft" },
    { UpRight, "UpRight" },
    { DownLeft, "DownLeft" },
    { DownRight, "DownRight" },
  };
  Log.traceln("Button clicked: button=%s, mode=%s", buttons_map.at(button), modes_map.at(mode));
  Nav::Instance.onButton(button, mode);
}

