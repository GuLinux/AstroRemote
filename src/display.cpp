#include "display.h"
#include <Wire.h>
#include <SPI.h>
#include <ArduinoLog.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define LOGPREFIX "[Display] "

Display &Display::Instance = *new Display();

Display::Display() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
}

void Display::setup() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        delay(5000);
        Log.errorln(LOGPREFIX "SSD1306 allocation failed");
        return;
    }
    static const uint8_t logo[] = {0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x03, 0xff, 0xc0, 0x07, 0xe7, 0xe0, 0x0f, 0x00, 0xf0, 0x1c, 0x7e, 0x38, 0x39, 0xff, 0x9c, 0x3b, 0xc3, 0xdc, 0x73, 0x81, 0xce, 0x77, 0x3c, 0xee, 0x67, 0x7e, 0xe6, 0x66, 0x66, 0x66, 0x66, 0x6e, 0x66, 0x77, 0x7e, 0xee, 0x77, 0x3c, 0xee, 0x73, 0x81, 0xce, 0x3b, 0xe7, 0xdc, 0x39, 0xff, 0x9c, 0x1c, 0x7e, 0x38, 0x0f, 0x00, 0xf0, 0x07, 0xe7, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00};

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setFont(NULL);
    display.setCursor(30, 5);
    display.println("Remote");
    display.setCursor(30, 25);
    display.println("Focuser");
    display.drawBitmap(1, 1, logo, 24, 24, 1);
    display.display();
}

void Display::sleep() {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}


void Display::drawOption(const char *title, const char *text, bool showLeftChevron, bool showRightChevron) {
    Log.infoln(LOGPREFIX "drawing option: title=`%s`, text=`%s`, leftChevron=%T, rightChevron=%T", title, text, showLeftChevron, showRightChevron);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont(NULL);
    display.setCursor(0, 2);
    display.setTextWrap(0);
    display.setCursor(40, 2);
    display.println(title);
    
    display.setCursor(0, 25);
    display.print(showLeftChevron ? "<" : " ");

    display.setCursor(122, 25);
    display.print(showRightChevron ? ">" : " ");

    // display.setCursor(13, 25);
    // display.println(text);
    drawCentered(text, 13);
    display.display();
    Log.infoln(LOGPREFIX "Option drawn");
}

void Display::drawFooter(const char *text) {
    display.setTextWrap(true);
    display.setCursor(0, 48);
    display.println(text);
    display.display();
}

void Display::drawCentered(const char *text, int y) {
    int16_t destX, destY;
    uint16_t destW, destH;
    display.getTextBounds(text, 0, 0, &destX, &destY, &destW, &destH);
    display.setCursor(SCREEN_WIDTH/2 - destW/2, y);

    display.setTextWrap(0);
    display.print(text);
}

