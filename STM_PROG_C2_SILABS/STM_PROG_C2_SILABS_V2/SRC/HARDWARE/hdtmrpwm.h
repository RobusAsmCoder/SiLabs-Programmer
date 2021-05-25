//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HDTMRPWM_H
#define __HDTMRPWM_H

#include "SysType.h"

#define  TIM_EGR_UG                          ((uint8_t)0x01)               /*!<Update Generation */
#define AyFreq        10000000
#define AyFreqCPU     hdTmr_FreqCPU

#define hdTmr_Freq    AyFreq
#define hdTmr_FreqCPU (u32)(hdTmr_Freq*100/100)

//static __forceinline u8 _name##_PWMcur(void)    { return (*(u32 *)(_name##wp_CNT)<*(u32 *)(_name##wp_PWM)) ? 1:0;                       }   \

#define TIM0        0
#define hdTmr_TIM1_Interrupt        TIM1_CC_IRQHandler  //TIM1_BRK_UP_TRG_COM_IRQHandler
#define hdTmr_TIM2_Interrupt        TIM2_IRQHandler
#define hdTmr_TIM3_Interrupt        TIM3_IRQHandler
#define hdTmr_TIM6_Interrupt        TIM6_DAC_IRQHandler
#define hdTmr_TIM14_Interrupt       TIM14_IRQHandler
#define hdTmr_TIM15_Interrupt       TIM15_IRQHandler
#define hdTmr_TIM16_Interrupt       TIM16_IRQHandler
#define hdTmr_TIM17_Interrupt       TIM17_IRQHandler

//#define hdTmr_TIM_Interrupt_Generate(_tnum)  void hdTmr_TIM##_tnum##_Interrupt(void) __irq
#define hdTmr_TIM_Interrupt_Generate(_tnum)  void TIM##_tnum##_IRQHandler(void) __irq
  


extern void hdTimerHardwareInterrupt(TIM_TypeDef *TIMx, u8 isOn);
extern u8 hdTimerSetup(TIM_TypeDef *TIMxM, TIM_TypeDef *TIMxS, u32 freq, u32 per, u8 enMask);

