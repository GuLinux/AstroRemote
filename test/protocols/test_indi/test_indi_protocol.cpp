#include <gtest/gtest.h>
#include "protocols/indi/indiparser.h"
#include "protocols/indi/indidevice.h"
// uncomment line below if you plan to use GMock
// #include <gmock/gmock.h>

// TEST(...)
// TEST_F(...)

void PrintTo(const INDIDevice& device, std::ostream* os) {
    *os << "INDIDevice(" << device.name << ", interfaces: [";
    bool first = true;
    for(const auto &interface: device.interfaces()) {
        if(!first) {
            *os << ", ";
        } else {
            first = false;
        }
        *os << interface;
    }
    *os << "])";
}

TEST(TestIndiDevice, itShouldStoreNameAndInterfaceAndCompareByStrEquality) {
    INDIDevice device{"foo", INDIDevice::Interface::Telescope};
    char differentPointer[4];
    strcpy(differentPointer, device.name.c_str());
    INDIDevice anotherDevice{differentPointer, INDIDevice::Interface::Telescope};
    ASSERT_NE(device.name.c_str(), differentPointer);
    ASSERT_NE(device.name.c_str(), anotherDevice.name.c_str());
    ASSERT_EQ(device, anotherDevice);
}

TEST(TestIndiDevice, itShouldExtractTheRightInterfacesWithSingleInterface) {
    INDIDevice device{"foo", INDIDevice::Interface::CCD};

    ASSERT_TRUE(device.is(INDIDevice::Interface::CCD));
    ASSERT_FALSE(device.is(INDIDevice::Interface::Telescope));

    std::set<INDIDevice::Interface> expected{INDIDevice::Interface::CCD};
    ASSERT_EQ(device.interfaces(), expected);
}

TEST(TestIndiDevice, itShouldExtractTheRightInterfacesWithMultipleInterfaces) {
    INDIDevice device{"foo", INDIDevice::Interface::CCD | INDIDevice::Interface::Telescope | INDIDevice::Interface::Aux | INDIDevice::Interface::Focuser};
    
    ASSERT_TRUE(device.is(INDIDevice::Interface::CCD));
    ASSERT_TRUE(device.is(INDIDevice::Interface::Telescope));
    ASSERT_TRUE(device.is(INDIDevice::Interface::Aux));
    ASSERT_TRUE(device.is(INDIDevice::Interface::Focuser));
    ASSERT_FALSE(device.is(INDIDevice::Interface::Dome));

    std::set<INDIDevice::Interface> expected{INDIDevice::Interface::CCD, INDIDevice::Interface::Telescope, INDIDevice::Interface::Aux, INDIDevice::Interface::Focuser};
    ASSERT_EQ(device.interfaces(), expected);
}


TEST(TestIndiParser, itShouldParseDevicesList){
  INDIParser parser;
  const char *xml = R"_(
<defTextVector device="CCD Simulator" name="DRIVER_INFO" label="Driver Info" group="General Info" state="Idle" perm="ro" timeout="60" timestamp="2024-11-20T22:53:34">
    <defText name="DRIVER_NAME" label="Name">
CCD Simulator
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_ccd
    </defText>
    <defText name="DRIVER_VERSION" label="Version">
1.0
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
22
    </defText>
</defTextVector>
<defTextVector device="Telescope Simulator" name="DRIVER_INFO" label="Driver Info" group="Connection" state="Idle" perm="ro" timeout="60" timestamp="2024-11-20T22:53:34">
    <defText name="DRIVER_NAME" label="Name">
Telescope Simulator
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_telescope
    </defText>
    <defText name="DRIVER_VERSION" label="Version">
1.0
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
5
    </defText>
</defTextVector>
  )_";
  INDIDevice::List devices;
  parser.parseDevices(xml, strlen(xml), std::back_inserter(devices));
  std::list<INDIDevice> expected {
    {"CCD Simulator", INDIDevice::Interface::CCD | INDIDevice::Interface::Guider | INDIDevice::Interface::Filter },
    {"Telescope Simulator", INDIDevice::Interface::Telescope | INDIDevice::Interface::Guider},
  };
  ASSERT_EQ(devices, expected);
}

TEST(TestIndiParser, itShouldParseDevicesListWhenBufferIsFragmented){
  INDIParser parser;
  std::initializer_list<const char*> xml_fragments{
  R"_(
<defTextVector device="CCD Simulator" name="DRIVER_INFO" label="Driver Info" group="General Info" state="Idle" perm="ro" timeout="60" timestamp="2024-11-20T22:53:34">
    <defText name="DRIVER_NAME" label="Name">
CCD Simulator
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_ccd
    </defText>
    <defText name="DRIVER_VERSION" label="Version">
1.0
)_",
  R"_(
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
22
    </defText>
</defTextVector>
<defTextVector device="Telescope Simulator" name="DRIVER_INFO" label="Driver Info" group="Connection" state="Idle" perm="ro" timeout="60" timestamp="2024-11-20T22:53:34">
    <defText name="DRIVER_NAME" label="Name">
Telescope Simulator
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_telescope
    </defText>
)_",
 R"_(
    <defText name="DRIVER_VERSION" label="Version">
