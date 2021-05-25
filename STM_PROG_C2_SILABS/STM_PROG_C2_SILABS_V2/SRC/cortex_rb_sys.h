//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __CORTEX_RB_SYS_H
#define __CORTEX_RB_SYS_H
#include "SysType.h"


#if defined ( __CC_ARM   )
  #define __ASM            __asm                                      //!< asm keyword for ARM Compiler
  #define __INLINE         __inline                                   //!< inline keyword for ARM Compiler

#elif defined ( __ICCARM__ )
  #define __ASM           __asm                                       //!< asm keyword for IAR Compiler
  #define __INLINE        inline                                      //!< inline keyword for IAR Compiler. Only avaiable in High optimization mode!

#elif defined   (  __GNUC__  )
  #define __ASM            __asm                                      //!< asm keyword for GNU Compiler
  #define __INLINE         inline                                     //!< inline keyword for GNU Compiler

#elif defined   (  __TASKING__  )
  #define __ASM            __asm                                      //!< asm keyword for TASKING Compiler
  #define __INLINE         inline                                     //!< inline keyword for TASKING Compiler

#endif







//////////////////////////////////////////////////////////////////////////////////
// @brief  Enable Interrupt in NVIC Interrupt Controller
//
// @param  IRQn   The positive number of the external interrupt to enable
//
// Enable a device specific interupt in the NVIC interrupt controller.
// The interrupt number cannot be a negative value.
//////////////////////////////////////////////////////////////////////////////////
static __INLINE void NVIC_EnableIRQ(u32 IRQn)
{
  NVIC->ISER[((u32)(IRQn) >> 5)] = (1 << ((u32)(IRQn) & 0x1F)); /* enable interrupt */
}

//////////////////////////////////////////////////////////////////////////////////
// @brief  Disable the interrupt line for external interrupt specified
// 
// @param  IRQn   The positive number of the external interrupt to disable
// 
// Disable a device specific interupt in the NVIC interrupt controller.
// The interrupt number cannot be a negative value.
//////////////////////////////////////////////////////////////////////////////////
static __INLINE void NVIC_DisableIRQ(u32 IRQn)
{
  NVIC->ICER[((u32)(IRQn) >> 5)] = (1 << ((u32)(IRQn) & 0x1F)); /* disable interrupt */
}

#endif
