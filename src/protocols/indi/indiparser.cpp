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

void INDIParser::parseDevices(const char *xml, std::size_t len, INDIDevice::ListInserter inserter)
{
    Log.infoln("Received XML: size=%d, buffer size=%d", len, buffer.size());

    Buffer working_buffer(buffer.size());
    std::move(buffer.begin(), buffer.end(), working_buffer.begin());
    buffer.clear();
    std::move(xml, xml+len, std::back_inserter(working_buffer));
    size_t search_start = 0;
    size_t chunk_start = 0;
    while(working_buffer[search_start] == '\n' || working_buffer[search_start] == '\r') search_start++;
    for(
        auto newline_it=std::find_if(working_buffer.begin() + search_start, working_buffer.end(), [](const char c) { return c == '\n' || c == '\r'; });
        newline_it != working_buffer.end();
        newline_it = std::find_if(working_buffer.begin() + search_start, working_buffer.end(), [](const char c) { return c == '\n' || c == '\r'; })
        ) {

        size_t chunk_length = std::distance(working_buffer.begin() + chunk_start, newline_it);
        std::string_view chunk(&working_buffer[chunk_start], chunk_length);

        Log.infoln("Current buffer: size=%d", working_buffer.size());
        Log.infoln("********************* search_start=%d, chunk_start=%d, chunk_length=%d, chunk=%s", search_start, chunk_start, chunk_length, std::string(chunk).data());

        search_start += std::distance(working_buffer.begin() + search_start, newline_it)+1;
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(chunk.data(), chunk_length);
        Log.infoln("Result: %d, offset=%d, %s", static_cast<bool>(result), result.offset, result.description());
        if(!result) continue;
        chunk_start = search_start;
        // if(!result) {
        //     std::move(working_buffer.begin(), working_buffer.end(), std::back_inserter(buffer));
        //     Log.infoln("Remaining buffer: %d chars, `%s`", buffer.size(), buffer.data());
        //     continue;
        // }
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
            }
        }
    }
    Log.infoln("Filling back buffer: buffer_start=%d, working_buffer size: %d, existing buffer size: %d", chunk_start, working_buffer.size(), buffer.size());
    if(chunk_start< working_buffer.size()) {
        std::move(working_buffer.begin() + chunk_start, working_buffer.end(), std::back_inserter(buffer));
    }

}
