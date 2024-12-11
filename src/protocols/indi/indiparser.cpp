#include "indiparser.h"
#include <set>
#include <pugixml.hpp>

#define LOGSCOPE "[INDIParser] "
#ifdef ARDUINO
#include <ArduinoLog.h>
#include <WString.h>
#else
#include <cstdarg>

#define __LOGF(level, ...) template<typename... Args>  \
    void level ## ln(const char *fmt, Args ...args) { \
        printf("[%s] ", # level); \
        printf(fmt, args...); \
        printf("\n"); \
    }

struct _Log {
    __LOGF(info)
    __LOGF(warning)
    __LOGF(trace)
};
_Log Log;
#endif

long StringToNumber(const char *text) {
#ifdef ARDUINO
    return String(text).toInt(); // Use Arduino's String class
#else
    return std::stoi(std::string(text)); // Use std::string for native environments
#endif
}

bool INDIParser::parseDevices(const char *xml, std::size_t len, INDIDevice::ListInserter inserter)
{
    Log.infoln("Received XML: size=%d, buffer size=%d", len, buffer.size());


    std::vector<char> working_buffer(buffer.size());
    std::move(buffer.begin(), buffer.end(), working_buffer.begin());
    buffer.clear();
    std::move(xml, xml+len, std::back_inserter(working_buffer));

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(working_buffer.data(), working_buffer.size(), pugi::parse_default | pugi::parse_fragment);
    if(!result) {
        std::move(working_buffer.begin(), working_buffer.end(), std::back_inserter(buffer));
        Log.infoln("Remaining buffer: %d chars, `%s`", buffer.size(), buffer.data());
        return false;
    }
    Log.infoln("Buffer parse result: %d, desc=%s, offset=%d", static_cast<bool>(result), result.description(), result.offset);
    for(const pugi::xml_node &element: doc.children()) {
        // Log.infoln("Received element: %s, %s", element.name(), element.attribute("name"));
        if(strcmp("DRIVER_INFO", element.attribute("name").as_string()) == 0) {
            const char *deviceName = element.attribute("device").as_string();
            Log.traceln(LOGSCOPE "Got DRIVER_INFO element for device %s", deviceName);
            uint32_t interfaces = 0;
            pugi::xml_node child = element.find_child([](const pugi::xml_node &n){
                return strcmp("defText", n.name()) == 0 &&
                    strcmp("DRIVER_INTERFACE", n.attribute("name").as_string()) == 0;
            
            });
            if(child) {
                interfaces = child.text().as_int();
            }
            inserter = INDIDevice{deviceName, interfaces};
        }
    }
    
    return result;
}
