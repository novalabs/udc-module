#pragma once

#include <Configuration.hpp>
#include <Core/MW/CoreModule.hpp>
#include <Core/MW/CoreSensor.hpp>

namespace sensors {}

// Forward declarations
namespace sensors {
	class QEI_Delta;
}

class Module:
	public Core::MW::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------
	static sensors::QEI_Delta& qei;
// ----------------------------------------------------------------------------

	static bool
	initialize();


	Module();
	virtual ~Module() {}
};
