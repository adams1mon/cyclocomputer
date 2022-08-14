/*
 * Timers on STM32F103C8T6
 *
 * timer.h
 *
 *  Created on: Aug 14, 2022
 *      Author: Adam Simon
 */

#ifndef RTCHEADER_H_
#define RTCHEADER_H_

#include "stm32f103x6.h"
#include "nvic_interrupts.h"

// RTC final frequency = Input Clock Frequency / (PRESCALER + 1)

// prescaler value to yield seconds using the LSE 32.768kHz crystal
#define RTC_PRESCALER_SECONDS 0x00007FFF

// real-time clock abstraction 
// doesn't support setting counter and alarm values, and enabling alarm interrupts
class Rtc
{
public:
  Rtc(uint32_t prescaler = RTC_PRESCALER_SECONDS);

  void setupAndStart();

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
  // 20-bit value
  uint32_t prescaler;

	void _enablePowerAndBackupClocks();
  void _disableBackupDomainWriteProtect();

  void _enableBackupDomainWriteProtect();
  void _disablePowerAndBackupClocks();

  void _enableConfigMode();
	void _disableConfigMode();

  void _enable();

  void _startLseOscillator();
  void _selectLseAsSource();

  void _setPrescaler();
};


#endif /* RTCHEADER_H_ */