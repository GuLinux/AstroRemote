#include "settings.h"

#define SETTINGS_KEY_VERSION "version"
#define SETTINGS_VALUE_VERSION 1

Settings &Settings::Instance = *new Settings();

Settings::Settings() {
}

void Settings::setup() {   
    prefs.begin("RemoteFocuser");
    load();
}

void Settings::load() {
    uint16_t version = prefs.getUInt(SETTINGS_KEY_VERSION);
    if(version != SETTINGS_VALUE_VERSION) {
        loadDefaults();
        return;
    }
}

void Settings::loadDefaults() {
}