#define AYxPERupd(_name,_val) { const u32 val = _val;                                   \
                                u32 tmp = (*(vu32 *)(_name##wp_CNT));                   \
                                          (*( u16 *)(_name##wp_PSC)) = val;             \
                                          (*( u16 *)(_name##wp_EGR)) = TIM_EGR_UG;      \
                                          (*(vu32 *)(_name##wp_CNT)) = tmp; }
#define AYxPERupD(_name,_val)             (*( u16 *)(_name##wp_PSC)) = (_val)
#define AYxFRQupd(_name,_val)             (*( u16 *)(_name##wp_PSC)) = 1000000/(_val)
#define AYxPERcur(_name)      (*(u32 *)(_name##wp_PSC))
#define AYxCNTcur(_name)      (*(u32 *)(_name##wp_CNT))
#define AYxPWMcur(_name)      (*(u32 *)(_name##wp_PWM)!=0) ? ((*(u32 *)(_name##wp_CNT)<*(u32 *)(_name##wp_PWM)) ? 1:0) : 1
#define AYxCNMcur(_name,_mu)  ((AYxCNTcur(_name) * (_mu))/(*(u32 *)(_name##wp_PER)))
#define AYxPWMupd(_name,_val) (*(u32 *)(_name##wp_PWM))=(((*(u32 *)(_name##wp_PER))*(u8)(_val))>>8)

//TIM##_tims
//TIM##_timm

#define hdAYTIMPTR(_tims)     ((TIM_TypeDef *)(TIM##_tims))
                                          
#define hdAYGEX(_name, _timm, _tims, _chn, _div)                                                                                                                      \
enum { _name##wp_TIM          = (u32)(hdAYTIMPTR(_tims))               ,                                                                                                              \
       _name##wp_TIMM         = (u32)(hdAYTIMPTR(_timm))               ,                                                                                                              \
       _name##wp_CHN          = (u16)(_chn)                ,                                                                                                          \
       _name##wp_EGR          = (u32)(&hdAYTIMPTR(_tims)->EGR)         ,                                                                                              \
       _name##wp_PSC          = (u32)(&hdAYTIMPTR(_tims)->PSC)         ,                                                                                              \
       _name##wp_PER          = (u32)(&hdAYTIMPTR(_tims)->ARR)         ,                                                                                              \
       _name##wp_PWM          = (u32)(&hdAYTIMPTR(_tims)->CCR##_chn)   ,                                                                                              \
       _name##wp_CNT          = (u32)(&hdAYTIMPTR(_tims)->CNT)         ,                                                                                              \
  TIM##_tims##wp_num          = _tims                  ,                                                                                                              \
};                                                                                                                                                                    \
static __forceinline void _name##_InitC(u8 chnl)    { hdTimerSetup((TIM_TypeDef *)(_name##wp_TIMM), (TIM_TypeDef *)(_name##wp_TIM), hdTmr_FreqCPU/(_div), 1000, chnl); }  \
static __forceinline void _name##_Init(void)        { _name##_InitC(1<<((_chn)-1));                                                         }                         \
static __forceinline void hdTmr_TIM##_tims##Init(void)        { _name##_InitC(1<<((_chn)-1));                                                         }                         \
static __forceinline void _name##_IntCFG(u8 iOn)    { hdTimerHardwareInterrupt((TIM_TypeDef *)(_name##wp_TIM), iOn);                        }                         \
static __forceinline void _name##_PER(u16 _val)     { *(u32 *)(_name##wp_PSC)=(u32)(_val);                                                  }                         \
static __forceinline void _name##_PWM(u8 _val)      { *(u32 *)(_name##wp_PWM)=(u32)( (*(u32 *)(_name##wp_PER)*_val)>>8 );                   }                         \
static __forceinline u8 _name##_PWMcur(void)        { return AYxPWMcur(_name);                                                              }                         \
static __forceinline u16 _name##_CNTcur(void)       { return AYxCNTcur(_name);                                                              }                         \
static __forceinline u16 _name##_CNMcur(u16 d)      { return AYxCNMcur(_name, d);                                                           }                         \

#define hdAYGEN(_name, _tim, _chn)              hdAYGEX(_name, 0, _tim, _chn, TNE_DIVER)
#define hdAYGER(_name, _tim, _chn, _div)        hdAYGEX(_name, 0, _tim, _chn, _div)


////////////////////////////////////////////////////////////////////////////////
// AY_CLK -> TIM5CH2 or TIM2CH2 - input clock for AY
//
//  slave TIM      ITR0(TS=000)    ITR1(TS=001)    ITR2(TS=010)    ITR3(TS=011)
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

////////////////////////////////////////////////////////////////////////////////
// We have Timers ...
//   tim  pwm
//  TIM1  x4 - FOR MY TESTING I SEE ON THE PA9+PA10 !!!
//  TIM2  x4 - +OPORA
//  TIM3  x4 - ++
//  TIM4  x4 - ++
//  TIM5  x4 - ++

//  TIM8  x4 - ++

//  TIM9  x2 - +
//  TIM10 x1 - +
//  TIM11 x1 - +
//  TIM12 x2 - +
//  TIM13 x1 - +
//  TIM14 x1 - +

//  TIM6  x0 - for DAC !!!
//  TIM7  x0 - for DAC !!!
//

extern TIM_TypeDef *hdTimerTIM(u8 tnum);
extern u16 hdTimerRCC(TIM_TypeDef *TIMx);
extern u32 hdTimerFreq(TIM_TypeDef *TIMx);
extern u64 PWM_Timer_ConfigARR(u8 TIMNum, u8 CHNNum, u32 TimerMcs, u32 vARR);
extern u64 PWM_Timer_Config(u8 TIMNum, u8 CHNNum, u32 TimerMcs);
extern void hdTimerHardwareInit(void);


  #define TNE_DIVER       1

    //TIM1->ARR  = 200;   //PER
    //TIM1->CCR2 = 100;   //PWM CH2
  

//  hdAYGER(DelayTimer,         2 , 1,  8);
//  hdAYGER(DelayTimerDDD,      1 , 1,  8);

  hdAYGER(SoundTNE,         5 , 1, 10);     //10
  hdAYGER(SoundVOL,         2 , 2,  1);     //9




#endif
