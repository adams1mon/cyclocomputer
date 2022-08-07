#include "timer.h"

void Timer::setup(TimerInfo* timerInfo)
{
  Timer::_enablePeripherals(timerInfo);
  timerInfo->timer->CNT = timerInfo->counter;
	timerInfo->timer->PSC = timerInfo->prescaler;
	timerInfo->timer->ARR = timerInfo->autoReloadValue;
}

void Timer::enable(TimerInfo* timerInfo)
{
	timerInfo->timer->CR1 |= TIM_CR1_CEN;
}

void Timer::disable(TimerInfo* timerInfo) 
{
	timerInfo->timer->CR1 &= ~(TIM_CR1_CEN);
  // TODO: disable peripherals as well ?
}

uint32_t Timer::getCount(TimerInfo* timerInfo)
{
  return timerInfo->timer->CNT; 
}

void Timer::enableInterrupt(TimerInfo* timerInfo)
{
  // reset the interrupt flag
	timerInfo->timer->SR &= ~(TIM_SR_UIF);

  // enable timer hardware interrupt
	timerInfo->timer->DIER |= TIM_DIER_UIE;
}

void Timer::disableInterrupt(TimerInfo* timerInfo)
{
  // reset the interrupt flag
	timerInfo->timer->SR &= ~(TIM_SR_UIF);

  // disable timer hardware interrupt
	timerInfo->timer->DIER &= ~TIM_DIER_UIE;
}

void Timer::enableNVICInterrupt(TimerInfo* timerInfo, NVICInterruptPriority priority, uint32_t callback)
{
  IRQn_Type interrupt = Timer::_getNVICInterruptForTimer(timerInfo);
  NVICInterrupt::setHandler(interrupt, callback);
  NVICInterrupt::setPriority(interrupt, priority);
  NVICInterrupt::enableInterrupt(interrupt);
}

void Timer::disableNVICInterrupt(TimerInfo* timerInfo)
{
  IRQn_Type interrupt = Timer::_getNVICInterruptForTimer(timerInfo);
  NVICInterrupt::disableInterrupt(interrupt);
}

bool Timer::isInterruptPending(TimerInfo* timerInfo)
{
  return timerInfo->timer->SR & TIM_SR_UIF;
}

void Timer::clearInterruptPending(TimerInfo* timerInfo)
{
  timerInfo->timer->SR &= ~(TIM_SR_UIF);
}

bool Timer::isNVICInterruptPending(TimerInfo* timerInfo)
{
  return NVICInterrupt::getPending(Timer::_getNVICInterruptForTimer(timerInfo));
}

void Timer::clearNVICInterruptPending(TimerInfo* timerInfo)
{
  NVICInterrupt::clearPending(Timer::_getNVICInterruptForTimer(timerInfo));
}

void Timer::_enablePeripherals(TimerInfo* timerInfo)
{
  // advanced-control timers
	if (timerInfo->timer == TIM1) 
  {
		RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
		RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM1RST);
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	} 
  else if (timerInfo->timer == TIM2) 
  {
	  // general-purpose timers
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	} 
  else if (timerInfo->timer == TIM3) {
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  }
}

void Timer::_disablePeripherals(TimerInfo* timerInfo)
{
  // advanced-control timers
	if (timerInfo->timer == TIM1) 
  {
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
	} 
  else if (timerInfo->timer == TIM2) 
  {
	  // general-purpose timers
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
	} 
  else if (timerInfo->timer == TIM3) {
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
  }
}

IRQn_Type Timer::_getNVICInterruptForTimer(TimerInfo* timerInfo)
{
  if (timerInfo->timer == TIM2)        return TIM2_IRQn;
  else if (timerInfo->timer == TIM3)   return TIM3_IRQn;

  return TIM2_IRQn;
}
