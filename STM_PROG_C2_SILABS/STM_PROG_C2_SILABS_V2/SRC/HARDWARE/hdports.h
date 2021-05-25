//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HDPORTS_H
#define __HDPORTS_H

#include "SysType.h"

#define isDefinedSTM32F1X    ( defined (STM32F10X_LD)       ||  \
                               defined (STM32F10X_LD_VL)    ||  \
                               defined (STM32F10X_MD)       ||  \
                               defined (STM32F10X_MD_VL)    ||  \
                               defined (STM32F10X_HD)       ||  \
                               defined (STM32F10X_HD_VL)    ||  \
                               defined (STM32F10X_XL)       ||  \
                               defined (STM32F10X_CL)       ||  \
                               0)
//STM32F0XX, STM32F0XX_MD, 
#define isDefinedSTM32F0X    ( defined (STM32F0XX_MD)       ||  \
                               defined (STM32F051)       ||  \
                               0)

#define isDefinedSTM32FXX    ( isDefinedSTM32F0X || isDefinedSTM32F1X )

#define isDefinedSTM32LXX    ( defined (STM32L1XX_MD)       ||  \
                               defined (STM32L1XX_MDP)      ||  \
                               defined (STM32L1XX_HD)       ||  \
                               defined (STM32L1XX_XL) )

#if isDefinedSTM32F0X
  #include "stm32f0xx_rcc.h"
  #include "stm32f0xx_gpio.h"
#elif isDefinedSTM32F1X
  #include "stm32f10x_rcc.h"
  #include "stm32f10x_gpio.h"
#elif isDefinedSTM32LXX
 #include "stm32l1xx_rcc.h"
 #include "stm32l1xx_gpio.h"
#else
 #include "stm32l0xx_ll_rcc.h"
 #include "stm32l0xx_ll_gpio.h"
 #define GPIO_InitTypeDef     LL_GPIO_InitTypeDef 
#endif


#define dhportF1XX_MODE_OPP     0x03ULL
#define dhportF1XX_MODE_INP     0x08ULL
#define dhportF1XX_MODE_INO     0x04ULL

#define dH_BITV(_p_, _v_)     ((u32)(_v_)<<(_p_))
#define dH_BIT(_p_)           dH_BITV((_p_), 1)
#define dH_PORT_BSSR(_p_, _v_)  dH_BITV(_p_, _v_) | dH_BIT((_p_) | 16)

#define dH_PORT_BSSR_R(_p_)  dH_BIT((_p_) | 16)
#define dH_PORT_BSSR_S(_p_)  dH_BIT((_p_))
#define dH_PORT_CFG_MD(_name_, _p_, _v_)  _name_ = (_name_ & (~( 0xF<<(4*(_p_) ))) ) | ((u32)(_v_)<<(4*(_p_)))

#define hdPin_O_DefineSetup(_name, _port, _pin, _mode, _def)                                                                                                  \
hdPin_Define(_name, _port, _pin, _mode);                                                                                                                      \
static __forceinline void GpioInit_##_name(void) { hdGPIO_Setup((GPIO_TypeDef *)_name##wp_port, _name##wp_pin, _name##wp_mode, _def); }                       \

#define hdPin_Define(_name, _port, _pin, _mode)                                                                                             \
enum { _name##wp_port         = (u32)(GPIO##_port)        ,                                                                                 \
       _name##wp_pin          = (u32)(_pin)               ,                                                                                 \
       _name##wp_bit          = (s32)(dH_BIT(_pin))       ,                                                                                 \
       _name##wp_mode         = (u16)(_mode)              ,                                                                                 \
}

#ifdef isDefinedSTM32FXX
  #define GPIO_PortWrite(_port,_pin,_v)     uu32(((GPIO_TypeDef *)(void *)_port)->BSRR) = dH_PORT_BSSR((_pin),(_v))
#else
  #define GPIO_PortWrite(_port,_pin,_v)     uu32(((GPIO_TypeDef *)(void *)_port)->BSRRL) = dH_PORT_BSSR((_pin),(_v))
