#include <Core/MW/Middleware.hpp>

#include "ch.h"
#include "hal.h"

#include <Core/HW/QEI.hpp>
#include <Core/MW/Thread.hpp>
#include <Module.hpp>
#include <sensors/QEI.hpp>

static Core::HW::QEI_<Core::HW::QEI_4> ENCODER_DEVICE;

using LED_PAD = Core::HW::Pad_<Core::HW::GPIO_C, LED_PIN>;
static LED_PAD _led;

static sensors::QEI       _qei_device(ENCODER_DEVICE);
static sensors::QEI_Delta _qei_delta(_qei_device);

sensors::QEI_Delta& Module::qei = _qei_delta;


static THD_WORKING_AREA(wa_info, 1024);
static Core::MW::RTCANTransport rtcantra(RTCAND1);

RTCANConfig rtcan_config = {
	1000000, 100, 60
};

QEIConfig qei_config = {
	QEI_MODE_QUADRATURE, QEI_BOTH_EDGES, QEI_DIRINV_FALSE,
};

#ifndef CORE_MODULE_NAME
#define CORE_MODULE_NAME "UDC"
#endif

Core::MW::Middleware Core::MW::Middleware::instance(CORE_MODULE_NAME, "BOOT_" CORE_MODULE_NAME);

Module::Module()
{}

bool
Module::initialize()
{
//	CORE_ASSERT(Core::MW::Middleware::instance.is_stopped()); // TODO: capire perche non va...

	static bool initialized = false;

	if (!initialized) {
		halInit();
		qeiInit();

		chSysInit();

		Core::MW::Middleware::instance.initialize(wa_info, sizeof(wa_info), Core::MW::Thread::LOWEST);
		rtcantra.initialize(rtcan_config);
		Core::MW::Middleware::instance.start();


		ENCODER_DEVICE.start(&qei_config);

		initialized = true;
	}

	return initialized;
} // Board::initialize

// Leftover from CoreBoard (where LED_PAD cannot be defined
void
Core::MW::CoreModule::Led::toggle()
{
	_led.toggle();
}

void
Core::MW::CoreModule::Led::write(
		unsigned on
)
{
	_led.write(on);
}
