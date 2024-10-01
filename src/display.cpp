#include "display.h"
#include <Wire.h>
#include <SPI.h>
#include <ArduinoLog.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C


Display::Display() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
}

void Display::setup() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Log.errorln("SSD1306 allocation failed");
        return;
    }
    display.display();
}

void Display::sleep() {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}
