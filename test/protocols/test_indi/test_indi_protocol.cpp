#include <gtest/gtest.h>
#include "protocols/indi/indiparser.h"
#include "protocols/indi/indidevice.h"
// uncomment line below if you plan to use GMock
// #include <gmock/gmock.h>

// TEST(...)
// TEST_F(...)


TEST(TestIndiDevice, itShouldStoreNameAndInterfaceAndCompareByStrEquality) {
    INDIDevice device{"foo", INDIDevice::Interface::Telescope};
    char differentPointer[4];
    strcpy(differentPointer, device.name);
    INDIDevice anotherDevice{differentPointer, INDIDevice::Interface::Telescope};
    ASSERT_NE(device.name, differentPointer);
    ASSERT_NE(device.name, anotherDevice.name);
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
  auto devices = parser.parseDevices(xml, strlen(xml));
  ASSERT_EQ(devices.size(), 2);
  ASSERT_STREQ(devices.front(), "CCD Simulator");
  ASSERT_STREQ(devices.back(), "Telescope Simulator");
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

