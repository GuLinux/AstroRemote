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

namespace {
std::map<String, Settings::Protocol> STRINGS_TO_PROTOCOLS;
std::map<Settings::Protocol, String> PROTOCOLS_TO_STRINGS;
std::map<String, Settings::DeviceType> STRINGS_TO_DEVICE_TYPES;
std::map<Settings::DeviceType, String> DEVICE_TYPES_TO_STRINGS;
void _map(String protocolName, Settings::Protocol protocol) {
    STRINGS_TO_PROTOCOLS[protocolName] = protocol;
    PROTOCOLS_TO_STRINGS[protocol] = protocolName;
}
void _map(String devicetypeName, Settings::DeviceType deviceType) {
    STRINGS_TO_DEVICE_TYPES[devicetypeName] = deviceType;
    DEVICE_TYPES_TO_STRINGS[deviceType] = devicetypeName;
}

}
Settings::Settings() : _wifiSettings(prefs, LittleFS, "AstroRemote") {
}


void Settings::setup() {
    _map("lx200", LX200);
    _map("myFP2", MyFP2);
    _map("focuser", Focuser);
    _map("telescope", Telescope);

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

void Settings::loadDefaults()
{
#ifdef CONFIG_SIMULATOR
    _wifiSettings.setStationConfiguration(0, "Wokwi-GUEST", "");
    _focusers.push_back({"MyFP2ESP32", DeviceType::Focuser, Protocol::MyFP2, "myfp2esp32.lan", 2020});
    _indiServers.push_back({"BlueBox", "bluebox.lan"});
#else
    _wifiSettings.loadDefaults();
    loadConfigurationFile();
    #endif
}

void Settings::asJson(JsonVariant &document) {
    Log.traceln(LOGPREFIX "Converting settings to json");
    JsonObject root = document.to<JsonObject>();
    JsonArray devices = root["devices"].to<JsonArray>();
    Log.traceln(LOGPREFIX "Adding devices");
    const auto mapDevice = [&devices] (const Device &device) {
        auto deviceObject = devices.add<JsonObject>();
        Log.traceln(LOGPREFIX " * adding device %s", device.name);
        deviceObject["name"] = device.name;
        deviceObject["type"] = DEVICE_TYPES_TO_STRINGS.at(device.type);
        deviceObject["protocol"] = PROTOCOLS_TO_STRINGS.at(device.protocol);
        deviceObject["address"] = device.address;
        deviceObject["port"] = device.port;
    };
    std::for_each(_focusers.begin(), _focusers.end(), mapDevice); 
    std::for_each(_telescopes.begin(), _telescopes.end(), mapDevice); 
    JsonArray indiServers = root["INDI Servers"].to<JsonArray>();
    Log.traceln(LOGPREFIX "Adding INDI servers");
    std::for_each(_indiServers.begin(), _indiServers.end(), [&indiServers](const INDIServer &indiServer) {
        Log.traceln(LOGPREFIX " * adding INDI server %s", indiServer.name);
        auto indiServerObject = indiServers.add<JsonObject>();
        indiServerObject["name"] = indiServer.name;
        indiServerObject["address"] = indiServer.address;
        indiServerObject["port"] = indiServer.port;
    });
}


void Settings::loadConfigurationFile() {
    _focusers.clear();
    _telescopes.clear();
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
            (STRINGS_TO_DEVICE_TYPES.count(jsonDevice["type"]) ? STRINGS_TO_DEVICE_TYPES.at(jsonDevice["type"]) : UnknownDevice),
            (STRINGS_TO_PROTOCOLS.count(jsonDevice["protocol"]) ? STRINGS_TO_PROTOCOLS.at(jsonDevice["protocol"]) : UnknownProtocol),
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
