#include "gpio.h"

// initialize private static member 
uint8_t GPIO::_config_mode = 0x00;

uint32_t GPIO::_getPortBits(GPIO_TypeDef* port)
{
  if (port == GPIOA)        return RCC_APB2ENR_IOPAEN;
  else if (port == GPIOB)   return RCC_APB2ENR_IOPBEN;
  else if (port == GPIOC)   return RCC_APB2ENR_IOPCEN;
  else if (port == GPIOD)   return RCC_APB2ENR_IOPDEN;
  return -1;
}

void GPIO::_enablePort(GPIO_TypeDef* port)
{
  RCC->APB2ENR |= GPIO::_getPortBits(port);
}

void GPIO::_disablePort(GPIO_TypeDef* port)
{
  RCC->APB2ENR &= ~GPIO::_getPortBits(port);
}

// sets CNF and MODE bits to 0
void GPIO::_clearPinConfig(GPIOPin gpioPin)
{
  if (gpioPin.pin > 7)
  {
    gpioPin.pin -= 8;
    for (int i=0; i < 4; ++i)
      gpioPin.port->CRH &= ~(1 << ((gpioPin.pin * 4) + i));
  }
  else
  {
    for (int i=0; i < 4; ++i) 
      gpioPin.port->CRL &= ~(1 << ((gpioPin.pin * 4) + i));
  }
}

void GPIO::_setPinConfig(GPIOPin gpioPin)
{
  if (gpioPin.pin > 7)
  {
    gpioPin.port->CRH |= GPIO::_config_mode << ((gpioPin.pin - 8) * 4);
  }
  else
  {
    gpioPin.port->CRL |= GPIO::_config_mode << (gpioPin.pin * 4);
  }
}

void GPIO::_setInputPinPullUp(GPIOPin gpioPin)
{
  gpioPin.port->ODR |= 1 << gpioPin.pin;
}

void GPIO::_setInputPinPullDown(GPIOPin gpioPin)
{
  gpioPin.port->ODR &= ~(1 << gpioPin.pin);
}

bool GPIO::_isInputPUPD(GPIOPin gpioPin)
{
  if (gpioPin.pin > 7)
  {
    return gpioPin.port->CRH & (static_cast<uint8_t>(GPIOInputMode::PUPD) << ((gpioPin.pin - 8) * 4));
  }
  else
  {
    return gpioPin.port->CRL & (static_cast<uint8_t>(GPIOInputMode::PUPD) << (gpioPin.pin * 4));
  }
}

bool GPIO::_isInputPinPulledUp(GPIOPin gpioPin)
{
  return gpioPin.port->ODR & (1 << gpioPin.pin);
}

uint32_t GPIO::_getInterruptPortBits(GPIO_TypeDef* port)
{
	if (port == GPIOA)      return 0;
	else if (port == GPIOB) return 1;
	else if (port == GPIOC) return 2;
	else if (port == GPIOD) return 3;
  return -1;
}

void GPIO::_enableAlternateFunctionClock()
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void GPIO::_configureExternalInterruptTrigger(int pin, GPIOInterruptTrigger trigger)
{
  switch (trigger)
  {
    case GPIOInterruptTrigger::FALLING:
		  EXTI->FTSR |= (1 << pin);
      break;
    
    case GPIOInterruptTrigger::RISING:
		  EXTI->RTSR |= (1 << pin);
      break;

    case GPIOInterruptTrigger::RISING_AND_FALLING:
		  EXTI->FTSR |= (1 << pin);
		  EXTI->RTSR |= (1 << pin);
      break;

    case GPIOInterruptTrigger::NONE:
      EXTI->FTSR &= ~(1 << pin);
		  EXTI->RTSR &= ~(1 << pin);
      break;

    default:
      // falling
      EXTI->FTSR |= (1 << pin);
      break;
  }
}

void GPIO::_enableExternalInterrupt(GPIOPin gpioPin)
{
  // set corresponding bits
	AFIO->EXTICR[gpioPin.pin / 4] |= (GPIO::_getInterruptPortBits(gpioPin.port) << ((gpioPin.pin % 4) * 4));

  // enable interrupt on chosen line
	EXTI->IMR |= (1 << gpioPin.pin);
}

void GPIO::_disableExternalInterrupt(GPIOPin gpioPin)
{
  // reset bits to 0
	AFIO->EXTICR[gpioPin.pin / 4] &= ~(GPIO::_getInterruptPortBits(gpioPin.port) << ((gpioPin.pin % 4) * 4));

  // disable interrupt on chosen line
	EXTI->IMR &= ~(1 << gpioPin.pin);
}

