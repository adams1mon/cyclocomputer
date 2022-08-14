#include "gpio.h"

GpioPin::GpioPin(GPIO_TypeDef* port, int pin)
{
  this->port = port;
  this->pin = pin;
}

void GpioPin::setAsOutputPin(GpioOutputSpeed speed, bool alternateFunction)
{
  uint8_t config_mode = (alternateFunction ? 0x08 : 0x00);
  config_mode |= static_cast<uint8_t>(speed);

  _enablePort();
  _clearPinConfig();
  _setPinConfig(config_mode);
}

void GpioPin::setAsInputPin(GpioInputMode inputMode, GpioPullMode pullMode)
{
  uint8_t config_mode = static_cast<uint8_t>(inputMode);

  _enablePort();
  _clearPinConfig();
  _setPinConfig(config_mode);

  // check if the _setPinConfig set the pin as input pull-up or pull-down
  if (_isInputPUPD())
  {
    if (pullMode == GpioPullMode::PULL_UP)
      _setInputPinPullUp();
    else
      _setInputPinPullDown();
  }
}

void GpioPin::resetPin()
{
  _clearPinConfig();
}

void GpioPin::writePin()
{
  this->port->BSRR |= 1 << this->pin;
}

void GpioPin::clearPin()
{
  this->port->BRR |= 1 << this->pin;
}

bool GpioPin::readPin()
{
  if (_isInputPUPD() && _isInputPinPulledUp())
  {
    return !(this->port->IDR & (1 << this->pin));
  }
  return this->port->IDR & (1 << this->pin);
}

void GpioPin::enableExternalInterrupt(GpioInterruptTrigger trigger)
{
  _enableAlternateFunctionClock();
  _configureExternalInterruptTrigger(this->pin, trigger);

  _disableExternalInterrupt();
  _enableExternalInterrupt();
}

void GpioPin::disableExternalInterrupt()
{
  _disableExternalInterrupt();
  _configureExternalInterruptTrigger(this->pin, GpioInterruptTrigger::NONE);
}

// enables NVIC interrupt on ALL gpio ports for a given pin,
// there are cases when 1 interrupt line is assigned to multiple pins on all ports (9-5, 15-10)
// TODO: introduce error handling (throw an error if the interrupt of a given line has already been set)
void GpioPin::enableNvicInterrupt(NvicInterruptPriority priority, uint32_t callback)
{
  IRQn_Type nvicInterrupt = _getNvicInterruptForLine(this->pin);
  NvicInterrupt::setHandler(nvicInterrupt, callback);
  NvicInterrupt::setPriority(nvicInterrupt, priority);
  NvicInterrupt::enableInterrupt(nvicInterrupt);
}

void GpioPin::disableNvicInterrupt()
{
  IRQn_Type nvicInterrupt = _getNvicInterruptForLine(this->pin);
  NvicInterrupt::disableInterrupt(nvicInterrupt);
}

bool GpioPin::isExternalInterruptPending()
{
  return (EXTI->PR & (1 << this->pin));
}

void GpioPin::clearExternalInterruptPending()
{
  EXTI->PR |= (1 << this->pin);
}

bool GpioPin::isNvicInterruptPending()
{
  return NvicInterrupt::getPending(_getNvicInterruptForLine(this->pin));
}

void GpioPin::clearNvicInterruptPending()
{
  NvicInterrupt::clearPending(_getNvicInterruptForLine(this->pin));
}


uint32_t GpioPin::_getPortBits()
{
  if (this->port == GPIOA)       return RCC_APB2ENR_IOPAEN;
  else if (this->port == GPIOB)  return RCC_APB2ENR_IOPBEN;
  else if (this->port == GPIOC)  return RCC_APB2ENR_IOPCEN;
  else if (this->port == GPIOD)  return RCC_APB2ENR_IOPDEN;
  return -1;
}

void GpioPin::_enablePort()
{
  RCC->APB2ENR |= _getPortBits();
}

