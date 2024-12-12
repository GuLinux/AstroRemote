#ifndef SRC_INDI_DEVICE
#define SRC_INDI_DEVICE
#include <set>
#include <enum.h>
#include <functional>
#include <algorithm>
#include <stdint.h>
#ifdef ARDUINO
#include <WString.h>
#else
#include <string>
#endif

using namespace std::placeholders;

namespace {
BETTER_ENUM(INDI_Device_Interface, uint32_t,
    General       = 0,         /**< Default interface for all INDI devices */
    Telescope     = (1 << 0),  /**< Telescope interface, must subclass INDI::Telescope */
    CCD           = (1 << 1),  /**< CCD interface, must subclass INDI::CCD */
    Guider        = (1 << 2),  /**< Guider interface, must subclass INDI::GuiderInterface */
    Focuser       = (1 << 3),  /**< Focuser interface, must subclass INDI::FocuserInterface */
    Filter        = (1 << 4),  /**< Filter interface, must subclass INDI::FilterInterface */
    Dome          = (1 << 5),  /**< Dome interface, must subclass INDI::Dome */
    GPS           = (1 << 6),  /**< GPS interface, must subclass INDI::GPS */
    Weather       = (1 << 7),  /**< Weather interface, must subclass INDI::Weather */
    AO            = (1 << 8),  /**< Adaptive Optics Interface */
    Dustcap       = (1 << 9),  /**< Dust Cap Interface */
    Lightbox      = (1 << 10), /**< Light Box Interface */
    Detector      = (1 << 11), /**< Detector interface, must subclass INDI::Detector */
    Rotator       = (1 << 12), /**< Rotator interface, must subclass INDI::RotatorInterface */
    Spectrograph  = (1 << 13), /**< Spectrograph interface */
    Correlator    = (1 << 14), /**< Correlators (interferometers) interface */
    Aux           = (1 << 15), /**< Auxiliary interface */
    Output        = (1 << 16), /**< Digital Output (e.g. Relay) interface */
    Input         = (1 << 17), /**< Digital/Analog Input (e.g. GPIO) interface */
    Power         = (1 << 18), /**< Auxiliary interface */
    Sensor        = Spectrograph | Detector | Correlator
)
}

struct INDIDevice {
    using Interface = INDI_Device_Interface;
    using List = std::list<INDIDevice>;
    using ListInserter = std::back_insert_iterator<List>;

#ifdef ARDUINO
    String name;
#else
    std::string name;
#endif
    uint32_t interface;
    bool is(Interface interface) const {
        return this->interface & interface;
    }
    std::set<Interface> interfaces() const {
        std::set<Interface> retval;
        std::copy_if(
            Interface::_values().begin(),
            Interface::_values().end(),
            std::inserter(retval, retval.begin()),
            std::bind(&INDIDevice::is, this, _1)
        );
        return retval;
    }
    bool operator==(const INDIDevice &other) const {
        return interface == other.interface && name == other.name;
    }
};
#endif