IRQn_Type GPIO::_getNVICInterruptForLine(int pin)
{
  if (pin == 0)                          return EXTI0_IRQn;
	else if (pin == 1)                     return EXTI1_IRQn;
	else if (pin == 2)                     return EXTI2_IRQn;
	else if (pin == 3)                     return EXTI3_IRQn;
	else if (pin == 4)                     return EXTI4_IRQn;
	else if (pin >= 5 && pin <= 9)         return EXTI9_5_IRQn;
	else if (pin >= 10 && pin <= 15)       return EXTI15_10_IRQn;

  // TODO: throw error ? (should never reach this point...)
  return EXTI0_IRQn;
}

void GPIO::setOutputPin(GPIOPin  gpioPin, GPIOOutputSpeed speed, bool alternateFunction)
{
  GPIO::_config_mode = alternateFunction ? 0x08 : 0x00;
  GPIO::_config_mode |= static_cast<uint8_t>(speed);

  GPIO::_enablePort(gpioPin.port);
  GPIO::_clearPinConfig(gpioPin);
  GPIO::_setPinConfig(gpioPin);
}

void GPIO::setInputPin(GPIOPin  gpioPin, GPIOInputMode inputMode, GPIOPullMode pullMode)
{
  GPIO::_config_mode = static_cast<uint8_t>(inputMode);

  GPIO::_enablePort(gpioPin.port);
  GPIO::_clearPinConfig(gpioPin);
  GPIO::_setPinConfig(gpioPin);

  if (GPIO::_isInputPUPD(gpioPin))
  {
    if (pullMode == GPIOPullMode::PULL_UP)
      GPIO::_setInputPinPullUp(gpioPin);
    else
      GPIO::_setInputPinPullDown(gpioPin);
  }
}

void GPIO::resetPin(GPIOPin  gpioPin)
{
  GPIO::_clearPinConfig(gpioPin);
}

void GPIO::writePin(GPIOPin  gpioPin)
{
  gpioPin.port->BSRR |= 1 << gpioPin.pin;
}

void GPIO::clearPin(GPIOPin  gpioPin)
{
  gpioPin.port->BRR |= 1 << gpioPin.pin;
}

bool GPIO::readPin(GPIOPin  gpioPin)
{
  if (GPIO::_isInputPUPD(gpioPin) && GPIO::_isInputPinPulledUp(gpioPin))
  {
    return !(gpioPin.port->IDR & (1 << gpioPin.pin));
  }
  return gpioPin.port->IDR & (1 << gpioPin.pin);
}

void GPIO::enableExternalInterrupt(GPIOPin  gpioPin, GPIOInterruptTrigger trigger)
{
  GPIO::_enableAlternateFunctionClock();
  GPIO::_configureExternalInterruptTrigger(gpioPin.pin, trigger);

  GPIO::_disableExternalInterrupt(gpioPin);
  GPIO::_enableExternalInterrupt(gpioPin);
}

void GPIO::disableExternalInterrupt(GPIOPin  gpioPin)
{
  GPIO::_disableExternalInterrupt(gpioPin);
  GPIO::_configureExternalInterruptTrigger(gpioPin.pin, GPIOInterruptTrigger::NONE);
}

void GPIO::enableNVICInterrupt(GPIOPin  gpioPin, NVICInterruptPriority priority, uint32_t callback)
{
  IRQn_Type nvicInterrupt = GPIO::_getNVICInterruptForLine(gpioPin.pin);
  NVICInterrupt::setHandler(nvicInterrupt, callback);
  NVICInterrupt::setPriority(nvicInterrupt, priority);
  NVICInterrupt::enableInterrupt(nvicInterrupt);
}

void GPIO::disableNVICInterrupt(GPIOPin  gpioPin)
{
  IRQn_Type nvicInterrupt = GPIO::_getNVICInterruptForLine(gpioPin.pin);
  NVICInterrupt::disableInterrupt(nvicInterrupt);
}

bool GPIO::isExternalInterruptPending(GPIOPin gpioPin)
{
  return (EXTI->PR & (1 << gpioPin.pin));
}

void GPIO::clearExternalInterruptPending(GPIOPin gpioPin)
{
	EXTI->PR |= (1 << gpioPin.pin);
}

bool GPIO::isNVICInterruptPending(GPIOPin gpioPin)
{
  return NVICInterrupt::getPending(GPIO::_getNVICInterruptForLine(gpioPin.pin));
}

void GPIO::clearNVICInterruptPending(GPIOPin gpioPin)
{
  NVICInterrupt::clearPending(GPIO::_getNVICInterruptForLine(gpioPin.pin));
}
