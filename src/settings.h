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
    enum Protocol {
        MyFP2, LX200, UnknownProtocol
    };
    enum DeviceType {
        Focuser, Telescope, UnknownDevice
    };
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
        uint16_t port;
    };
    using Devices = std::vector<Device>;
    using INDIServers = std::vector<INDIServer>;
    Devices focusers() const { return _focusers; }
    Devices telescopes() const { return _telescopes; }
    INDIServers indiServers() const { return _indiServers; }
    GuLinux::WiFiSettings &wifiSettings() { return _wifiSettings; }
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
