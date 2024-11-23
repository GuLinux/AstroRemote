#ifndef INDI_PARSER
#define INDI_PARSER

#include <tinyxml2.h>
#include <list>

class INDIParser {
public:
    std::list<const char*> parseDevices(const char *xml, std::size_t len);
};

#endif