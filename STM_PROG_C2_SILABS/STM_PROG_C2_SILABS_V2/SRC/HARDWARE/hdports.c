//////////////////////////////////////////////////////////////////////////////////////
// 
// Low level for STM32F100
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "hdports.h"

void hdGPIO_Active(GPIO_TypeDef* GPIOx)
{
    switch ((u32)GPIOx) 
    {
      //RCC_AHBPeriph_GPIOA
#if isDefinedSTM32F0X
      case GPIOA_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); break;
      case GPIOB_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); break;
      case GPIOC_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); break;
      case GPIOD_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE); break;
      case GPIOE_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE); break;
      case GPIOF_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE); break;
#elif isDefinedSTM32F1X
      case GPIOA_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); break;
      case GPIOB_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); break;
      case GPIOC_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); break;
      case GPIOD_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); break;
      case GPIOE_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); break;
      case GPIOF_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); break;
      case GPIOG_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE); break;
#elif isDefinedSTM32LXX
      case GPIOA_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); break;
      case GPIOB_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); break;
      case GPIOC_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); break;
      case GPIOD_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE); break;
      case GPIOE_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE); break;
      case GPIOF_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE); break;
      case GPIOG_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOG, ENABLE); break;
      case GPIOH_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOH, ENABLE); break;
#else
      //case GPIOA_BASE: RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); break;
#endif
    }
}
// IS_GPIO_MODE(_v) ? (((_v)+1)<< 0)
//IS_GPIO_OTYPE(_v) ? (((_v)+1)<< 4)
//IS_GPIO_SPEED(_v) ? (((_v)+1)<< 8)
// IS_GPIO_PUPD(_v) ? (((_v)+1)<<12)
u8 hdGPIO_SetGetMode(GPIO_InitTypeDef *inSt, u16 mode, u8 num)
{
      u8 res;
      res = (mode>>(4*num)) & 0xF;
      //if (res)
      {
#if isDefinedSTM32FXX
        const u8 md = res;
        switch(num)
        {
          case 0: inSt->GPIO_Mode  =  (GPIOMode_TypeDef)md; break;
          case 2: inSt->GPIO_Speed = (GPIOSpeed_TypeDef)md; break;
        }
#elif isDefinedSTM32LXX
        const u8 md = res;
        switch(num)
        {
          case 0: inSt->GPIO_Mode  =  (GPIOMode_TypeDef)md; break;
          case 2: inSt->GPIO_Speed = (GPIOSpeed_TypeDef)md; break;
          case 1: inSt->GPIO_OType = (GPIOOType_TypeDef)md; break;
          case 3: inSt->GPIO_PuPd  =  (GPIOPuPd_TypeDef)md; break;
        }
#else
          
          
#endif
        res = res | 0x80;
      }
      return res;
}

  

