#include "stm32f103x6.h"
#include "core_cm3.h"


#include "gpio.h"
#include "timer.h"
#include "rtc.h"


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

// //Alternates blue and green LEDs quickly
// int main(void)
// {
//     ENABLE_GPIO_CLOCK;              // enable the clock to GPIO
//     LEDPORT->_MODER |= GPIOMODER;   // set pins to be general purpose output
//     for (;;) {
//     ms_delay(500);
//     LEDPORT->ODR ^= (1<<LED1);  // toggle diodes
//     }

//     return 0;
// }

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


int main() {

  out_pin.setAsOutputPin();
  pd_in_pin.setAsInputPin(GpioInputMode::PUPD, GpioPullMode::PULL_DOWN);
  pu_in_pin.setAsInputPin(GpioInputMode::PUPD, GpioPullMode::PULL_UP);

  pd_in_pin.enableNvicInterrupt(NvicInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_gpio);

  // rewrites previous nvic interrupt handler
  pd_in_pin.enableNvicInterrupt(NvicInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_gpio);

  pd_in_pin.enableExternalInterrupt(GpioInterruptTrigger::RISING);
  pu_in_pin.enableExternalInterrupt(GpioInterruptTrigger::RISING);


  // timer_2.enable();
  // timer_2.enableNvicInterrupt(NvicInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_timer);
  // timer_2.enableInterrupt();
  // timer_2.start();

  rtc.enableNvicInterrupt(NvicInterruptPriority::HIGH, (uint32_t) toggle_led_with_timer);
  rtc.enableInterrupt();
  rtc.setupAndStart();


  while (true)
  {
  
  }

  return 0;
}


