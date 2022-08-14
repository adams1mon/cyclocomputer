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

// all timers are configured as general purpose, no advanced feature timers
// all timers are configured in edge-aligned up-counting mode (timers count from 0 to the auto-reload value)
class Timer
{
public:

  Timer(
      TIM_TypeDef* timer,
      uint16_t counter = 0,
      uint16_t prescaler = 1,
      uint16_t autoReloadValue = 0xffff
  );

  // starting timer clocks consumes current so we have different methods than start/stop to control them
  void enable();
  void disable();

  void start();
  void stop();

  void setCounter(uint16_t counter);
  uint32_t getCounter();

  void enableInterrupt();
  void disableInterrupt();

  // note: interrupt vector table must be mapped in SRAM to be able to set a callback function as the interrupt handler
  void enableNvicInterrupt(NvicInterruptPriority priority, uint32_t callback);
  void disableNvicInterrupt();

  bool isInterruptPending();
  void clearInterruptPending();

  bool isNvicInterruptPending();
  void clearNvicInterruptPending();

private:
  TIM_TypeDef* timer;
  uint16_t counter;
  uint16_t prescaler;
  uint16_t autoReloadValue;

  void _enablePeripherals();
  void _disablePeripherals();

  IRQn_Type _getNvicInterruptForTimer();
};

#endif /* TIMERHEADER_H_ */