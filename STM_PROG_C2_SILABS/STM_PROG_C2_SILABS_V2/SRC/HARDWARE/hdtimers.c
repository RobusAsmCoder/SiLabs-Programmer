//////////////////////////////////////////////////////////////////////////////////////
// 
// Low Level Timers For Project
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////
#include "SysType.h"
#include "cortex_rb_sys.h"
#include "cortex_rb_dwt.h"          
#include "hdtimers.h"
#if (CoCOS_EN==1)
#include "coocox.h"
#endif


//vu32 PCLK1;
//vu32 PCLK2;
vu16 PCLK1_NOP;
//vu32 TIMER0_PER;
//vu32 TIMER1_PER;
//vu32 TIMER2_PER;
//vu32 TIMER3_PER;
//vu32 TIMER4_PER;

RCC_ClocksTypeDef MPU_ClocksStatus;


///////////////////////////
// Timer Procedures
///////////////////////////

#if WaitMsTimerNum==0
  u32 CortexTimerDeltaProcDO(u32 *cnt, u32 *tmr, const u32 syst)
  {
      const u32 systick = syst;
      const u32 delt=(vu32)(DWT->CYCCNT - *cnt);
      DWT->CTRL |= (u32)1;
      CoreDebug->DEMCR |= (u32)0x01000000UL;
      if (delt >= (systick))
      {
        const u32 dtime=delt / (u32)(systick);
        *cnt += ((u32)(dtime)) * (u32)(systick);
        *tmr += ((u32)(dtime));
      }
      return *tmr;
  }
  #define CortexTimerSysTick()    MPU_CluckTiks()
#else
  u64 WaitMcsTimerCounter = 0;
  u64 WaitMcsTimerStep = 0;
  u32 CortexTimerDeltaProcDO(u32 *cnt, u32 *tmr, const u32 syst)
  {
      const u32 systick = syst;
      const u32 delt=(((vu32 *)(&WaitMcsTimerCounter))[1] - *cnt);
      if (delt)
      {
        const u32 dtime=delt / (u32)(systick);
        *cnt += ((u32)(dtime)) * (u32)(systick);
        *tmr += ((u32)(dtime));
      }
      return *tmr;
  }
  #define CortexTimerSysTick()      1000000
#endif


#define CortexTimerDeltaProcedure(_timer, _systicks)                    \
do{ static u32 ccnt=0;                                                  \
    CortexTimerDeltaProcDO((u32 *)&ccnt, (u32 *)&_timer, _systicks);    \
  }while(0)

/*
#define CortexTimerDeltaProcedure(_timer, _systicks)      \
do{ static u32 ccnt=0;                                    \
    const u32 systick = _systicks;                        \
    const u32 delt=(vu32)(DWT->CYCCNT - ccnt);            \
    DWT->CTRL |= (vu32)1;                                 \
    CoreDebug->DEMCR |= (vu32)0x01000000;                 \
    if (delt >= (systick))                                \
    {                                                     \
      const u32 dtime=delt / (u32)(systick);              \
      ccnt += ((u32)(dtime)) * (u32)(systick);            \
      _timer += ((u32)(dtime));                           \
    }                                                     \
  }while(0)
*/

//#define isTimerCompareOverflow(_valTimerDelay, _valTimerCounter)    ( ((((u32)(_valTimerDelay)) - ((u32)(_valTimerCounter))) >>29) )

vu32 Timer_CounterX=0;
u32 Timer_CounterProc(void)
{
    return TimerCounterUpdate();//Timer_Counter;
}

vu32 TimerCounterUpdate(void)
{
    CortexTimerDeltaProcedure(Timer_CounterX, CortexTimerSysTick()/1000000); //PCLK1_NOP);
    return Timer_CounterX;
}


/*
u32 isTimerCompareOverflow(vu32 valTimerDelay, vu32 valTimerCounter)
{
  vu32 st=1;
  if (valTimerDelay)
  {
    st = ((((vu32)(valTimerDelay)) - ((vu32)(valTimerCounter))) >>30);
  }
  //if (st) st=1;
  return st;
}
*/
u8 isTimerCompareOverflowX(u32 *vpD, u32 *vpC)
{
  u8 st=1;
  if (vpD[0])
  {
    if ( (u32)((vpD[0] - vpC[0]) & 0xE0000000)!=0 )
    {
      st = 1;
    } else {
      st = 0;
    }
  } else {
    st = 1;
  }
  //if (st) st=1;
  return st;
}

#pragma push
#pragma O3
#pragma Ospace  //Otime
u32 isTimerCompareOverflow(vu32 valTimerDelay, vu32 valTimerCounter)
{
    return isTimerCompareOverflowX((u32 *)&valTimerDelay, (u32 *)&valTimerCounter);
}
#pragma pop


void SysTick_Interrupt(u32 sysTimeAdder)
{
}

/*
void SysTick_Interrupt(u32 sysTimeAdder)
{
        //////Timer_Counter+=sysTimeAdder;//TIMER0_PER;
//        Timer_Counter+=TIMER0_PER;
        //ExtBuf6Vns_Interrupt();
}

//#ifndef CoCOS_EN
//#endif
//#define CoCOS_EN  0
*/
//#if (CoCOS_EN!=1)
//void SysTick_Handler(void) __irq
//{
//      SysTick_Interrupt(TIMER0_PER);
//}
//#endif



