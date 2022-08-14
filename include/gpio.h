/*
 * gpio.h
 *
 *  Created on: Jul 10, 2022
 *      Author: Adam Simon
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f103x6.h"
#include "core_cm3.h"

#include "nvic_interrupts.h"


enum class GpioOutputSpeed:uint8_t { LOW = 0x02, MEDIUM = 0x01, HIGH = 0x03 };
enum class GpioInputMode:uint8_t { ANALOG = 0x00, FLOATING = 0x04, PUPD = 0x08 };
enum class GpioPullMode { PULL_DOWN, PULL_UP };
enum class GpioInterruptTrigger:uint8_t { NONE, RISING, FALLING, RISING_AND_FALLING };
 
 
// alternate function not implemented
class GpioPin {

public:

  GpioPin(GPIO_TypeDef* port, int pin);

  // set the config and mode bits before setting a particular pin to input or output (NOT STATELESS methods)
  void setAsOutputPin(
    GpioOutputSpeed speed = GpioOutputSpeed::LOW, 
    bool alternateFunction = false
  );

  void setAsInputPin(
    GpioInputMode inputMode = GpioInputMode::FLOATING, 
    GpioPullMode pullMode = GpioPullMode::PULL_DOWN
  );

  void resetPin();

  void writePin();
  void clearPin();
  bool readPin();

  void enableExternalInterrupt(GpioInterruptTrigger trigger);
  void disableExternalInterrupt();

  // enables NVIC interrupt on ALL gpio ports for a given pin,
  // there are cases when 1 interrupt line is assigned to multiple pins on all ports (9-5, 15-10)
  // TODO: introduce error handling (throw an error if the interrupt of a given line has already been set)
  // note: interrupt vector table must be mapped in SRAM to be able to set a callback function as the interrupt handler
  void enableNvicInterrupt(NvicInterruptPriority priority, uint32_t callback);
  void disableNvicInterrupt();

  bool isExternalInterruptPending();
  void clearExternalInterruptPending();

  bool isNvicInterruptPending();
  void clearNvicInterruptPending();

private:

  GPIO_TypeDef* port;
  int pin;

  uint32_t _getPortBits();
  void _enablePort();
  void _disablePort();

  void _clearPinConfig();
  void _setPinConfig(uint8_t config_mode);

  void _setInputPinPullUp();
  void _setInputPinPullDown();
  bool _isInputPUPD();
  bool _isInputPinPulledUp();

  uint32_t _getInterruptPortBits();
  void _enableAlternateFunctionClock();
  void _configureExternalInterruptTrigger(int pin, GpioInterruptTrigger trigger);
  void _enableExternalInterrupt();
  
  void _disableExternalInterrupt();

  IRQn_Type _getNvicInterruptForLine(int pin);
};

#endif /* GPIO_H_ */
