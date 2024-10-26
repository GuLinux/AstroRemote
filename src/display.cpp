#include "display.h"
#include <Wire.h>
#include <SPI.h>
#include <ArduinoLog.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define LOGPREFIX "[Display] "
#include "remote-control.xbm.h"
#define DISPLAY_INTERLINE 3
Display &Display::Instance = *new Display();

Display::Display() : display(U8G2_R0) {
}

void Display::setup() {
    if(!display.begin()) {
        delay(5000);
        Log.errorln(LOGPREFIX "SSD1306 allocation failed");
        return;
    }
    display.sleepOff();

    display.clearBuffer();
    display.setDrawColor(1);
    display.setFontMode(0);
    display.setFont(u8g2_font_6x12_t_symbols);
    display.setFontPosTop();
    display.setCursor(remote_control_width + 5, 5);
    display.println("Remote");
    display.setCursor(remote_control_width + 5, 25);
    display.println("Focuser");
    display.drawXBM(1, 1, remote_control_width, remote_control_height, remote_control_bits);
    display.sendBuffer();
}

void Display::sleep() {
    display.sleepOn();
}

void Display::clear() {
    display.clearBuffer();
}

Display::Draw Display::draw()
{
    return Draw(display);
}

Display::Draw::Draw(HardwareDisplay &display) : display{&display} {
    Log.traceln(LOGPREFIX "Default constructor with display");
    display.clearBuffer();
    
}

Display::Draw::Draw(Draw &other) {
    Log.traceln(LOGPREFIX "Copy constructor");
}

Display::Draw::~Draw() {
    Log.traceln(LOGPREFIX "sending buffer");
    display->sendBuffer();
}

Display::Draw &Display::Draw::title(const char *title) {
    Log.traceln(LOGPREFIX "drawing title: `%s`", title);
    drawCentered(title, display->getFontAscent());
    hasTitle = true;
    return *this;
}

void Display::Draw::drawCentered(const char *text, int y) {
    uint16_t destW = display->getUTF8Width(text);
    display->setCursor(SCREEN_WIDTH/2 - destW/2, y);
    display->print(text);
}

Display::Draw &Display::Draw::nav(const char *text, bool showLeftChevron, bool showRightChevron) {
    Log.traceln(LOGPREFIX "nav: %s, < %d, > %d", text, showLeftChevron, showRightChevron);
    display->setCursor(0, 25);
    display->print(showLeftChevron ? "<" : " ");

    display->setCursor(122, 25);
    display->print(showRightChevron ? ">" : " ");
    drawCentered(text, 25);
    return *this;
}

Display::Draw &Display::Draw::focuser(uint32_t position, uint16_t steps, bool moving) {
    Log.traceln(LOGPREFIX "focuser: positon=%d, steps=%d, moving=%d", position, steps, moving);
    uint16_t yPos = display->getAscent()*2 + DISPLAY_INTERLINE*2;
    display->setCursor(0, yPos);
    display->printf("Position: %d", position);
    yPos += display->getAscent() + DISPLAY_INTERLINE;
    display->setCursor(0, yPos);
    display->printf("Steps: %d\n", steps);
    yPos += display->getAscent() + DISPLAY_INTERLINE;
    display->setCursor(0, yPos);
    display->printf("Moving: %s\n", moving ? "true" : "false");
    return *this;
}

Display::Draw &Display::Draw::message(std::initializer_list<const char *> messages, uint16_t startingY) {
    for(const char *message: messages) {
        Log.traceln(LOGPREFIX "message: %s, y=%d", message, startingY);
        display->drawStr(0, startingY, message);
        startingY += display->getAscent() + DISPLAY_INTERLINE;
    }
    return *this;
}

Display::Draw &Display::Draw::message(std::initializer_list<const char *> messages) {
    return message(messages, hasTitle ? display->getAscent()*3 : 0);
}

Display::Draw &Display::Draw::footer(std::initializer_list<const char *> messages) {
    return message(messages, display->getHeight() - messages.size() * (display->getAscent() + DISPLAY_INTERLINE));
}
