/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <ModuleConfiguration.hpp>
#include <core/mw/CoreModule.hpp>

// Forward declarations
namespace core {
namespace QEI_driver {
class QEI_Delta;
}
}

namespace core {
namespace MC33926_driver {
class MC33926_SignMagnitude;
}
}

class Module:
    public core::mw::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------
    static core::QEI_driver::QEI_Delta& qei;
    static core::MC33926_driver::MC33926_SignMagnitude& pwm;
// ----------------------------------------------------------------------------

    static bool
    initialize();


    Module();
    virtual ~Module() {}
};
