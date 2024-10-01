#ifndef FOCUSER_SETTINGS_H
#define FOCUSER_SETTINGS_H
#include <Preferences.h>
#include <wifisettings.h>
#include <vector>

class Settings {
public:
    static Settings &Instance;
    Settings();
    void setup();
    void load();
    struct Focuser {
        String name;
        String address;
        uint16_t port;
    };
    using Focusers = std::vector<Focuser>;
    Focusers focusers;
private:
    Preferences prefs;
    GuLinux::WiFiSettings wifiSettings;
    void loadDefaults();
    void loadFocusers();
};
#endif