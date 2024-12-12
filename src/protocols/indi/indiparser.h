#ifndef INDI_PARSER
#define INDI_PARSER

#include <list>
#include "indidevice.h"
#include <vector>

class INDIParser {
public:
    void parseDevices(const char *xml, std::size_t len, INDIDevice::ListInserter inserter);
private:
    using Buffer=std::vector<char>;
    Buffer buffer;
};

#endif
