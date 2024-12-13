#include "indiparser.h"
#include <set>
#include <pugixml.hpp>
#include <regex>

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

size_t INDIParser::parseDevices(const char *xml, std::size_t len, INDIDevice::ListInserter inserter) {
    size_t devices = 0;
    parse(xml, len, [&devices, &inserter](const pugi::xml_document &doc){
        for(const auto &element: doc.children()) {
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
                devices++;
            }
        }
    });
    return devices;
}

void INDIParser::parse(const char *xml, std::size_t len, const OnXmlParsed onXmlParsed) {
    Log.infoln("Received XML: size=%d, buffer size=%d", len, buffer.size());

    Buffer working_buffer(buffer.size());
    std::move(buffer.begin(), buffer.end(), working_buffer.begin());
    buffer.clear();
    std::move(xml, xml+len, std::back_inserter(working_buffer));

    size_t chunk_start = 0;
    Buffer::iterator search_start = working_buffer.begin();
    for(auto it = find_end_tags(search_start, working_buffer.end()); it != working_buffer.end(); it = find_end_tags(search_start, working_buffer.end())) {
        size_t chunk_length = std::distance(working_buffer.begin() + chunk_start, it);
        std::string_view chunk(&working_buffer[chunk_start], chunk_length);
        // Log.infoln("**** Chunk found: start=%d, length=%d, chunk=`%s`", chunk_start, chunk_length, std::string(chunk).c_str());
        search_start = it;
        
        
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(chunk.data(), chunk_length);
        Log.infoln("Result: %d, offset=%d, %s", static_cast<bool>(result), result.offset, result.description());
        if(!result) continue;

        chunk_start = std::distance(working_buffer.begin(), it);
        onXmlParsed(doc);
    }
    Log.infoln("Filling back buffer: buffer_start=%d, working_buffer size: %d, existing buffer size: %d", chunk_start, working_buffer.size(), buffer.size());
    if(chunk_start< working_buffer.size()) {
        std::move(working_buffer.begin() + chunk_start, working_buffer.end(), std::back_inserter(buffer));
    }

}


INDIParser::Buffer::iterator INDIParser::find_end_tags(Buffer::iterator begin, Buffer::iterator end) {
    auto found = std::find(begin, end, '>');
    if(found == end) {
        return end;
    }
    return found + 1;
}

