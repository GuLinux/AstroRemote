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
    void drawFooter(const char *text);
    void drawMessage(const char *title, const char *message, const char *footer=nullptr);
    void drawFocuser(const char *name, uint32_t position, uint16_t steps, bool moving);
private:
    void drawCentered(const char *text, int y);
    Adafruit_SSD1306 display;

    void drawMenu();
};
#endif
