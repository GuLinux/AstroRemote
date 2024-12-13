#ifndef INDI_PARSER
#define INDI_PARSER

#include <list>
#include "indidevice.h"
#include <vector>
#include <pugixml.hpp>
#include <functional>

class INDIParser {
public:
    size_t parseDevices(const char *xml, std::size_t len, INDIDevice::ListInserter inserter);
private:
    using OnXmlParsed = std::function<void(const pugi::xml_document&)>;
    using Buffer=std::vector<char>;
    Buffer buffer;
    Buffer::iterator find_end_tags(Buffer::iterator begin, Buffer::iterator end);
    void parse(const char *xml, std::size_t len, const OnXmlParsed onXmlParsed);
};

#endif
