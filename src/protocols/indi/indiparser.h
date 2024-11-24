#ifndef INDI_PARSER
#define INDI_PARSER

#include <tinyxml2.h>
#include <list>
#include "indidevice.h"

class INDIParser {
public:
    std::list<INDIDevice> parseDevices(const char *xml, std::size_t len);
};

#endif