void hdGPIO_Init(GPIO_TypeDef* GPIOx, u8 pinpos, u16 mode, u8 isSet)// GPIOMode_TypeDef GPIO_Mode, GPIOSpeed_TypeDef GPIO_Speed, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd)
{
      #define mac_Step_md(_des,_type)             \
        _des=(_type)(mode&0xF);                   \
        mode>>=4
//  u8 md;
  
#if isDefinedSTM32FXX
  
  
  
  
  
#if isDefinedSTM32F0X
  //uu64(GPIOx->CRL) = (uu64(GPIOx->CRL) & ~((u64)(0xFULL<<(4*pinpos)))) | ((u64)tmpreg)<<(4*pinpos);
  u64 q;
  const u16 posA = (pinpos*2);
  const u16 posB = (pinpos*1+32);
  const u16 posC = (pinpos*2+32);
    
  q = (*(u64 *)(&GPIOx->MODER));
  (*(u64 *)(&GPIOx->MODER))   = ( q & ( (~(3<<posA)) & (~(1<<posB)) ) ) | (dHmPREXT(mode,0)<<posA) | ((u64)dHmPREXT(mode,3)<<posB);
  q = (*(u64 *)(&GPIOx->OSPEEDR));
  (*(u64 *)(&GPIOx->OSPEEDR)) = ( q & ( (~(3<<posA)) & (~(3<<posC)) ) ) | (dHmPREXT(mode,1)<<posA) | ((u64)dHmPREXT(mode,2)<<posC);
  GPIOMode_TypeDef  GPIO_Mode;      mac_Step_md(GPIO_Mode,  GPIOMode_TypeDef  );
#elif isDefinedSTM32F1X
  GPIOMode_TypeDef  GPIO_Mode;      mac_Step_md(GPIO_Mode,  GPIOMode_TypeDef  );
  GPIOSpeed_TypeDef GPIO_Speed;     mac_Step_md(GPIO_Speed, GPIOSpeed_TypeDef );
  u32 tmpreg = 0x00;
  tmpreg = (GPIO_Mode<<2) | (GPIO_Speed);
  uu64(GPIOx->CRL) = (uu64(GPIOx->CRL) & ~((u64)(0xFULL<<(4*pinpos)))) | ((u64)tmpreg)<<(4*pinpos);
#endif  
  if (GPIO_Mode==GPIO_Mode_OUT)
  {
    GPIO_PortWrite(GPIOx, pinpos, isSet?1:0);
  } else {
    GPIOx->ODR = (GPIOx->ODR & ~(1<<pinpos)) | ((mode&1)<<pinpos);
  }
  
#elif isDefinedSTM32LXX
  u32 tmpreg = 0x00;
  GPIOMode_TypeDef  GPIO_Mode;
  GPIOSpeed_TypeDef GPIO_Speed;
  GPIOOType_TypeDef GPIO_OType;
  GPIOPuPd_TypeDef  GPIO_PuPd;
  
  mac_Step_md(GPIO_Mode,  GPIOMode_TypeDef  );
  mac_Step_md(GPIO_OType, GPIOOType_TypeDef );
  mac_Step_md(GPIO_Speed, GPIOSpeed_TypeDef );
  mac_Step_md(GPIO_PuPd,  GPIOPuPd_TypeDef  );

      // Use temporary variable to update MODER register configuration, to avoid 
      // unexpected transition in the GPIO pin configuration.
      tmpreg = GPIOx->MODER;
      tmpreg &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
      tmpreg |= (((u32)GPIO_Mode) << (pinpos * 2));
      GPIOx->MODER = tmpreg;

      if ((GPIO_Mode == GPIO_Mode_OUT) || (GPIO_Mode == GPIO_Mode_AF))
      {
        // Use temporary variable to update OSPEEDR register configuration, to avoid 
        // unexpected transition in the GPIO pin configuration.
        tmpreg = GPIOx->OSPEEDR;
        tmpreg &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        tmpreg |= ((u32)(GPIO_Speed) << (pinpos * 2));
        GPIOx->OSPEEDR = tmpreg;

        // Use temporary variable to update OTYPER register configuration, to avoid 
        // unexpected transition in the GPIO pin configuration.
        tmpreg = GPIOx->OTYPER;
        tmpreg &= ~((GPIO_OTYPER_OT_0) << ((u16)pinpos));
        tmpreg |= (u16)(((u16)GPIO_OType) << ((u16)pinpos));
        GPIOx->OTYPER = tmpreg;
      }

      // Use temporary variable to update PUPDR register configuration, to avoid 
      // unexpected transition in the GPIO pin configuration.
      tmpreg = GPIOx->PUPDR;
      tmpreg &= ~(GPIO_PUPDR_PUPDR0 << ((u16)pinpos * 2));
      tmpreg |= (((u32)GPIO_PuPd) << (pinpos * 2));
      GPIOx->PUPDR = tmpreg;
      if (GPIO_Mode==GPIO_Mode_OUT)
      {
        GPIO_PortWrite(GPIOx, pinpos, isSet?1:0);
      }
#else
#endif
      //return GPIO_Mode;
}



void hdGPIO_Setup(GPIO_TypeDef* GPIOx, u8 pin, u16 mode, u8 def)
{
      hdGPIO_Active(GPIOx);
      hdGPIO_Init(GPIOx, pin, mode, def);
//      if (hdGPIO_Init(GPIOx, pin, mode, def)==GPIO_Mode_OUT)
//      {
//        GPIO_PortWrite(GPIOx, pin, def?1:0);
//      }
  
}



