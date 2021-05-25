//////////////////////////////////////////////////////////////////////////////////////
// 
// Low Level Timers For Project
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "hdtmrpwm.h"


//TIM_TypeDef* PWM_Timer_TnumToTIM(u8 TIMNum)
//{
//}

TIM_TypeDef *hdTimerTIM(u8 tnum)
{
        TIM_TypeDef* TIMx=0;
        switch (tnum)
        {
          #ifdef TIM1
          case 1: TIMx = TIM1; break;
          #endif
          #ifdef TIM2
          case 2: TIMx = TIM2; break;
          #endif
          #ifdef TIM3
          case 3: TIMx = TIM3; break;
          #endif
          #ifdef TIM4
          case 4: TIMx = TIM4; break;
          #endif
          #ifdef TIM5
          case 5: TIMx = TIM5; break;
          #endif
          #ifdef TIM6
          case 6: TIMx = TIM6; break;
          #endif
          #ifdef TIM7
          case 7: TIMx = TIM7; break;
          #endif
          #ifdef TIM8
          case 8: TIMx = TIM8; break;
          #endif
          #ifdef TIM9
          case 9: TIMx = TIM9; break;
          #endif
          #ifdef TIM10
          case 10: TIMx = TIM10; break;
          #endif
          #ifdef TIM11
          case 11: TIMx = TIM11; break;
          #endif
          #ifdef TIM12
          case 12: TIMx = TIM12; break;
          #endif
          #ifdef TIM13
          case 13: TIMx = TIM13; break;
          #endif
          #ifdef TIM14
          case 14: TIMx = TIM14; break;
          #endif
        }
        return TIMx;
}

//return 0 if error
u16 hdTimerRCC(TIM_TypeDef *TIMx)
{
        u16 r = 0;
        switch ((u32)TIMx)
        { default: break;
          #ifdef TIM1
          case (u32)TIM1 : RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE); r= 0x0401; break;
          #endif
          #ifdef TIM2
          case (u32)TIM2 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE); r= 0x0402;  break;
          #endif
          #ifdef TIM3
          case (u32)TIM3 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE); r= 0x0403; break;
          #endif
          #ifdef TIM4
          case (u32)TIM4 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE); r= 0x0404; break;
          #endif
          #ifdef TIM5
          case (u32)TIM5 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5 , ENABLE); r= 0x0405; break;
          #endif
          #ifdef TIM6
          case (u32)TIM6 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE); r= 0x0406; break;
          #endif
          #ifdef TIM7
          case (u32)TIM7 : RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE); r= 0x0407; break;
          #endif
          #ifdef TIM8
          case (u32)TIM8 : RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE); r= 0x0408; break;
          #endif
          #ifdef TIM9
          case (u32)TIM9 : RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9 , ENABLE); r= 0x0209; break;
          #endif
          #ifdef TIM10
          case (u32)TIM10: RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE); r= 0x010A; break;
          #endif
          #ifdef TIM11
          case (u32)TIM11: RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE); r= 0x010B; break;
          #endif
          #ifdef TIM12
          case (u32)TIM12: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE); r= 0x020C; break;
          #endif
          #ifdef TIM13
          case (u32)TIM13: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE); r= 0x010D; break;
          #endif
          #ifdef TIM14
          case (u32)TIM14: RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); r= 0x010E; break;
          #endif
        }
        return r;
}
#ifdef   TIM1
 #ifndef TIM1_BRK_UP_TRG_COM_IRQn
 #define TIM1_BRK_UP_TRG_COM_IRQn  TIM1_CC_IRQChannel
 #endif
#endif
#ifdef   TIM2
 #ifndef TIM2_IRQn
 #define TIM2_IRQn  TIM2_IRQChannel
 #endif
#endif
#ifdef   TIM3
 #ifndef TIM3_IRQn
 #define TIM3_IRQn  TIM3_IRQChannel
 #endif
#endif
#ifdef   TIM4
 #ifndef TIM4_IRQn
 #define TIM4_IRQn  TIM4_IRQChannel
 #endif
#endif
#ifdef   TIM5
 #ifndef TIM5_IRQn
 #define TIM5_IRQn  TIM5_IRQChannel
 #endif
#endif
#ifdef   TIM6
 #ifndef TIM6_IRQn
 #define TIM6_DAC_IRQn  TIM6_IRQChannel
 #endif
#endif
#ifdef   TIM7
 #ifndef TIM7_IRQn
 #define TIM7_IRQn  TIM7_IRQChannel
 #endif
