#include "rtc.hpp"

Rtc::Rtc(uint32_t prescaler)
{
  this->prescaler = prescaler;
}

void Rtc::setupAndStart()
{
	_enablePowerAndBackupClocks();
  _disableBackupDomainWriteProtect();

  _startLseOscillator();
  _selectLseAsSource();

  _enable();

  _enableConfigMode();
  _setPrescaler();

  _disableInterrupt();

  // clock starts ticking when config mode is disabled
	_disableConfigMode();

  _enableBackupDomainWriteProtect();
  _disablePowerAndBackupClocks();
}

void Rtc::setupAndStartWithNVICInterrupt()
{
	_enablePowerAndBackupClocks();
  _disableBackupDomainWriteProtect();

  _startLseOscillator();
  _selectLseAsSource();

  _enable();

  _enableConfigMode();
  _setPrescaler();

  _enableInterrupt();

  // clock starts ticking when config mode is disabled
	_disableConfigMode();

  _enableBackupDomainWriteProtect();
  _disablePowerAndBackupClocks();
}

void Rtc::enableNvicInterrupt(NvicInterruptPriority priority, uint32_t callback)
{
  NvicInterrupt::setPriority(RTC_IRQn, priority);
  NvicInterrupt::setHandler(RTC_IRQn, callback);
  NvicInterrupt::enableInterrupt(RTC_IRQn);
}

void Rtc::disableNvicInterrupt()
{
  NvicInterrupt::disableInterrupt(RTC_IRQn);
}


bool Rtc::isInterruptPending()
{
  return RTC->CRL & RTC_CRL_SECF;
}

void Rtc::clearInterruptPending()
{
  RTC->CRL &= ~(RTC_CRL_SECF);
}

bool Rtc::isNvicInterruptPending()
{
  return NvicInterrupt::getPending(RTC_IRQn);
}

void Rtc::clearNvicInterruptPending()
{
  NvicInterrupt::clearPending(RTC_IRQn);
}

// Enable the power and backup interface clocks
void Rtc::_enablePowerAndBackupClocks()
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
}

// Disable backup and power interface clocks (to save power)
void Rtc::_disablePowerAndBackupClocks()
{
  RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
  RCC->APB1ENR &= ~RCC_APB1ENR_BKPEN;
}

void Rtc::_enableBackupDomainWriteProtect()
{
  PWR->CR &= ~PWR_CR_DBP;
}

// Disable backup domain write-protect (RTC_BDCR register is in this memory area)
void Rtc::_disableBackupDomainWriteProtect()
{
  PWR->CR |= PWR_CR_DBP;
}

// Enable (restart) the Low-frequency external oscilator (32.768 kHz, pretty accurate)
void Rtc::_startLseOscillator()
{
  RCC->BDCR &= ~(RCC_BDCR_LSEON);
  RCC->BDCR |= RCC_BDCR_LSEON;

  // wait until the oscillator is stable
  while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0);
}

// Select the LSE as the clock source for the RTC
// clear the select flags, then select the right clock
void Rtc::_selectLseAsSource()
{
  RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
  RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
}

// Enable (restart) the RTC clock
void Rtc::_enable()
{
  RCC->BDCR &= ~(RCC_BDCR_RTCEN);
  RCC->BDCR |= RCC_BDCR_RTCEN;
}

// enables config mode so we can set the RTC registers
void Rtc::_enableConfigMode()
{
  // Poll RTOFF until 1 --> we can write to the registers
  while ((RTC->CRL & RTC_CRL_RTOFF) == 0);

  // Set CNF bit
  RTC->CRL |= RTC_CRL_CNF;
}

void Rtc::_disableConfigMode()
{
  // Clear the RTC CNF bit --> start updating the RTC counter
  RTC->CRL &= ~(RTC_CRL_CNF);

  // Check that the write operation has been done
  while ((RTC->CRL & RTC_CRL_RTOFF) == 0);
}

// Set the RTC Prescaler to divide the 32.768 kHz of the LSE
// prescaler is 20 bits long
void Rtc::_setPrescaler()
{
  // higher 4 bits
  RTC->PRLH = this->prescaler >> 16;

  // lower 16 bits
  RTC->PRLL = this->prescaler;
}

void Rtc::_enableInterrupt()
{
  RTC->CRH |= RTC_CRH_SECIE;
}

void Rtc::_disableInterrupt()
{
  RTC->CRH &= ~RTC_CRH_SECIE;
}