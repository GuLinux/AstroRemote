#ifndef INDI_PARSER
#define INDI_PARSER

#include <tinyxml2.h>
#include <list>
#include <string>

class INDIParser {
public:
    std::list<std::string> parseDevices(const char *xml, std::size_t len);
};

#endif