#include <gtest/gtest.h>
#include "protocols/indi/indiparser.h"
// uncomment line below if you plan to use GMock
// #include <gmock/gmock.h>

// TEST(...)
// TEST_F(...)

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
  std::list<std::string> expected{"CCD Simulator", "Telescope Simulator"};
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

