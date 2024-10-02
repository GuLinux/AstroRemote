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
    void drawOption(const char *title, const char *text, bool showLeftChevron=true, bool showRightChevron=true);
private:
    Adafruit_SSD1306 display;

    void drawMenu();
};
#endif
