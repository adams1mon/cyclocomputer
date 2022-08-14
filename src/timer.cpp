#include "timer.h"

Timer::Timer(TIM_TypeDef* timer, uint16_t counter, uint16_t prescaler, uint16_t autoReloadValue)
{
  this->timer = timer;
  this->timer->CNT = counter;
  this->timer->PSC = prescaler;
  this->timer->ARR = autoReloadValue;
}

void Timer::enable()
{
  _enablePeripherals();
}

void Timer::disable()
{
  _disablePeripherals();
}

void Timer::start()
{
  this->timer->CR1 |= TIM_CR1_CEN;
}

void Timer::stop()
{
  this->timer->CR1 &= ~(TIM_CR1_CEN);
}

void Timer::setCounter(uint16_t counter)
{
  this->timer->CNT = counter;
}

uint32_t Timer::getCounter()
{
  return this->timer->CNT;
}

void Timer::enableInterrupt()
{
  // reset the interrupt flag
  this->timer->SR &= ~(TIM_SR_UIF);

  // enable timer hardware interrupt
  this->timer->DIER |= TIM_DIER_UIE;
}

void Timer::disableInterrupt()
{
  // reset the interrupt flag
  this->timer->SR &= ~(TIM_SR_UIF);

  // disable timer hardware interrupt
  this->timer->DIER &= ~TIM_DIER_UIE;
}

void Timer::enableNvicInterrupt(NvicInterruptPriority priority, uint32_t callback)
{
  IRQn_Type interrupt = _getNvicInterruptForTimer();
  NvicInterrupt::setHandler(interrupt, callback);
  NvicInterrupt::setPriority(interrupt, priority);
  NvicInterrupt::enableInterrupt(interrupt);
}

void Timer::disableNvicInterrupt()
{
  IRQn_Type interrupt = _getNvicInterruptForTimer();
  NvicInterrupt::disableInterrupt(interrupt);
}

bool Timer::isInterruptPending()
{
  return this->timer->SR & TIM_SR_UIF;
}

void Timer::clearInterruptPending()
{
  this->timer->SR &= ~(TIM_SR_UIF);
}

bool Timer::isNvicInterruptPending()
{
  return NvicInterrupt::getPending(_getNvicInterruptForTimer());
}

void Timer::clearNvicInterruptPending()
{
  NvicInterrupt::clearPending(_getNvicInterruptForTimer());
}

void Timer::_enablePeripherals()
{
  // advanced-control timers
  if (this->timer == TIM1)
  {
    RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM1RST);
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  }
  else if (this->timer == TIM2)
  {
    // general-purpose timers
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  }
  else if (this->timer == TIM3)
  {
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  }
}

void Timer::_disablePeripherals()
{
  // advanced-control timers
  if (this->timer == TIM1)
  {
    RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
  }
  else if (this->timer == TIM2)
  {
    // general-purpose timers
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
  }
  else if (this->timer == TIM3)
  {
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
  }
}

IRQn_Type Timer::_getNvicInterruptForTimer()
{
  if (this->timer == TIM2)       return TIM2_IRQn;
  else if (this->timer == TIM3)  return TIM3_IRQn;

  // shouldn't reach this point, all timers should be handled in 
  return TIM2_IRQn;
}
