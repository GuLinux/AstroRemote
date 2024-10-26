# AstroRemote

A DIY remote control for your astronomy devices


## Supported devices

Initially, this was meant to be a wifi enabled remote control for myFocuserPro2 devices.

But I realised that the same principle can be applied to support many more devices, including remote controlling INDI servers, so the following list will grow with time.

### Direct protocol connection

The following devices are supported without a centralised server (such as INDI)

#### Currently supported

 - myFocuserPro2
    - Focus in/out, change steps


#### Near future

 - OnStep/LX200 mount

### INDI Server

Using an INDI server allows for more devices to be supported, since the protocol heavy lifting will be done by INDI itself, with the drawback of having to rely on a PC running the INDI server.

#### Near future

 - Telescopes
 - Focusers
 - Filter wheels


## Hardware

At the moment the project is based on ESP32 C3 Supermini devices, as they're pretty cheap and easy to come around.

In general, the vast majority of ESP32 microcontroller should be supported, provided you change the pin mapping.

In addition to the microcontroller, you'll also need:

 - An I2C OLED module (SSD1306/SH1106), ideally at least 1.3".
 - 9 momentary push buttons
 - Pull down resistors where applicable (some ESP32 pins don't have internal pull down)
 - A power source (ideally batteries + step up or down modules, depending on your choice)

## Software requirement

This project is built and configured with Platformio, so that all libraries are managed directly by the platform.

You _could_ use the Arduino IDE, by renaming the `main.cpp` file to `AstroFocuser.ino`, and then managing the dependencies manually, this is not officially supported though.

## Notes

 - For telescopes, GOTO support and other advances features are unlikely to be implemented. This project is not meant to fully replace your handset, but rather to be a lightweight handy remote for slewing and small field corrections.