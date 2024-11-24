#ifndef FOCUSER_SETTINGS_H
#define FOCUSER_SETTINGS_H
#include <Preferences.h>
#include <wifisettings.h>
#include <ArduinoJson.h>
#include <vector>
#include <enum.h>

namespace {
    BETTER_ENUM(_Settings_Protocol, uint8_t, MyFP2, LX200, UnknownProtocol)
    BETTER_ENUM(_Settings_DeviceType, uint8_t, Focuser, Telescope, UnknownDevice)
}
class Settings {
public:
    using Protocol = _Settings_Protocol;
    using DeviceType = _Settings_DeviceType;
    static Settings &Instance;
    Settings();
    void setup();
    void load();
    
    struct Device {
        String name;
        DeviceType type;
        Protocol protocol;
        String address;
        uint16_t port;
        
    };
    struct INDIServer {
        String name;
        String address;
        uint16_t port = 7624;
    };
    using Devices = std::vector<Device>;
    using INDIServers = std::vector<INDIServer>;
    Devices focusers() const { return _focusers; }
    Devices telescopes() const { return _telescopes; }
    INDIServers indiServers() const { return _indiServers; }
    GuLinux::WiFiSettings &wifiSettings() { return _wifiSettings; }
    void asJson(JsonVariant &document);
private:
    Preferences prefs;
    GuLinux::WiFiSettings _wifiSettings;
    Devices _focusers;
    Devices _telescopes;
    INDIServers _indiServers;
    void loadDefaults();
    void loadConfigurationFile();
};
#endif
