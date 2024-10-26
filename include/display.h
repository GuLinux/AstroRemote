#ifndef FOCUSER_DISPLAY_H
#define FOCUSER_DISPLAY_H

#include <U8g2lib.h>
#include <memory>

class Display {
public:
    class Draw;
    using HardwareDisplay = U8G2_SH1106_128X64_NONAME_F_HW_I2C;
    static Display &Instance;
    Display();
    void setup();
    void sleep();
    Draw draw();
    class Draw {
    public:
        Draw(HardwareDisplay &display);
        Draw(Draw &other);
        ~Draw();
        Draw &title(const char *title);
        Draw &nav(const char *text, bool showLeftChevron=true, bool showRightChevron=true);
        
        Draw &message(std::initializer_list<const char *>messages);
        Draw &message(std::initializer_list<const char *>messages, uint16_t startingY);
        Draw &footer(std::initializer_list<const char *>messages);
        Draw &focuser(uint32_t position, uint16_t steps, uint16_t multipliedSteps, bool moving);
    private:
        void drawCentered(const char *text, int y);
        HardwareDisplay *display;
        bool hasTitle = false;
    };

private:
    
    void clear();
    HardwareDisplay display;
};
#endif
