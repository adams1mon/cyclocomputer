#include "stm32f103x6.h"
#include "core_cm3.h"


#include "gpio.h"
#include "timer.h"


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

GPIOPin out_pin {GPIOA, 10};
GPIOPin pd_in_pin {GPIOA, 9};
GPIOPin pu_in_pin {GPIOB, 5};

TimerInfo timer_2 {TIM2, 0, 0xffff, 1};

// void EXTI9_5_IRQHandler()
// {
//   // GPIO::setOutputPin(out_pin);
//   GPIO::writePin(out_pin);

//   ms_delay(1000);
//   GPIO::clearPin(out_pin);

//   GPIO::clearInterruptPending(pd_in_pin);
//   GPIO::clearInterruptPending(pu_in_pin);
// }

bool on = false;

void toggle_led() {
  on = !on;
  if (on)
    GPIO::writePin(out_pin);
  else
    GPIO::clearPin(out_pin);
}

void toggle_led_with_gpio() {

  toggle_led();

  GPIO::clearExternalInterruptPending(pd_in_pin);
  GPIO::clearExternalInterruptPending(pu_in_pin);
}

void toggle_led_with_timer() {
  toggle_led();
  ms_delay(100);
  Timer::clearInterruptPending(&timer_2);
}


int main() {

  GPIO::setOutputPin(out_pin);

  GPIO::setInputPin(pd_in_pin, GPIOInputMode::PUPD, GPIOPullMode::PULL_DOWN);

  GPIO::setInputPin(pu_in_pin, GPIOInputMode::PUPD, GPIOPullMode::PULL_UP);

  GPIO::enableNVICInterrupt(pd_in_pin, NVICInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_gpio);
  GPIO::enableNVICInterrupt(pu_in_pin, NVICInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_gpio);

  GPIO::enableExternalInterrupt(pd_in_pin, GPIOInterruptTrigger::RISING);
  GPIO::enableExternalInterrupt(pu_in_pin, GPIOInterruptTrigger::RISING);


  Timer::setup(&timer_2);

  Timer::enableNVICInterrupt(&timer_2, NVICInterruptPriority::MEDIUM, (uint32_t) toggle_led_with_timer);
  Timer::enableInterrupt(&timer_2);

  Timer::enable(&timer_2);
  

  // RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;


  // set corresponding bits
	// AFIO->EXTICR[pu_in_pin.pin / 4] |= (1 << ((pu_in_pin.pin % 4) * 4));

  // EXTI->FTSR |= (1 << pu_in_pin.pin);


  // enable interrupt on chosen line
	// EXTI->IMR |= (1 << pu_in_pin.pin);

  // NVIC_SetPriority(EXTI9_5_IRQn, 2);
  // NVIC_EnableIRQ(EXTI9_5_IRQn);

  // EXTI->PR |= (1 << pu_in_pin.pin);

  while (true)
  {
    // if (EXTI->PR & (1 << pu_in_pin.pin))
    // {
    //   GPIO::writePin(out_pin);

	  //   EXTI->PR |= (1 << pu_in_pin.pin);
    // }
    // else
    // {
    //   GPIO::clearPin(out_pin);
    // }

    // if (on) 
    //   GPIO::writePin(out_pin);
    // else
    //   GPIO::clearPin(out_pin);



    // if (GPIO::isInterruptPending(pu_in_pin))
    // {
    //   GPIO::writePin(out_pin);
    //   GPIO::clearInterruptPending(pu_in_pin);
    // }
    
    // if (GPIO::isInterruptPending(pd_in_pin))
    // {
    //   GPIO::clearPin(out_pin);
    //   GPIO::clearInterruptPending(pd_in_pin);
    // }

    // if (Timer::isInterruptPending(&timer_2))
    // {
    //   toggle_led();

    //   Timer::clearInterruptPending(&timer_2);
    // }

    // if (GPIO::readPin(pd_in_pin) || GPIO::readPin(pu_in_pin))
    // {
    //   GPIO::writePin(out_pin);
    // }
    // else
    // {
    //   GPIO::clearPin(out_pin);
    // }

    // GPIO::clearPin(GPIOA, out_pin);
  }

  return 0;
}


