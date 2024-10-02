#ifndef FOCUSER_BUTTONS_H
#define FOCUSER_BUTTONS_H
#include <OneButton.h>

class Buttons {
public:
    static Buttons &Instance;
    void setup();
    void loop();
private:
    OneButton upButton;
    OneButton downButton;
    OneButton leftButton;
    OneButton centralButton;
    OneButton rightButton;

};

#endif

