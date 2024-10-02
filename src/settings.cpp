#include "settings.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#define SETTINGS_KEY_VERSION "version"
#define SETTINGS_VALUE_VERSION 1
#define FOCUSERS_FILE_PATH "/focusers.json"
#define LOGPREFIX "[Settings] "

Settings &Settings::Instance = *new Settings();

Settings::Settings() : wifiSettings(prefs, LittleFS, "FocuserRemote-") {
}

Settings::Focuser::Focuser(const String &name, const String &address, uint16_t port) : port{port} {
    strcpy(this->name, name.c_str());
    strcpy(this->address, address.c_str());
}

void Settings::setup() {
    prefs.begin("RemoteFocuser");
    wifiSettings.setup();
    load();
}

void Settings::load() {
    uint16_t version = prefs.getUInt(SETTINGS_KEY_VERSION);
    if(version != SETTINGS_VALUE_VERSION) {
        Log.warningln(LOGPREFIX "No settings version found, loading defaults");
        loadDefaults();
        return;
    }
    Log.infoln(LOGPREFIX "Loading settings");
    wifiSettings.load();
    loadFocusers();
}

void Settings::loadDefaults() {
    wifiSettings.loadDefaults();
    loadFocusers();
}

void Settings::loadFocusers() {
    _focusers.clear();
    if(!LittleFS.exists(FOCUSERS_FILE_PATH)) {
        Log.infoln(LOGPREFIX FOCUSERS_FILE_PATH " not found, skipping loading focusers list");
        return;
    }
    fs::File focusersFile = LittleFS.open(FOCUSERS_FILE_PATH);
    JsonDocument focusersDocument;
    DeserializationError error = deserializeJson(focusersDocument, focusersFile);
    if(error != DeserializationError::Ok) {
        Log.errorln(LOGPREFIX "Error during deserialisation of " FOCUSERS_FILE_PATH " : %s (%d)", error.c_str(), error.code());
        return;
    }
    _focusers.reserve(focusersDocument.as<JsonArray>().size());
    for(JsonObject focuser: focusersDocument.as<JsonArray>()) {
        _focusers.push_back({focuser["name"], focuser["address"], focuser["port"]});
        Log.infoln(LOGPREFIX R"(Added focuser: {name="%s", address="%s", port=%d})", _focusers.back().name, _focusers.back().address, _focusers.back().port);

    }
}