void GpioPin::_disablePort()
{
  RCC->APB2ENR &= ~_getPortBits();
}

// sets CNF and MODE bits to 0
void GpioPin::_clearPinConfig()
{
  if (this->pin > 7)
  {
    for (int i = 0; i < 4; ++i)
      this->port->CRH &= ~(1 << (((this->pin - 8) * 4) + i));
  }
  else
  {
    for (int i = 0; i < 4; ++i)
      this->port->CRL &= ~(1 << ((this->pin * 4) + i));
  }
}

// set CNF and MODE bits on the pin
void GpioPin::_setPinConfig(uint8_t config_mode)
{
  if (this->pin > 7)
  {
    this->port->CRH |= config_mode << ((this->pin - 8) * 4);
  }
  else
  {
    this->port->CRL |= config_mode << (this->pin * 4);
  }
}

void GpioPin::_setInputPinPullUp()
{
  this->port->ODR |= 1 << this->pin;
}

void GpioPin::_setInputPinPullDown()
{
  this->port->ODR &= ~(1 << this->pin);
}

bool GpioPin::_isInputPUPD()
{
  if (this->pin > 7)
  {
    return this->port->CRH & (static_cast<uint8_t>(GpioInputMode::PUPD) << ((this->pin - 8) * 4));
  }
  else
  {
    return this->port->CRL & (static_cast<uint8_t>(GpioInputMode::PUPD) << (this->pin * 4));
  }
}

bool GpioPin::_isInputPinPulledUp()
{
  return this->port->ODR & (1 << this->pin);
}

uint32_t GpioPin::_getInterruptPortBits()
{
  if (this->port == GPIOA)         return 0;
  else if (this->port == GPIOB)    return 1;
  else if (this->port == GPIOC)    return 2;
  else if (this->port == GPIOD)    return 3;
  return -1;
}

void GpioPin::_enableAlternateFunctionClock()
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void GpioPin::_configureExternalInterruptTrigger(int triggerPin, GpioInterruptTrigger trigger)
{
  switch (trigger)
  {
  case GpioInterruptTrigger::FALLING:
    EXTI->FTSR |= (1 << triggerPin);
    break;

  case GpioInterruptTrigger::RISING:
    EXTI->RTSR |= (1 << triggerPin);
    break;

  case GpioInterruptTrigger::RISING_AND_FALLING:
    EXTI->FTSR |= (1 << triggerPin);
    EXTI->RTSR |= (1 << triggerPin);
    break;

  case GpioInterruptTrigger::NONE:
    EXTI->FTSR &= ~(1 << triggerPin);
    EXTI->RTSR &= ~(1 << triggerPin);
    break;

  default:
    // falling
    EXTI->FTSR |= (1 << triggerPin);
    break;
  }
}

void GpioPin::_enableExternalInterrupt()
{
  // set corresponding bits
  AFIO->EXTICR[this->pin / 4] |= (_getInterruptPortBits() << ((this->pin % 4) * 4));

  // enable interrupt on chosen line
  EXTI->IMR |= (1 << this->pin);
}

void GpioPin::_disableExternalInterrupt()
{
  // reset bits to 0
  AFIO->EXTICR[this->pin / 4] &= ~(_getInterruptPortBits() << ((this->pin % 4) * 4));

  // disable interrupt on chosen line
  EXTI->IMR &= ~(1 << this->pin);
}

IRQn_Type GpioPin::_getNvicInterruptForLine(int pin)
{
  if (pin == 0)                       return EXTI0_IRQn;
  else if (pin == 1)                  return EXTI1_IRQn;
  else if (pin == 2)                  return EXTI2_IRQn;
  else if (pin == 3)                  return EXTI3_IRQn;
  else if (pin == 4)                  return EXTI4_IRQn;
  else if (pin >= 5 && pin <= 9)      return EXTI9_5_IRQn;
  else if (pin >= 10 && pin <= 15)    return EXTI15_10_IRQn;

  // TODO: throw error ? (should never reach this point...)
  return EXTI0_IRQn;
}
