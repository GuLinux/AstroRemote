#include "indiparser.h"
#include <tinyxml2.h>
#include <set>

#ifdef ARDUINO
#include <WString.h>
#endif

long StringToNumber(const char *text) {
#ifdef ARDUINO
    return String(text).toInt(); // Use Arduino's String class
#else
    return std::stoi(std::string(text)); // Use std::string for native environments
#endif
}

// #include <iostream>
std::list<INDIDevice> INDIParser::parseDevices(const char *xml, std::size_t len)
{
    std::list<INDIDevice> out;
    tinyxml2::XMLDocument xmlDoc;
    xmlDoc.Parse(xml, len);
    tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
    while(element) {
        // std::cerr << "element name: `" << element->Name() << "`\n";
        if(strcmp(element->Attribute("name"), "DRIVER_INFO") == 0) {
            const char *deviceName = element->Attribute("device");
            uint32_t interfaces = 0;
            tinyxml2::XMLElement* child = element->FirstChildElement("defText");
            while (child) {
                const char* name = child->Attribute("name");
                if (strcmp(child->Attribute("name"), "DRIVER_INTERFACE") == 0) {
                    const char* value = child->GetText();
                    interfaces = StringToNumber(value);
                }
                child = child->NextSiblingElement("defText");
            }
            out.push_back({deviceName, interfaces});
        }
        element = element->NextSiblingElement();
    }
    return out;
}
