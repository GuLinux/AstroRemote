#ifndef FOCUSER_DISPLAY_H
#define FOCUSER_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display {
public:
    static Display &Instance;
    Display();
    void setup();
    void sleep();
private:
    Adafruit_SSD1306 display;
};
#endif
