/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

#include "ch.h"
#include "hal.h"

#include <core/hw/GPIO.hpp>
#include <core/hw/QEI.hpp>
#include <core/hw/PWM.hpp>
#include <core/os/Thread.hpp>
#include <Module.hpp>
#include <core/QEI_driver/QEI.hpp>
#include <core/MC33926_driver/MC33926.hpp>

static core::hw::QEI_<core::hw::QEI_4> ENCODER_DEVICE;

static core::hw::PWMChannel_<core::hw::PWM_1, 0> PWM_CHANNEL_0;
static core::hw::PWMChannel_<core::hw::PWM_1, 1> PWM_CHANNEL_1;

using LED_PAD = core::hw::Pad_<core::hw::GPIO_F, LED_PIN>;
static LED_PAD _led;

using HBRIDGE_ENABLE_PAD = core::hw::Pad_<core::hw::GPIO_B, GPIOB_MOTOR_ENABLE>;
using HBRIDGE_D1_PAD     = core::hw::Pad_<core::hw::GPIO_A, GPIOA_MOTOR_D1>;

static HBRIDGE_ENABLE_PAD _hbridge_enable;
static HBRIDGE_D1_PAD     _hbridge_d1;

static core::QEI_driver::QEI       _qei_device(ENCODER_DEVICE);
static core::QEI_driver::QEI_Delta _qei_delta("m_encoder", _qei_device);

static core::MC33926_driver::MC33926 _pwm_device(PWM_CHANNEL_0, PWM_CHANNEL_1, _hbridge_enable, _hbridge_d1);
static core::MC33926_driver::MC33926_SignMagnitude _pwm("m_motor", _pwm_device);

core::QEI_driver::QEI_Delta& Module::qei = _qei_delta;
core::MC33926_driver::MC33926_SignMagnitude& Module::pwm = _pwm;

static core::os::Thread::Stack<1024> management_thread_stack;
static core::mw::RTCANTransport      rtcantra(RTCAND1);

RTCANConfig rtcan_config = {
   1000000, 100, 60
};

QEIConfig qei_config = {
   QEI_MODE_QUADRATURE, QEI_BOTH_EDGES, QEI_DIRINV_FALSE,
};

/*
 * PWM configuration.
 */
static PWMConfig pwmcfg = {
   36000000,                         /* 36MHz PWM clock.   */
   4096,                             /* 12-bit PWM, 9KHz frequency. */
   nullptr,                          {{PWM_OUTPUT_ACTIVE_HIGH, NULL}, {PWM_OUTPUT_ACTIVE_HIGH, NULL}, {PWM_OUTPUT_DISABLED, NULL}, {
                                         PWM_OUTPUT_DISABLED, NULL
                                      }}, 0,
};

#ifndef CORE_MODULE_NAME
#define CORE_MODULE_NAME "UDC"
#endif

core::mw::Middleware core::mw::Middleware::instance(CORE_MODULE_NAME, "BOOT_" CORE_MODULE_NAME);

Module::Module()
{}

bool
Module::initialize()
{
//	CORE_ASSERT(core::mw::Middleware::instance.is_stopped()); // TODO: capire perche non va...

   static bool initialized = false;

   if (!initialized) {
      halInit();
      qeiInit();

      chSysInit();

      core::mw::Middleware::instance.initialize(management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
      rtcantra.initialize(rtcan_config);
      core::mw::Middleware::instance.start();


      ENCODER_DEVICE.start(&qei_config);
      pwmStart(core::hw::PWM_1::driver, &pwmcfg);

      initialized = true;
   }

   return initialized;
} // Board::initialize

// Leftover from CoreBoard (where LED_PAD cannot be defined
void
core::mw::CoreModule::Led::toggle()
{
   _led.toggle();
}

void
core::mw::CoreModule::Led::write(
   unsigned on
)
{
   _led.write(on);
}