#endif  
#define dH_PORT_Write(_name,_v)           GPIO_PortWrite(_name##wp_port, (_name##wp_pin), (_v))
#define dH_ODR_REG(_name)                 ((GPIO_TypeDef *)(void *)_name##wp_port)->ODR
#define dH_nam_SET(_name)                 ( (_name##wp_bit))
#define dH_nam_RES(_name)                 (~(_name##wp_bit))
#define dH_ODR_Write(_name,_v)            dH_ODR_REG(_name) = dH_ODR_REG(_name) & dH_nam_RES(_name) | ((_v)?dH_nam_SET(_name):0)  //if (_v) ((GPIO_TypeDef *)(void *)_name##wp_port)->ODR |= _name##wp_bit; else ((GPIO_TypeDef *)(void *)_name##wp_port)->ODR &= ~(_name##wp_bit)

  #define dH_MODEIO_Set(_name,_m)           uu64(((GPIO_TypeDef *)(void *)_name##wp_port)->CRL) = (uu64(((GPIO_TypeDef *)(void *)_name##wp_port)->CRL) & (~(0x0FULL<<(_name##wp_pin*4))) ) | (((_m))<<(_name##wp_pin*4))
  #define dH_MODEIO_Write(_name,_v)         dH_MODEIO_Set(_name,(_v)??dhportF1XX_MODE_INP:dhportF1XX_MODE_OPP)  //uu64(((GPIO_TypeDef *)(void *)_name##wp_port)->CRL) = (uu64(((GPIO_TypeDef *)(void *)_name##wp_port)->CRL) & (~(0x0FULL<<(_name##wp_pin*4))) ) | (((_v)?dhportF1XX_MODE_INP:dhportF1XX_MODE_OPP)<<(_name##wp_pin*4))
  
//if (_v) ((GPIO_TypeDef *)(void *)_name##wp_port)->ODR |= _name##wp_bit; else ((GPIO_TypeDef *)(void *)_name##wp_port)->ODR &= ~(_name##wp_bit)
//dhportF1XX_MODE_OPP
  
#define GPIO_PortRead(_port,_pin)         ((uu16(((GPIO_TypeDef *)(void *)_port)->IDR)>>(_pin))&1)
#define dH_PORT_Read(_name)               GPIO_PortRead(_name##wp_port, (_name##wp_pin))

#define dHmPRTN(_n, _o, _v)       dHmPRT_##_n = (s32)((u16)(_v))<<((_o)*4)
#define dHmPRT(_o, _v)            dHmPRTN(_v, _o, _v)
#define dHmPREXT(_v, _o)          (((_v)>>((_o)*4)) & 15)

#if isDefinedSTM32F1X
  enum{
    GPIO_OType_PP     = 0,
    GPIO_OType_OD     = 1,
    
    GPIO_Mode_AN      = 0,
    GPIO_Mode_IN      = 2,
    GPIO_Mode_OUT     = 0,
    GPIO_Mode_AF      = 2,
    
    GPIO_PuPd_NOPULL  = 0,
    GPIO_PuPd_UP      = 3,
    GPIO_PuPd_DOWN    = 2,
  };
#endif


#if isDefinedSTM32FXX
  enum{
    GPIO_SpeeD_2MHz  = GPIO_Speed_2MHz,
    GPIO_SpeeD_10MHz = GPIO_Speed_10MHz,
    GPIO_SpeeD_50MHz = GPIO_Speed_50MHz,
    
    dHmPRT(0, GPIO_Mode_IN),
    dHmPRT(0, GPIO_Mode_OUT),     //+speed mhz
    dHmPRT(0, GPIO_Mode_AF),      //+speed mhz
    dHmPRT(0, GPIO_Mode_AN),
  //CNF0
#if isDefinedSTM32F0X
    dHmPRT(3, GPIO_OType_PP),
    dHmPRT(3, GPIO_OType_OD),
#else
    dHmPRT(0, GPIO_OType_PP),
    dHmPRT(0, GPIO_OType_OD),
#endif
    
    dHmPRT(1, GPIO_SpeeD_2MHz),
    dHmPRT(1, GPIO_SpeeD_10MHz),
    dHmPRT(1, GPIO_SpeeD_50MHz),
    
    dHmPRT(2, GPIO_PuPd_NOPULL),
    dHmPRT(2, GPIO_PuPd_UP),
    dHmPRT(2, GPIO_PuPd_DOWN),
    
    //dHmPRT_GPIO_Speed_50MHz = 0,
           
  };
  enum {
    hd_gptp_IN_OD     = dHmPRT_GPIO_Mode_AN  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL                               ,
    hd_gptp_IN_PPU    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP                                   ,
    hd_gptp_IN_PPD    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN                                 ,
    hd_gptp_OUT_OD    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL   | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_OUT_PP    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP                             | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_OUT_PPU   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP       | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_OUT_PPD   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN     | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_AF_OD     = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL   | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_AF_PPU    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP       | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_AF_PPD    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN     | dHmPRT_GPIO_SpeeD_50MHz   ,
    hd_gptp_FFFFFFFF  = (u16)0xFFFF,
  };
