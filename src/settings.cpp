#include "settings.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <map>

#define SETTINGS_KEY_VERSION "version"
#define SETTINGS_VALUE_VERSION 1
#define CONFIGURATION_FILE_PATH "/config.json"
#define LOGPREFIX "[Settings] "

Settings &Settings::Instance = *new Settings();

Settings::Settings() : _wifiSettings(prefs, LittleFS, "AstroRemote") {
}


void Settings::setup() {
    prefs.begin("AstroRemote");
    _wifiSettings.setup();
    load();
    Log.infoln("Finished loading settings: ");
    Log.infoln("WiFi: %d stations, hostname=%s", _wifiSettings.stations().size(), _wifiSettings.apConfiguration().essid);
    for(auto station: _wifiSettings.stations()) {
        Log.infoln(" - station: %s", station.essid);
    }
    Log.infoln("Focusers: %d", _focusers.size());
    for(auto focuser: _focusers) {
        Log.infoln(" - %s: %s:%d", focuser.name, focuser.address, focuser.port);
    }
}

void Settings::load() {
    uint16_t version = prefs.getUInt(SETTINGS_KEY_VERSION);
    if(version != SETTINGS_VALUE_VERSION) {
        Log.warningln(LOGPREFIX "No settings version found, loading defaults");
        loadDefaults();
        return;
    }
    Log.infoln(LOGPREFIX "Loading settings");
    _wifiSettings.load();
    loadConfigurationFile();
}

void Settings::loadDefaults() {
    _wifiSettings.loadDefaults();
    loadConfigurationFile();
}

void Settings::loadConfigurationFile() {
    _focusers.clear();
    _telescopes.clear();
    static const std::map<String, Protocol> protocols { {"lx200", LX200}, {"myFP2", MyFP2}};
    static const std::map<String, DeviceType> deviceTypes{ {"focuser", Focuser}, {"telescope", Telescope}};
    if(!LittleFS.exists(CONFIGURATION_FILE_PATH)) {
        Log.infoln(LOGPREFIX CONFIGURATION_FILE_PATH " not found, skipping loading focusers list");
        return;
    }
    fs::File configurationFile = LittleFS.open(CONFIGURATION_FILE_PATH);
    Log.infoln(LOGPREFIX "Loading configuration file: %s, size=%dB", configurationFile.path(), configurationFile.size());
    JsonDocument configDocument;
    DeserializationError error = deserializeJson(configDocument, configurationFile);
    if(error != DeserializationError::Ok) {
        Log.errorln(LOGPREFIX "Error during deserialisation of " CONFIGURATION_FILE_PATH " : %s (%d)", error.c_str(), error.code());
        return;
    }
    for(JsonObject jsonDevice: configDocument["devices"].as<JsonArray>()) {
        Device device{
            jsonDevice["name"],
            (deviceTypes.count(jsonDevice["type"]) ? deviceTypes.at(jsonDevice["type"]) : UnknownDevice),
            (protocols.count(jsonDevice["protocol"]) ? protocols.at(jsonDevice["protocol"]) : UnknownProtocol),
            jsonDevice["address"],
            jsonDevice["port"],
        };
        switch(device.type) {
            case Focuser:
                _focusers.push_back(device);
                break;
            case Telescope:
                _telescopes.push_back(device);
                break;
            default:
                Log.errorln(LOGPREFIX "Unknown device found: name=%s, type=%s", jsonDevice["name"], jsonDevice["type"]);
                break;
        }
        if(device.type != UnknownDevice) Log.infoln("Added device: type=%s, protocol=%s, name=%s, address=%s, port=%d",
            jsonDevice["type"].as<String>().c_str(),
            (device.protocol != UnknownProtocol ? jsonDevice["protocol"].as<String>().c_str() : "unknown"),
            device.name.c_str(),
            device.address.c_str(),
            device.port
        );
    }
}