#endif
#ifdef   TIM8
 #ifndef TIM8_IRQn
 #define TIM8_IRQn  TIM8_CC_IRQChannel
 #endif
#endif
#ifdef   TIM9
 #ifndef TIM9_IRQn
 #define TIM9_IRQn  TIM9_IRQChannel
 #endif
#endif
#ifdef   TIM10
 #ifndef TIM10_IRQn
 #define TIM10_IRQn  TIM10_IRQChannel
 #endif
#endif
#ifdef   TIM11
 #ifndef TIM11_IRQn
 #define TIM11_IRQn  TIM11_IRQChannel
 #endif
#endif
#ifdef   TIM12
 #ifndef TIM12_IRQn
 #define TIM12_IRQn  TIM12_IRQChannel
 #endif
#endif
#ifdef   TIM13
 #ifndef TIM13_IRQn
 #define TIM13_IRQn  TIM13_IRQChannel
 #endif
#endif
#ifdef   TIM14
 #ifndef TIM14_IRQn
 #define TIM14_IRQn  TIM14_IRQChannel
 #endif
#endif

u32 hdTimerIRQ(TIM_TypeDef *TIMx)
{
        u32 r = 0;
        switch ((u32)TIMx)
        { default: break;
          #ifdef TIM1
          case (u32)TIM1 : r=TIM1_BRK_UP_TRG_COM_IRQn; break;
          #endif
          #ifdef TIM2
          case (u32)TIM2 : r=TIM2_IRQn; break;
          #endif
          #ifdef TIM3
          case (u32)TIM3 : r=TIM3_IRQn; break;
          #endif
          #ifdef TIM4
          case (u32)TIM4 : r=TIM4_IRQn; break;
          #endif
          #ifdef TIM5
          case (u32)TIM5 : r=TIM5_IRQn; break;
          #endif
          #ifdef TIM6
          case (u32)TIM6 : r=TIM6_DAC_IRQn; break;
          #endif
          #ifdef TIM7_IRQn  //???error
          case (u32)TIM7 : r=TIM7_IRQn; break;
          #endif
          #ifdef TIM8
          case (u32)TIM8 : r=TIM8_IRQn; break;
          #endif
          #ifdef TIM9
          case (u32)TIM9 : r=TIM9_IRQn; break;
          #endif
          #ifdef TIM10
          case (u32)TIM10 : r=TIM10_IRQn; break;
          #endif
          #ifdef TIM11
          case (u32)TIM11 : r=TIM11_IRQn; break;
          #endif
          #ifdef TIM12
          case (u32)TIM12 : r=TIM12_IRQn; break;
          #endif
          #ifdef TIM13
          case (u32)TIM13 : r=TIM13_IRQn; break;
          #endif
          #ifdef TIM14
          case (u32)TIM14 : r=TIM14_IRQn; break;
          #endif
        }
        return r;
}


/*
u32 hdTimerFreq(TIM_TypeDef *TIMx)
{
        u32 r = hdTimerRCC(TIMx);
        RCC_ClocksTypeDef       RCC_Clocks;
        RCC_GetClocksFreq(&RCC_Clocks);
        switch( r & 0xF )
        {
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
          case 12:
          case 13:
          case 14:
            r = RCC_Clocks.PCLK1_Frequency*2;
            break;
          case 1:
          case 8:
          case 9:
          case 10:
          case 11:
            r = RCC_Clocks.PCLK2_Frequency*2;
            break;
          default:
            r=0;
            
        }
        return r;
}
*/

