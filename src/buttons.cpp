#include "buttons.h"
#include <ArduinoLog.h>
#include "myfp2client.h"
#include "pinout.h"
#include "display.h"
#include "nav.h"

Buttons &Buttons::Instance = *new Buttons();

void Buttons::setup() {
  upButton.setup(UPBTN, INPUT_PULLDOWN, false);
  downButton.setup(DOWNBTN, INPUT_PULLDOWN, false);
  leftButton.setup(LEFTBTN, INPUT_PULLDOWN, false);
  rightButton.setup(RIGHTBTN, INPUT_PULLDOWN, false);
  centralButton.setup(CENTRALBTN, INPUT_PULLDOWN, false);

  upButton.attachClick([](){Nav::Instance.up(Nav::Single); });
  downButton.attachClick([](){Nav::Instance.down(Nav::Single); });
  leftButton.attachClick([](){Nav::Instance.left(Nav::Single); });
  rightButton.attachClick([](){Nav::Instance.right(Nav::Single); });
  centralButton.attachClick([](){Nav::Instance.center(Nav::Single); });
  centralButton.attachClick([](){Nav::Instance.center(Nav::Long); });
}

void Buttons::loop() {
  upButton.tick();
  downButton.tick();
  leftButton.tick();
  centralButton.tick();
  rightButton.tick();

}
