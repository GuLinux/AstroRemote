#include "indiparse.h"
#include <tinyxml2.h>
#include <iostream>
#include <set>

std::list<std::string> INDIParser::parseDevices(const char *xml, std::size_t len)
{
    std::set<std::string> keys;
    std::list<std::string> out;

    tinyxml2::XMLDocument xmlDoc;
    xmlDoc.Parse(xml, len);
    tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
    while(element) {
        const char *deviceName = element->Attribute("device");
        if(deviceName && !keys.contains(deviceName)) {
            keys.insert(deviceName);
            out.push_back(deviceName);
        }
        element = element->NextSiblingElement();
    }
    return out;
}
