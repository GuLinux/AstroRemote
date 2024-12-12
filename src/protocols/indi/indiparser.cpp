#include "indiparser.h"
#include <set>
#include <tinyxml2.h>

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

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result = doc.Parse(working_buffer.data(), working_buffer.size());

    if(result != tinyxml2::XML_SUCCESS) {
        std::move(working_buffer.begin(), working_buffer.end(), std::back_inserter(buffer));
        Log.infoln("Remaining buffer: %d chars, `%s`", buffer.size(), buffer.data());
        return false;
    }
    tinyxml2::XMLElement* element = doc.FirstChildElement();
    while(element) {
        // Log.infoln("Received element: %s, %s", element.name(), element.attribute("name"));
        if(strcmp("DRIVER_INFO", element->Attribute("name")) == 0) {
            const char *deviceName = element->Attribute("device");
            Log.traceln(LOGSCOPE "Got DRIVER_INFO element for device %s", deviceName);
            uint32_t interfaces = 0;
            tinyxml2::XMLElement* child = element->FirstChildElement("defText");
            while(child) {
                if(strcmp(child->Attribute("name"), "DRIVER_INTERFACE") == 0) {
                    interfaces = StringToNumber(child->GetText());
                    break;
                }
                child = child->NextSiblingElement("defText");
            }
            inserter = INDIDevice{deviceName, interfaces};
        }
        element = element->NextSiblingElement();
    }
    
    return true;
}