//Init Timer For Generator
u16 hdTimerSetupX(TIM_TypeDef *TIMx, u32 freq, u32 per, u8 enMask)  //u8 CHNNum,
{
  //TIM_OCInitTypeDef  TIM_OCInitStructure;
  //void(*TIM_OCxInit)(TIM_TypeDef*, TIM_OCInitTypeDef*);
        u16 rs=hdTimerRCC(TIMx);
        if (rs)
        {
          /*
          switch (CHNNum)
          { default:
              return 0;
            case 1: TIM_OCxInit=TIM_OC1Init; break;
            case 2: TIM_OCxInit=TIM_OC2Init; break;
            case 3: TIM_OCxInit=TIM_OC3Init; break;
            case 4: TIM_OCxInit=TIM_OC4Init; break;
          }
          */
          RCC_ClocksTypeDef       RCC_Clocks;
          u32                     presc;
          u32                     pwmper;
        
          RCC_GetClocksFreq(&RCC_Clocks);
          presc = (RCC_Clocks.HCLK_Frequency/freq);
          TIM_TimeBaseInitTypeDef TIM_BaseStruct;
          TIM_TimeBaseStructInit(&TIM_BaseStruct);

          TIM_BaseStruct.TIM_Prescaler = per-1;
          TIM_BaseStruct.TIM_Period = presc-1;//+2;// TIM_PeriodValue
          pwmper = presc;
          
          TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
          TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
          TIM_BaseStruct.TIM_RepetitionCounter = 0;
          TIM_TimeBaseInit(TIMx, &TIM_BaseStruct);

          //TIM_SelectInputTrigger(TIMx, TIM_TS_ITR1);
          //TIM_SelectSlaveMode(TIMx, TIM_SlaveMode_Trigger);
          TIM_SelectMasterSlaveMode(TIMx, TIM_MasterSlaveMode_Disable);

  /////////////////////////////////

          TIM_OCInitTypeDef TIM_OCStruct;
          // PWM mode 2 = Clear on compare match
          // PWM mode 1 = Set on compare match
          TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;//TIM_OCMode_PWM1;
          TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
          TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
          
          
          TIM_OCStruct.TIM_Pulse = (pwmper*50)/100; // 50%
          switch ((rs>>8) & 0x0F)
          {
            case 4:
              if (enMask & (1<<3)){ TIM_OC4Init(TIMx, &TIM_OCStruct);
              TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);}
            case 3:
              if (enMask & (1<<2)){ TIM_OC3Init(TIMx, &TIM_OCStruct);
              TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);}
            case 2:
              if (enMask & (1<<1)){ TIM_OC2Init(TIMx, &TIM_OCStruct);
              TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);}
            case 1:
              if (enMask & (1<<0)){ TIM_OC1Init(TIMx, &TIM_OCStruct);
              TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);}
            default:
              break;
          }

/////////////////////////////////

            TIM_BDTRInitTypeDef timer_bdtr;
            TIM_BDTRStructInit(&timer_bdtr);
            timer_bdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
            TIM_BDTRConfig(TIMx, &timer_bdtr);
          // Start count on TIMx
            TIM_Cmd(TIMx, ENABLE);
          }
          return rs;
}



//    TIM1          TIM5_TRGO       TIM2_TRGO       TIM3_TRGO       TIM4_TRGO
//    TIM2          TIM1_TRGO       TIM8_TRGO       TIM3_TRGO       TIM4_TRGO
//    TIM3          TIM1_TRGO       TIM2_TRGO       TIM5_TRGO       TIM4_TRGO
//    TIM4          TIM1_TRGO       TIM2_TRGO       TIM3_TRGO       TIM8_TRGO
//    TIM5          TIM2_TRGO       TIM3_TRGO       TIM4_TRGO       TIM8_TRGO
//    TIM6             ---             ---             ---             ---
//    TIM7             ---             ---             ---             ---
//    TIM8          TIM1_TRGO       TIM2_TRGO       TIM4_TRGO       TIM5_TRGO
//    TIM9          TIM2_TRGO       TIM3_TRGO       TIM10_OC        TIM11_OC
//    TIM10            ---             ---             ---             ---
//    TIM11            ---             ---             ---             ---
//    TIM12         TIM4_TRGO       TIM5_TRGO       TIM13_OC        TIM14_OC
//    TIM13            ---             ---             ---             ---
//    TIM14            ---             ---             ---             ---

u8 hdTimerSetup(TIM_TypeDef *TIMxM, TIM_TypeDef *TIMxS, u32 freq, u32 per, u8 enMask)
{
    const u16 tmlTS[16]={
      0x0000,
      0x5234,
      0x1834,
      0x1254,
      0x1238,
      0x2348,
      0x0000,
      0x0000,
      0x1245,
      0x23AB,
      0x0000,
      0x0000,
      0x45DE,
      0x0000,
      0x0000,
      0x0000,
    };
      
    u8 rs=0;
    if (TIMxM==0)
    {
      rs = hdTimerSetupX(TIMxS, freq, per, enMask);
    } else {
      u8 rm = hdTimerSetupX(TIMxM, freq*2, 5*0+0x34+1, 0x01);
         rs = hdTimerSetupX(TIMxS, 100000,   1, 0x01);
      u16 w = tmlTS[rs];
      for (u8 n=0; n<4; n++)
      {
        u8 b=(w>>(12-n*4)) & 0x0F;
        if (b==rm)
        {
          TIM_Cmd(TIMxS, DISABLE);
        
          TIM_SelectOutputTrigger(TIMxM, TIM_TRGOSource_OC1);
          TIM_SelectMasterSlaveMode(TIMxM, TIM_MasterSlaveMode_Enable);
          switch (n) 
          {
            case 0: TIM_SelectInputTrigger(TIMxS, TIM_TS_ITR0); break;
            case 1: TIM_SelectInputTrigger(TIMxS, TIM_TS_ITR1); break;
            case 2: TIM_SelectInputTrigger(TIMxS, TIM_TS_ITR2); break;
            case 3: TIM_SelectInputTrigger(TIMxS, TIM_TS_ITR3); break;
          }
          TIM_SelectSlaveMode(TIMxS, TIM_SlaveMode_Gated);
          
          TIMxS->CNT = 0x0000;
          TIMxS->ARR = 0xFFFF;
          TIMxS->CCR2 = per;
          
          TIM_Cmd(TIMxS, ENABLE);
          break;
        }
      }
      
    }
    return rs;
}


