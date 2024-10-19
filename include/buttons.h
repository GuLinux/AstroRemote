#ifndef FOCUSER_BUTTONS_H
#define FOCUSER_BUTTONS_H
#include <OneButton.h>

class Buttons {
public:
    static Buttons &Instance;
    void setup();
    void loop();
    enum Button { Up, Down, Left, Right, UpLeft, UpRight, DownLeft, DownRight, Center };
    enum Mode { Single, Double, Long };

    static void onClick(Button button, Mode mode);
private:
    OneButton upButton;
    OneButton downButton;
    OneButton leftButton;
    OneButton centralButton;
    OneButton rightButton;

};

#endif

