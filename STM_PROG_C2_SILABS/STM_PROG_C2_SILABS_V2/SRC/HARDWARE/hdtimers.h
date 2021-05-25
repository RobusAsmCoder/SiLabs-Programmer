//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HDTIMERS_H
#define __HDTIMERS_H


#include "SysType.h"
#include "hdtmrpwm.h"
#include "hdports.h"

//#include "stm32f0xx_rcc.h"
//#include "stm32l0xx_ll_rcc.h"
//#define RCC_ClocksTypeDef       LL_RCC_ClocksTypeDef
//#define RCC_GetClocksFreq       LL_RCC_GetSystemClocksFreq


//////////////////////////////////////////////////////////////////////////////////////


extern u32 isTimerCompareOverflow(u32 valTimerDelay, u32 valTimerCounter );


//#define TMS_StartWait(__npos,__nval)   Timer_1ms_Store[__npos]=Timer_1ms_Counter+(__nval)
//#define TMS_CheckWait(__npos)          Timer_1ms_Store[__npos]<=Timer_1ms_Counter

//#define TMS_1msStartWait(__name,__nval)   __name=Timer_1ms_Counter+(__nval)
//#define TMS_1msCheckWait(__name)          __name<=Timer_1ms_Counter
//#define TMS_1msGetProcessTime(__name)     Timer_1ms_Counter-__name

//#define TMS_StoreStartWait(__num,__nval)   TMS_1msStartWait(Timer_1ms_Store[__num],__nval)
//#define TMS_StoreCheckWait(__num)          TMS_1msCheckWait(Timer_1ms_Store[__num])
//#define TMS_StoreGetProcessTime(__num)     TMS_1msGetProcessTime(Timer_1ms_Store[__num])


#define MPU_CluckTiks()     MPU_ClocksStatus.SYSCLK_Frequency   //for STM32F4xxx
//#define MPU_CluckTiks()     MPU_ClocksStatus.PCLK1_Frequency      //for STM32F1xxx


typedef u32(timerGetProc_t)(void);


extern u32 Timer_CounterProc(void);
extern vu32 TimerCounterUpdate(void);

extern u32 Timer_1ms_CounterUpdate(void);
#define Timer_1ms_Counter       Timer_1ms_CounterUpdate()     //extern vu32 Timer_1ms_Counter;
extern vu32 Timer_1ms_Store[];
extern u32 Timer_1ms_CounterProc(void);




extern RCC_ClocksTypeDef MPU_ClocksStatus;
//extern vu32 PCLK1;
//extern vu32 PCLK2;
extern vu16 PCLK1_NOP;
extern vu32 TIMER0_PER;
extern vu32 TIMER1_PER;
extern vu32 TIMER2_PER;
extern vu32 TIMER3_PER;
extern vu32 TIMER4_PER;



// Global define
extern void DelayMCS(u32 mcs);
extern void DelayMS(u32 ms);
extern void DelayNOP(u32 co);
extern void DelayNOP_CLK(void);

#define WaitMS_GetTime()    Timer_1ms_Counter

extern s32 WaitMS_Delta(u32 *p);
extern void WaitMS_Start(u32 *p, u32 ms);
extern u8 WaitMS_Check(u32 *p);
extern void WaitMS_Wait(u32 *p);
extern u32 WaitMS_CheckContinue(u32 *p, u32 ms);

extern s32 WaitMCS_Delta(u32 *p);
extern void WaitMCS_Start(u32 *p, u32 mcs);
extern u8 WaitMCS_Check(u32 *p);
extern void WaitMCS_Wait(u32 *p);
extern u32 WaitMCS_CheckContinue(u32 *p, u32 ms);

extern u32 Timer_1mcs_CounterUpdate(void);
#define Timer_1mcs_Counter       Timer_1mcs_CounterUpdate()


extern void Sleep(u32 ms);

extern void SysTick_Interrupt(u32 sysTimeAdder);
extern void WaitHdTimerInit(void);

#define WaitMsTimerNum            0
#define WaitMsTimerPerriodMCS     100

  hdAYGER(DelayTimer,         WaitMsTimerNum , 1,  (hdTmr_Freq/1000000));


#endif
