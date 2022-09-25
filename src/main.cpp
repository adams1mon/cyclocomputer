#include "stm32f103x6.h"
#include "core_cm3.h"


#include "gpio.hpp"
#include "timer.hpp"
#include "rtc.hpp"
#include "event.hpp"

#include "nokia5110_display.hpp"


#define LEDPORT (GPIOC)
#define LED1 (13)
#define ENABLE_GPIO_CLOCK (RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define _MODER    CRH
#define GPIOMODER (GPIO_CRH_MODE13_0)

void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=500;
      while (x-- > 0)
         __asm("nop");
   }
}

Rtc rtc;

Timer timer_2(TIM2);

bool on = false;

GpioPin out_pin(GPIOA, 10);
GpioPin pd_in_pin(GPIOA, 9);
GpioPin pu_in_pin(GPIOB, 5);

void toggle_led() {
  on = !on;
  if (on)
    out_pin.writePin();
  else
    out_pin.clearPin();
}

void toggle_led_with_gpio() {

  toggle_led();

  pd_in_pin.clearExternalInterruptPending();
  pu_in_pin.clearExternalInterruptPending();
}

void toggle_led_with_timer() {
  toggle_led();
  ms_delay(100);
  timer_2.clearInterruptPending();
}

void toggle_led_with_rtc() {
  toggle_led();
  ms_delay(2000);
  toggle_led();
  rtc.clearInterruptPending();
  rtc.clearNvicInterruptPending();
}

void toggle_led_with_rtc_0() {
  toggle_led();
  ms_delay(200);
  toggle_led();
  ms_delay(500);
}


Event rtcEvent;

void emitRtcEvent() {
  rtcEvent.emit();
}



// ===============  LCD  ===============
// NokiaDisplayConfig nokiaConfig = {
//   rst: GpioPin(GPIOA, 6),
//   ce: GpioPin(GPIOA, 5),
//   dc: GpioPin(GPIOA, 4),
//   din: GpioPin(GPIOA, 3),
//   clk: GpioPin(GPIOA, 2),
// };

GpioPin rst(GPIOA, 6);
GpioPin ce(GPIOA, 5);
GpioPin dc(GPIOA, 4);
GpioPin din(GPIOA, 3);
GpioPin clk(GPIOA, 2);

NokiaDisplayConfig nokiaConfig = {
  rst, ce, dc, din, clk
};

GpioPin nokiaLcdVcc(GPIOA, 1);
NokiaDisplay nokiaDisplay(&nokiaConfig);

// ====================================


int main() {

  out_pin.setAsOutputPin();
  pd_in_pin.setAsInputPin(GpioInputMode::PUPD, GpioPullMode::PULL_DOWN);
  pu_in_pin.setAsInputPin(GpioInputMode::PUPD, GpioPullMode::PULL_UP);

  // rewrites previous nvic interrupt handler
  pd_in_pin.enableNvicInterrupt(NvicInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_gpio);

  pd_in_pin.enableExternalInterrupt(GpioInterruptTrigger::RISING);
  pu_in_pin.enableExternalInterrupt(GpioInterruptTrigger::RISING);


  // timer_2.enable();
  // timer_2.enableNvicInterrupt(NvicInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_timer);
  // timer_2.enableInterrupt();
  // timer_2.start();

  rtcEvent.addListener((uint32_t) (toggle_led_with_rtc_0));
  rtcEvent.addListener((uint32_t) (toggle_led_with_rtc));

  // rtc.enableNvicInterrupt(NvicInterruptPriority::HIGH, (uint32_t) toggle_led_with_rtc);
  rtc.enableNvicInterrupt(NvicInterruptPriority::HIGH, (uint32_t) emitRtcEvent);
  rtc.setupAndStartWithNVICInterrupt();


  nokiaLcdVcc.writePin();
  nokiaDisplay.init();
  nokiaDisplay.drawPixel({20, 20});


  while (true)
  {
  
  }

  return 0;
}


