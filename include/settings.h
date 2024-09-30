#ifndef FOCUSER_SETTINGS_H
#define FOCUSER_SETTINGS_H
#include <Preferences.h>

class Settings {
public:
    static Settings &Instance;
    Settings();
    void setup();
    void load();
private:
    Preferences prefs;
    void loadDefaults();
};
#endif