#elif isDefinedSTM32LXX
  enum{
    dHmPRT(0, GPIO_Mode_IN),
    dHmPRT(0, GPIO_Mode_OUT),
    dHmPRT(0, GPIO_Mode_AF),
    dHmPRT(0, GPIO_Mode_AN),

    dHmPRT(1, GPIO_OType_PP),
    dHmPRT(1, GPIO_OType_OD),

    dHmPRT(2, GPIO_Speed_400KHz),
    dHmPRT(2, GPIO_Speed_2MHz),
    dHmPRT(2, GPIO_Speed_10MHz),
    dHmPRT(2, GPIO_Speed_40MHz),

    dHmPRT(3, GPIO_PuPd_NOPULL),
    dHmPRT(3, GPIO_PuPd_UP),
    dHmPRT(3, GPIO_PuPd_DOWN),
  };
  enum {
    hd_gptp_IN_OD     = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL                               ,
    hd_gptp_IN_PPU    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP                                   ,
    hd_gptp_IN_PPD    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN                                 ,
    hd_gptp_OUT_OD    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL                               ,
    hd_gptp_OUT_PP    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP                             | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_OUT_PPU   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP                                   ,
    hd_gptp_OUT_PPD   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN                                 ,
    hd_gptp_AF_OD     = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL   | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_AF_PPU    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP       | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_AF_PPD    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN     | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_FFFFFFFF  = (u16)0xFFFF,
  };
#else
  enum{

    GPIO_Speed_400KHz = 0,//LL_GPIO_SPEED_FREQ_LOW,
    GPIO_Speed_2MHz   = 1,//LL_GPIO_SPEED_FREQ_MEDIUM,
    GPIO_Speed_10MHz  = 2,//LL_GPIO_SPEED_FREQ_HIGH,
    GPIO_Speed_40MHz  = 3,//LL_GPIO_SPEED_FREQ_VERY_HIGH,

    GPIO_OType_PP     = 0,
    GPIO_OType_OD     = 1,
    
    GPIO_Mode_AN      = 0,
    GPIO_Mode_IN      = 2,
    GPIO_Mode_OUT     = 0,
    GPIO_Mode_AF      = 2,
    
    GPIO_PuPd_NOPULL  = 0,
    GPIO_PuPd_UP      = 3,
    GPIO_PuPd_DOWN    = 2,

    dHmPRT(0, GPIO_Mode_IN),
    dHmPRT(0, GPIO_Mode_OUT),
    dHmPRT(0, GPIO_Mode_AF),
    dHmPRT(0, GPIO_Mode_AN),

    dHmPRT(1, GPIO_OType_PP),
    dHmPRT(1, GPIO_OType_OD),

    dHmPRT(2, GPIO_Speed_400KHz),
    dHmPRT(2, GPIO_Speed_2MHz),
    dHmPRT(2, GPIO_Speed_10MHz),
    dHmPRT(2, GPIO_Speed_40MHz),

    dHmPRT(3, GPIO_PuPd_NOPULL),
    dHmPRT(3, GPIO_PuPd_UP),
    dHmPRT(3, GPIO_PuPd_DOWN),
  };
  enum {
    hd_gptp_IN_OD     = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL                               ,
    hd_gptp_IN_PPU    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP                                   ,
    hd_gptp_IN_PPD    = dHmPRT_GPIO_Mode_IN  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN                                 ,
    hd_gptp_OUT_OD    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL                               ,
    hd_gptp_OUT_PP    = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP                             | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_OUT_PPU   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP                                   ,
    hd_gptp_OUT_PPD   = dHmPRT_GPIO_Mode_OUT | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN                                 ,
    hd_gptp_AF_OD     = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_OD | dHmPRT_GPIO_PuPd_NOPULL   | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_AF_PPU    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_UP       | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_AF_PPD    = dHmPRT_GPIO_Mode_AF  | dHmPRT_GPIO_OType_PP | dHmPRT_GPIO_PuPd_DOWN     | dHmPRT_GPIO_Speed_40MHz   ,
    hd_gptp_FFFFFFFF  = (u16)0xFFFF,
  };
  
#endif


//////////////////////////////////////////////////////////////////////
extern void hdGPIO_Setup(GPIO_TypeDef* GPIOx, u8 pin, u16 mode, u8 def);
//////////////////////////////////////////////////////////////////////


#endif