1.0
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
5
    </defText>
</defTextVector>
<defTextVector device="Telescope Simulator2" name="DRIVER_INFO" label="Driver Info" group="Connection" state="Idle" perm="ro" timeout="60" timestamp="2024-11-20T22:53:34">
  )_",
  R"_(
    <defText name="DRIVER_NAME" label="Name">
Telescope Simulator2
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_telescope
    </defText>
    <defText name="DRIVER_VERSION" label="Version">
1.0
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
5
    </defText>
</defTextVector>
  )_"
  };
  INDIDevice::List devices;
  std::for_each(xml_fragments.begin(), xml_fragments.end(), [&devices, &parser](const char *xml){
    std::cout << "Sending fragment: `" << xml << "`\n\n";
    parser.parseDevices(xml, strlen(xml), std::back_inserter(devices));
  });
  std::list<INDIDevice> expected {
    {"CCD Simulator", INDIDevice::Interface::CCD | INDIDevice::Interface::Guider | INDIDevice::Interface::Filter },
    {"Telescope Simulator", INDIDevice::Interface::Telescope | INDIDevice::Interface::Guider},
    {"Telescope Simulator2", INDIDevice::Interface::Telescope | INDIDevice::Interface::Guider},
  };
  ASSERT_EQ(devices, expected);
}



TEST(TestIndiParser, itShouldOnlyParseDriverInfo){
  INDIParser parser;
  const char *xml = R"_(
<defSwitchVector device="CCD Simulator" name="CONNECTION" label="Connection" group="Main Control" state="Idle" perm="rw" rule="OneOfMany" timeout="60" timestamp="2024-11-24T16:15:28">
    <defSwitch name="CONNECT" label="Connect">
Off
    </defSwitch>
    <defSwitch name="DISCONNECT" label="Disconnect">
On
    </defSwitch>
</defSwitchVector>
<defTextVector device="CCD Simulator" name="DRIVER_INFO" label="Driver Info" group="General Info" state="Idle" perm="ro" timeout="60" timestamp="2024-11-24T16:15:28">
    <defText name="DRIVER_NAME" label="Name">
CCD Simulator
    </defText>
    <defText name="DRIVER_EXEC" label="Exec">
indi_simulator_ccd
    </defText>
    <defText name="DRIVER_VERSION" label="Version">
1.0
    </defText>
    <defText name="DRIVER_INTERFACE" label="Interface">
22
    </defText>
</defTextVector>
<defNumberVector device="CCD Simulator" name="POLLING_PERIOD" label="Polling" group="Options" state="Ok" perm="rw" timeout="0" timestamp="2024-11-24T16:15:28">
    <defNumber name="PERIOD_MS" label="Period (ms)" format="%.f" min="10" max="600000" step="1000">
1000
    </defNumber>
</defNumberVector>
<defSwitchVector device="CCD Simulator" name="DEBUG" label="Debug" group="Options" state="Ok" perm="rw" rule="OneOfMany" timeout="0" timestamp="2024-11-24T16:15:28">
    <defSwitch name="ENABLE" label="Enable">
Off
    </defSwitch>
    <defSwitch name="DISABLE" label="Disable">
On
    </defSwitch>
</defSwitchVector>
<defSwitchVector device="CCD Simulator" name="CONFIG_PROCESS" label="Configuration" group="Options" state="Idle" perm="rw" rule="AtMostOne" timeout="0" timestamp="2024-11-24T16:15:28">
    <defSwitch name="CONFIG_LOAD" label="Load">
Off
    </defSwitch>
    <defSwitch name="CONFIG_SAVE" label="Save">
Off
    </defSwitch>
    <defSwitch name="CONFIG_DEFAULT" label="Default">
Off
    </defSwitch>
    <defSwitch name="CONFIG_PURGE" label="Purge">
Off
    </defSwitch>
</defSwitchVector>

  )_";
  INDIDevice::List devices;
  parser.parseDevices(xml, strlen(xml), std::back_inserter(devices));
  std::list<INDIDevice> expected {
    {"CCD Simulator", INDIDevice::Interface::CCD | INDIDevice::Interface::Guider | INDIDevice::Interface::Filter },
  };
  ASSERT_EQ(devices, expected);
}



#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
    // should be the same value as for the `test_speed` option in "platformio.ini"
    // default value is test_speed=115200
    Serial.begin(115200);

    ::testing::InitGoogleTest();
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock();
}

void loop()
{
  // Run tests
  if (RUN_ALL_TESTS())
  ;

  // sleep for 1 sec
  delay(1000);
}

#else
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
#endif