//OUT:MCS Real Step
u64 PWM_Timer_ConfigARR(u8 TIMNum, u8 CHNNum, u32 TimerMcs, u32 vARR)
{
  RCC_ClocksTypeDef       RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  if (vARR==0) vARR++;
  double pd;
  u32 presc;
  pd = vARR*1000000UL;
  pd/= RCC_Clocks.SYSCLK_Frequency/vARR;
  //presc = ((TimerMcs*vARR) / (vARR*1000000UL / (RCC_Clocks.SYSCLK_Frequency/vARR)));
  presc = ((TimerMcs*vARR) / pd);
  if (presc==0) presc++;
  
//  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TypeDef* TIMx;
  void(*TIM_OCxInit)(TIM_TypeDef*, TIM_OCInitTypeDef*);
  TIMx = hdTimerTIM(TIMNum);
  if (TIMx)
  {
    hdTimerRCC(TIMx);
    switch (CHNNum)
    { default:
      case 1: TIM_OCxInit=TIM_OC1Init; break;
      case 2: TIM_OCxInit=TIM_OC2Init; break;
      case 3: TIM_OCxInit=TIM_OC3Init; break;
      case 4: TIM_OCxInit=TIM_OC4Init; break;
    }
    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = vARR-1;                           //ARR
    TIM_TimeBaseStructure.TIM_Prescaler = presc-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); 
    if (CHNNum)
    {
    //  TIM_OCStructInit(&TIM_OCInitStructure);
      TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;    
      TIM_OCInitStructure.TIM_Pulse=0;                                   //CCR1
      TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;            //Negative = Low
      TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;
      TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;
      TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
      TIM_OCxInit(TIMx, &TIM_OCInitStructure);
      TIM_CtrlPWMOutputs(TIMx, ENABLE);
    }
    TIM_Cmd(TIMx, ENABLE);
  }
  double db;
  u64 q;
  //db = 1000000.0f/RCC_Clocks.SYSCLK_Frequency;
  //db*= presc*256.0f;
  
  db = RCC_Clocks.SYSCLK_Frequency / presc;
  db/= vARR;
  db = 1000000.f / db;
  q = db * 65536.0f * 65536.0f;

/*  
  q = presc;  //(1000000ULL*presc*65536ULL*65536ULL);
  q*= 1000000ULL;
  q*= 65536ULL;
  q*= 65536ULL;
  q/= RCC_Clocks.SYSCLK_Frequency;
  q*= 255ULL;
*/  
  return q;
    
}

u64 PWM_Timer_Config(u8 TIMNum, u8 CHNNum, u32 TimerMcs)
{
    return PWM_Timer_ConfigARR(TIMNum, CHNNum, TimerMcs, 256);
}

void hdTimerHardwareInterrupt(TIM_TypeDef *TIMx, u8 isOn)
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = hdTimerIRQ(TIMx);
#if isDefinedSTM32F0X
    nvicStructure.NVIC_IRQChannelPriority = 0;
#else
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 0;
#endif  
    nvicStructure.NVIC_IRQChannelCmd = isOn ? ENABLE : DISABLE;
    NVIC_Init(&nvicStructure);
    //TIM_ITConfig(TIMx, TIM_IT_Update, isOn ? ENABLE : DISABLE);
    TIM_ITConfig(TIMx, TIM_IT_CC1, isOn ? ENABLE : DISABLE);
  
}




void hdTimerHardwareInit(void)
{
//      DelayTimer_InitC(0);
}



//#ifdef hdTmr_TIM2Init
//#ifdef DelayTimer_InitC


