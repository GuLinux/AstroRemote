#include "indiparser.h"
#include <tinyxml2.h>
#include <set>

std::list<const char*> INDIParser::parseDevices(const char *xml, std::size_t len)
{
    std::set<const char*> keys;
    std::list<const char*> out;

    tinyxml2::XMLDocument xmlDoc;
    xmlDoc.Parse(xml, len);
    tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
    while(element) {
        const char *deviceName = element->Attribute("device");
        if(deviceName && !keys.count(deviceName)) {
            keys.insert(deviceName);
            out.push_back(deviceName);
        }
        element = element->NextSiblingElement();
    }
    return out;
}
