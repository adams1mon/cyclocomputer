/*
 * interrupts.h
 *
 *  Created on: Jul 17, 2022
 *      Author: Adam Simon
 */

#ifndef NVIC_INTERRUPTS_H_
#define NVIC_INTERRUPTS_H_

#include "stm32f103x6.h"
#include "core_cm3.h"

// TODO: research priorities and numbers here, ordering matters
enum class NVICInterruptPriority:uint32_t { HIGH = 1, MEDIUM = 2, LOW = 3 };

class NVICInterrupt {
  public:
    static void setPriority(IRQn_Type interrupt, NVICInterruptPriority priority)
    {
      NVIC_SetPriority(interrupt, static_cast<uint32_t>(priority));
    }

    static void enableInterrupt(IRQn_Type interrupt)
    {
      NVIC_EnableIRQ(interrupt);
    }

    static void disableInterrupt(IRQn_Type interrupt)
    {
      NVIC_DisableIRQ(interrupt);
    }

    static bool getPending(IRQn_Type interrupt)
    {
      return NVIC_GetPendingIRQ(interrupt);
    }

    static void setPending(IRQn_Type interrupt)
    {
      NVIC_SetPendingIRQ(interrupt);
    }

    static void clearPending(IRQn_Type interrupt) 
    {
      NVIC_ClearPendingIRQ(interrupt);
    }

    static uint32_t getHandler(IRQn_Type interrupt)
    {
      return NVIC_GetVector(interrupt);
    }

    static void setHandler(IRQn_Type interrupt, uint32_t handler_addr)
    {
      NVIC_SetVector(interrupt, handler_addr);
    }
};

#endif /* NVIC_INTERRUPTS_H_ */