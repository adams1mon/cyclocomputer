/*
 * Timers on STM32F103C8T6
 *
 * timer.h
 *
 *  Created on: Aug 7, 2022
 *      Author: Adam Simon
 */

#ifndef TIMERHEADER_H_
#define TIMERHEADER_H_

#include "stm32f103x6.h"
#include "nvic_interrupts.h"

struct TimerInfo
{
  TIM_TypeDef* timer;
  uint16_t counter;
  uint16_t prescaler;
  uint16_t autoReloadValue;
};

// all timers are configured as general purpose, no advanced feature timers
// all timers are configured in edge-aligned up-counting mode
class Timer
{
public:

  static void setup(TimerInfo* timerInfo);
  static void enable(TimerInfo* timerInfo);
  static void disable(TimerInfo* timerInfo);

  static uint32_t getCount(TimerInfo* timerInfo);

  static void enableInterrupt(TimerInfo* timerInfo);
  static void disableInterrupt(TimerInfo* timerInfo);
  
  // note: interrupt vector table must be mapped in SRAM to be able to set a callback function as the interrupt handler
  static void enableNVICInterrupt(TimerInfo* timerInfo, NVICInterruptPriority priority, uint32_t callback);
  static void disableNVICInterrupt(TimerInfo* timerInfo);

  static bool isInterruptPending(TimerInfo* timerInfo);
  static void clearInterruptPending(TimerInfo* timerInfo);

  static bool isNVICInterruptPending(TimerInfo* timerInfo);
  static void clearNVICInterruptPending(TimerInfo* timerInfo);

private:

  static void _enablePeripherals(TimerInfo* timerInfo);
  static void _disablePeripherals(TimerInfo* timerInfo);

  static IRQn_Type _getNVICInterruptForTimer(TimerInfo* timerInfo);
};


#endif /* TIMERHEADER_H_ */