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


enum class GPIOOutputSpeed:uint8_t { LOW = 0x02, MEDIUM = 0x01, HIGH = 0x03 };
enum class GPIOInputMode:uint8_t { ANALOG = 0x00, FLOATING = 0x04, PUPD = 0x08 };
enum class GPIOPullMode { PULL_DOWN, PULL_UP };
enum class GPIOInterruptTrigger:uint8_t { NONE, RISING, FALLING, RISING_AND_FALLING };
 
struct GPIOPin 
{
  GPIO_TypeDef* port;
  int pin;
};

// alternate function not implemented
class GPIO {

private:
  // config and mode bits
  static uint8_t _config_mode; 

  static uint32_t _getPortBits(GPIO_TypeDef* port);
  static void _enablePort(GPIO_TypeDef* port);
  static void _disablePort(GPIO_TypeDef* port);

  static void _clearPinConfig(GPIOPin gpioPin);
  static void _setPinConfig(GPIOPin gpioPin);

  static void _setInputPinPullUp(GPIOPin gpioPin);
  static void _setInputPinPullDown(GPIOPin gpioPin);
  static bool _isInputPUPD(GPIOPin gpioPin);
  static bool _isInputPinPulledUp(GPIOPin gpioPin);

  static uint32_t _getInterruptPortBits(GPIO_TypeDef* port);
  static void _enableAlternateFunctionClock();
  static void _configureExternalInterruptTrigger(int pin, GPIOInterruptTrigger trigger);
  static void _enableExternalInterrupt(GPIOPin gpioPin);
  static void _disableExternalInterrupt(GPIOPin gpioPin);

  static IRQn_Type _getNVICInterruptForLine(int pin);

public:

  static void init();

  // set the config and mode bits before setting a particular pin to input or output (NOT STATELESS methods)
  static void setOutputPin(
    GPIOPin gpioPin,
    GPIOOutputSpeed speed = GPIOOutputSpeed::LOW, 
    bool alternateFunction = false
  );
  static void setInputPin(
    GPIOPin gpioPin,
    GPIOInputMode inputMode = GPIOInputMode::FLOATING, 
    GPIOPullMode pullMode = GPIOPullMode::PULL_DOWN
  );
  static void resetPin(GPIOPin gpioPin);

  static void writePin(GPIOPin gpioPin);
  static void clearPin(GPIOPin gpioPin);
  static bool readPin(GPIOPin gpioPin);

  static void enableExternalInterrupt(GPIOPin gpioPin, GPIOInterruptTrigger trigger);
  static void disableExternalInterrupt(GPIOPin gpioPin);

  // note: interrupt vector table must be mapped in SRAM to be able to set a callback function as the interrupt handler
  static void enableNVICInterrupt(GPIOPin  gpioPin, NVICInterruptPriority priority, uint32_t callback);
  static void disableNVICInterrupt(GPIOPin  gpioPin);

  static bool isExternalInterruptPending(GPIOPin gpioPin);
  static void clearExternalInterruptPending(GPIOPin gpioPin);

  static bool isNVICInterruptPending(GPIOPin gpioPin);
  static void clearNVICInterruptPending(GPIOPin gpioPin);
};

#endif /* GPIO_H_ */
