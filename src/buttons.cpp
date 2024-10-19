#include "buttons.h"
#include <ArduinoLog.h>
#include "myfp2client.h"
#include "pinout.h"
#include "display.h"
#include "nav.h"
#include <unordered_map>

Buttons &Buttons::Instance = *new Buttons();

void Buttons::setup() {
  upButton.setup(UPBTN, INPUT_PULLDOWN, false);
  downButton.setup(DOWNBTN, INPUT_PULLDOWN, false);
  leftButton.setup(LEFTBTN, INPUT_PULLDOWN, false);
  rightButton.setup(RIGHTBTN, INPUT_PULLDOWN, false);
  centralButton.setup(CENTRALBTN, INPUT_PULLDOWN, false);

  upButton.attachClick([](){ Buttons::onClick(Up, Single); });
  downButton.attachClick([](){ Buttons::onClick(Down, Single); });
  leftButton.attachClick([](){ Buttons::onClick(Left, Single); });
  rightButton.attachClick([](){ Buttons::onClick(Right, Single); });
  centralButton.attachClick([](){ Buttons::onClick(Center, Single); });
  centralButton.attachLongPressStop([](){ Buttons::onClick(Center, Long); });
}

void Buttons::loop() {
  upButton.tick();
  downButton.tick();
  leftButton.tick();
  centralButton.tick();
  rightButton.tick();

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
  switch(button) {
    case Up:
      Nav::Instance.up(mode);
      break;
    case Down:
      Nav::Instance.down(mode);
      break;
    case Left:
      Nav::Instance.left(mode);
      break;
    case Right:
      Nav::Instance.right(mode);
      break;
    case Center:
      Nav::Instance.center(mode);
      break;
  }
}
