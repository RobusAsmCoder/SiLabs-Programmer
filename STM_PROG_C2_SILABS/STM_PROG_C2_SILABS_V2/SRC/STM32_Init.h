//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __STM32_INIT_H
#define __STM32_INIT_H

//#include <stm32l1xx.h>                        // STM32F10x Library Definitions
#include "STM32_Reg.h"                            // missing bit definitions
//#include "stm32l1xx_type.h"


#ifndef TIM2_IRQChannel 
 #define TIM2_IRQChannel  TIM2_IRQn
#endif 

#ifndef TIM3_IRQChannel 
 #define TIM3_IRQChannel  TIM3_IRQn
#endif 

#ifndef TIM4_IRQChannel 
 #define TIM4_IRQChannel  TIM4_IRQn
#endif 

extern void stm32_Init(void);
extern unsigned int stm32_TimerGetReload (char tnum);
extern unsigned int stm32_GetPCLK1 (void);
extern unsigned int stm32_GetPCLK2 (void);


#endif
