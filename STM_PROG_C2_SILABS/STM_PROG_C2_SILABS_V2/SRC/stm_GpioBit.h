//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __STM_GPIO_BIT_ROBUS_H
#define __STM_GPIO_BIT_ROBUS_H

#include "SysType.h"

typedef struct {
 u32 B0 :1; u32 B1 :1; u32 B2 :1; u32 B3 :1; u32 B4 :1; u32 B5 :1; u32 B6 :1; u32 B7 :1;
 u32 B8 :1; u32 B9 :1; u32 B10:1; u32 B11:1; u32 B12:1; u32 B13:1; u32 B14:1; u32 B15:1;
 u32 B16:1; u32 B17:1; u32 B18:1; u32 B19:1; u32 B20:1; u32 B21:1; u32 B22:1; u32 B23:1;
 u32 B24:1; u32 B25:1; u32 B26:1; u32 B27:1; u32 B28:1; u32 B29:1; u32 B30:1; u32 B31:1;
} _tbit32;

typedef struct {
  _tbit32 CRL;
  _tbit32 CRH;
  _tbit32 IDR;
  _tbit32 ODR;
  _tbit32 BSRR;
  _tbit32 BRR;
  _tbit32 LCKR;
}GPIO_BIT_TypeDef;

#define GPIOA_B               ((GPIO_BIT_TypeDef *) GPIOA)
#define GPIOB_B               ((GPIO_BIT_TypeDef *) GPIOB)
#define GPIOC_B               ((GPIO_BIT_TypeDef *) GPIOC)
#define GPIOD_B               ((GPIO_BIT_TypeDef *) GPIOD)
#define GPIOE_B               ((GPIO_BIT_TypeDef *) GPIOE)
#define GPIOF_B               ((GPIO_BIT_TypeDef *) GPIOF)
#define GPIOG_B               ((GPIO_BIT_TypeDef *) GPIOG)


#define bit_elem(x) B##x;
#define bit_elem_decl(x) u8 B##x : 1;

//typedef union _bit_ar{
// u8 s;
//   u8 Bb:1;
//}bit_ar;
 
#define bit_at(un, indx) ( (un).u.bit_elem(indx) )


//typedef struct {
// _tbit32 CRL;
//}GPIO_BI_TypeDef;
//GPIO_BI_TypeDef zad;


#endif //__STM_GPIO_BIT_ROBUS_H