void DelayMCS(u32 mcs)
{
  u32 w_timc;
  u32 w_tmrcur;
        w_timc = /*Timer_Counter*/TimerCounterUpdate() + mcs;
        while (1)
        {
          w_tmrcur = /*Timer_Counter*/TimerCounterUpdate();
          if (!isTimerCompareOverflow(w_timc, w_tmrcur))
          {
            __nop();//while (w_timc>Timer_Counter) __nop();
          } else {
            break;
          }
        }
}

void DelayMS(u32 ms)
{
    u32 tmr=0;
    while (ms--)
    {
      while(!WaitMCS_CheckContinue(&tmr,1000));
    }
    //    while (ms--) DelayMCS(1000);
}

void DelayNOP(u32 co)
{
        while (co--) __nop();
}

void DelayNOP_CLK(void)
{
        DelayNOP(PCLK1_NOP);
}
//Timer 1 ms
vu32 Timer_1mcs_CounterX=0;
vu32 Timer_1ms_CounterX=0;
vu32 Timer_1ms_Store[8];


u32 Timer_1mcs_CounterUpdate(void)
{
    #if UseTimerRTC==1
      RtcTimerDeltaProcedure(Timer_1ms_CounterX,SystemLFXOClockGet()/1000000);
    #else
      CortexTimerDeltaProcedure(Timer_1mcs_CounterX, CortexTimerSysTick()/1000000);  //PCLK1_NOP*1000);
    #endif
    return Timer_1mcs_CounterX;
}

u32 Timer_1ms_CounterUpdate(void)
{
    #if UseTimerRTC==1
      RtcTimerDeltaProcedure(Timer_1ms_CounterX,SystemLFXOClockGet()/1000);
    #else
      CortexTimerDeltaProcedure(Timer_1ms_CounterX, CortexTimerSysTick() / 1000);  //PCLK1_NOP*1000);
    #endif
    return Timer_1ms_CounterX;
  
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

s32 WaitMS_Delta(u32 *p)
{
    s32 delta = (vs32)(p[0] - Timer_1ms_Counter);
    return delta;
}

void WaitMS_Start(u32 *p, u32 ms)
{
    p[0]=Timer_1ms_Counter+ms;
}

//return 1 if timer overflow started value
u8 WaitMS_Check(u32 *p)
{
    return isTimerCompareOverflow(p[0], Timer_1ms_Counter);//return p[0]<Timer_1ms_Counter;
}

void WaitMS_Wait(u32 *p)
{
   while (!WaitMS_Check(p));
}

//return number of timer overflow started value
u32 WaitMS_CheckContinue(u32 *p, u32 ms)
{
    u32 res = 0;
    if (p[0])
    {
      s32 del=WaitMS_Delta(p);
      while (del<0)
      {
        p[0]+=ms;
        del+=ms;
        res++;
      }
    } else {
      WaitMS_Start(p, ms);
    }
    return res;
}

/////////////////////

s32 WaitMCS_Delta(u32 *p)
{
    s32 delta = (vs32)(p[0] - Timer_1mcs_CounterUpdate());
    return delta;
}

void WaitMCS_Start(u32 *p, u32 mcs)
{
    p[0]=Timer_1mcs_CounterUpdate()+mcs;
}

//return 1 if timer overflow started value
u8 WaitMCS_Check(u32 *p)
{
    return isTimerCompareOverflow(p[0], Timer_1mcs_CounterUpdate());
}

void WaitMCS_Wait(u32 *p)
{
   while (!WaitMCS_Check(p));
}

//return number of timer overflow started value
u32 WaitMCS_CheckContinue(u32 *p, u32 ms)
{
    u32 res = 0;
    if (p[0])
    {
      s32 del=WaitMCS_Delta(p);
      while (del<0)
      {
        p[0]+=ms;
        del+=ms;
        res++;
      }
    } else {
      WaitMCS_Start(p, ms);
    }
    return res;
}



void Sleep(u32 ms)
{
  if (ms)
  {
    u32 p;
    WaitMS_Start(&p, ms);
    while (!WaitMS_Check(&p))
    {
#if (CoCOS_EN==1)
      CoTickDelay(1);
#endif
    }
  } else {
#if (CoCOS_EN==1)
      CoTickDelay(1);
#endif
  }
  
}




//#include "hardware.h"




#if WaitMsTimerNum
void TIM3_IRQHandler(void) __irq
{
//    static u8 v=0;
//    VK_2(v);
//    v^=1;
    WaitMcsTimerCounter+=WaitMcsTimerStep;
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update | TIM_IT_CC1);
}
#endif


void WaitHdTimerInit(void)
{
#if WaitMsTimerNum
    //DelayTimer_InitC(0);
    //AYxFRQupd(DelayTimer, 1000000/WaitMsTimerPerriodMCS);
    WaitMcsTimerStep = PWM_Timer_ConfigARR(WaitMsTimerNum, 1, WaitMsTimerPerriodMCS, 4);
    DelayTimer_IntCFG(1);
#endif
  
    
}



