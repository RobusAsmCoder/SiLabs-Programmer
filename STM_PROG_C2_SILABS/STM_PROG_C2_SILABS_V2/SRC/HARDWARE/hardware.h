//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HARDWARE_H
#define __HARDWARE_H


#include "SysType.h"
#include "hdtimers.h"
#include "hdtmrpwm.h"
#include "hdports.h"
#include "hdspi.h"
#include "hdadc.h"
/////////////////////////////////////////////////////////////////////////////
// Sys Define
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Hard Ver 2
/////////////////////////////////////////////////////////////////////////////


hdPin_O_DefineSetup(Led0,             B,  8, hd_gptp_OUT_PP,     1);
#define Led0(_v)                      dH_PORT_Write(Led0,_v)
hdPin_O_DefineSetup(Led1,             B,  9, hd_gptp_OUT_PP,     1);
#define Led1(_v)                      dH_PORT_Write(Led1,_v)

#define Key0()                        0//(dH_Key_PORT->IDR & (1<<dH_Key_PIN))

//UART DEFINES

hdPin_O_DefineSetup(UART_SET_CTS,     C, 14, hd_gptp_OUT_PP,     1);    //#define UART_SET_CTS(_val_) GPIOC->BSRR = dH_PORT_BSSR(14,_val_)
#define UART_SET_CTS(_v)              dH_PORT_Write(UART_SET_CTS,_v)
hdPin_O_DefineSetup(UART_SET_CD,      A, 15, hd_gptp_OUT_PP,     1);    //#define UART_SET_CD(_val_)  GPIOA->BSRR = dH_PORT_BSSR(15,_val_)
#define UART_SET_CD(_v)               dH_PORT_Write(UART_SET_CD,_v)
hdPin_O_DefineSetup(UART_SET_DSR,     B,  4, hd_gptp_OUT_PP,     1);    //#define UART_SET_DSR(_val_) GPIOB->BSRR = dH_PORT_BSSR( 4,_val_)
#define UART_SET_DSR(_v)              dH_PORT_Write(UART_SET_DSR,_v)
hdPin_O_DefineSetup(UART_SET_RI,      B,  6, hd_gptp_OUT_PP,     1);    //#define UART_SET_RI(_val_)  GPIOB->BSRR = dH_PORT_BSSR( 6,_val_)
#define UART_SET_RI(_v)               dH_PORT_Write(UART_SET_RI,_v)
hdPin_O_DefineSetup(UART_GET_DTR,     B,  3, hd_gptp_IN_OD,      0);    //#define UART_GET_DTR()     ((GPIOB->IDR>> 3) & 1)
#define UART_GET_DTR()                dH_PORT_Read(UART_GET_DTR)
hdPin_O_DefineSetup(UART_GET_RTS,     C, 13, hd_gptp_IN_OD,      0);    //#define UART_GET_RTS()     ((GPIOC->IDR>>13) & 1)
#define UART_GET_RTS()                dH_PORT_Read(UART_GET_RTS)

hdPin_O_DefineSetup(HERD_GET_VER,     C, 15, hd_gptp_IN_PPU,     1);    //#define HERD_GET_VER()     ((GPIOC->IDR>>15) & 1)
#define HERD_GET_VER()                dH_PORT_Read(HERD_GET_VER)

#define SPI_CLKSPEED(_val_) SPI1->CR1 = ((SPI1->CR1 & (~SPI_BaudRatePrescaler_256)) | _val_)

hdPin_O_DefineSetup(SET_TMS,          A,  4, hd_gptp_OUT_PP,     1);    //#define SET_TMS(_val_) GPIOA->BSRR = dH_PORT_BSSR(4,_val_)
#define SET_TMS(_v)                   dH_PORT_Write(SET_TMS,_v)
hdPin_O_DefineSetup(SET_TDO,          A,  8, hd_gptp_OUT_PP,     1);    //#define SET_TDO(_val_) GPIOA->BSRR = dH_PORT_BSSR(8,_val_)
#define SET_TDO(_v)                   dH_PORT_Write(SET_TDO,_v)

//#define dH_C2_PORT        GPIOA
//#define dH_C2_DAT_PIN     6
hdPin_O_DefineSetup(C2_DAT,           A,  6, hd_gptp_OUT_PP,     1);    //#define C2_DAT(_val_) dH_C2_PORT->BSRR = dH_PORT_BSSR(dH_C2_DAT_PIN,_val_)
#define C2_DAT(_v)                    dH_PORT_Write(C2_DAT,_v)
//#define C2_StrobeSendWait(_v)         C2_SendStrobeSPI(_v) 
#define C2_StrobeSendWait(_v)         SPI1->DR = _v; while( !(SPI1->SR & SPI_I2S_FLAG_RXNE)); SPI1->DR
//#define C2_StrobeSendWait(_v)         if (SPI1->SR & SPI_I2S_FLAG_RXNE) SPI1->DR; SPI1->DR = _v

//#define C2_DAT_STMINPORT    (4)   //IN FLOAT
//#define C2_DAT_STMINPORT    (8)   //IN FLOAT PULL UP/DOWN

////extern void C2_DAT_MODE_PROC_IN(void);
////extern void C2_DAT_MODE_PROC_OUT(void);
////#define C2_DAT_MODE_IN()      C2_DAT_MODE_PROC_IN()
////#define C2_DAT_MODE_OUT()     C2_DAT_MODE_PROC_OUT()

#define C2_DAT_MODE_IN()              dH_MODEIO_Set(C2_DAT, dhportF1XX_MODE_INO)//dH_C2_PORT->CRL = (dH_C2_PORT->CRL & (~(0xF<<(4*dH_C2_DAT_PIN)))) | (0x4<<(4*dH_C2_DAT_PIN))
#define C2_DAT_MODE_OUT()             dH_MODEIO_Set(C2_DAT, dhportF1XX_MODE_OPP)//dH_C2_PORT->CRL = (dH_C2_PORT->CRL & (~(0xF<<(4*dH_C2_DAT_PIN)))) | (0x3<<(4*dH_C2_DAT_PIN))

#define C2_DAT_PULL_DOWN()            dH_ODR_Write(C2_DAT,0)//dH_C2_PORT->ODR &= ~(1<<dH_C2_DAT_PIN)
#define C2_DAT_PULL_UP()              dH_ODR_Write(C2_DAT,1)//dH_C2_PORT->ODR |=  (1<<dH_C2_DAT_PIN)

#define C2_DAT_GET()                  dH_PORT_Read(C2_DAT)//((dH_C2_PORT->IDR>>dH_C2_DAT_PIN) & 1)

#define SPI_NSS(_val_)                SET_TMS(_val_)
#define SPI_GET()                     C2_DAT_GET()
#define SPI_SET(_val_)                SET_TDO(_val_)




hdPin_O_DefineSetup(PUP_EN,           D,  0, hd_gptp_OUT_PP,     1);    //#define PUP_EN(_val_)           GPIOD->BSRR = dH_PORT_BSSR( 0,_val_)
#define PUP_EN(_v)                    dH_PORT_Write(PUP_EN,_v)
#define PUP_EN_ON()                   PUP_EN(0)
#define PUP_EN_OFF()                  PUP_EN(1)

hdPin_O_DefineSetup(PEN_TDO,          B, 11, hd_gptp_OUT_PP,     1);    //#define PEN_TDO(_val_)          GPIOB->BSRR = dH_PORT_BSSR(11,_val_)
#define PEN_TDO(_v)                   dH_PORT_Write(PEN_TDO,_v)
hdPin_O_DefineSetup(PEN_TMS,          B, 10, hd_gptp_OUT_PP,     1);    //#define PEN_TMS(_val_)          GPIOB->BSRR = dH_PORT_BSSR(10,_val_)
#define PEN_TMS(_v)                   dH_PORT_Write(PEN_TMS,_v)
hdPin_O_DefineSetup(PEN_C2D,          B, 13, hd_gptp_OUT_PP,     1);    //#define PEN_C2D(_val_)          GPIOB->BSRR = dH_PORT_BSSR(13,_val_)
#define PEN_C2D(_v)                   dH_PORT_Write(PEN_C2D,_v)
hdPin_O_DefineSetup(PEN_C2CK,         B, 12, hd_gptp_OUT_PP,     1);    //#define PEN_C2CK(_val_)         GPIOB->BSRR = dH_PORT_BSSR(12,_val_)
#define PEN_C2CK(_v)                  dH_PORT_Write(PEN_C2CK,_v)

hdPin_O_DefineSetup(PUP_TDO,          A,  3, hd_gptp_OUT_PP,     1);    //#define PUP_TDO(_val_)          GPIOA->BSRR = dH_PORT_BSSR( 3,_val_)
#define PUP_TDO(_v)                   dH_PORT_Write(PUP_TDO,_v)
hdPin_O_DefineSetup(PUP_TMS,          A,  2, hd_gptp_OUT_PP,     1);    //#define PUP_TMS(_val_)          GPIOA->BSRR = dH_PORT_BSSR( 2,_val_)
#define PUP_TMS(_v)                   dH_PORT_Write(PUP_TMS,_v)
hdPin_O_DefineSetup(PUP_C2D,          A, 11, hd_gptp_OUT_PP,     1);    //#define PUP_C2D(_val_)          GPIOA->BSRR = dH_PORT_BSSR(11,_val_)
#define PUP_C2D(_v)                   dH_PORT_Write(PUP_C2D,_v)
hdPin_O_DefineSetup(PUP_C2CK,         A,  1, hd_gptp_OUT_PP,     1);    //#define PUP_C2CK(_val_)         GPIOA->BSRR = dH_PORT_BSSR( 1,_val_)
#define PUP_C2CK(_v)                  dH_PORT_Write(PUP_C2CK,_v)

#define PULL_UP_OFF()                 PUP_EN_OFF()
#define PULL_UP_LOW()                 PEN_TDO(1); PEN_TMS(1); PEN_C2D(1); PEN_C2CK(1);                      \
                                      PUP_TDO(0); PUP_TMS(0); PUP_C2D(0); PUP_C2CK(0); PUP_EN_ON()
#define PULL_UP_HIGH()                PEN_TDO(1); PEN_TMS(1); PEN_C2D(1); PEN_C2CK(1);                      \
                                      PUP_TDO(1); PUP_TMS(1); PUP_C2D(1); PUP_C2CK(1); PUP_EN_ON()
#define PULL_UP_VCC()                 PEN_TDO(0); PEN_TMS(0); PEN_C2D(0); PEN_C2CK(0); PUP_EN_ON()


#define SP_VBUS_EN(_val_)       GPIOD->BSRR = dH_PORT_BSSR( 1,_val_)
#define SP_VBUS_EN_ON()         SP_VBUS_EN(1)
#define SP_VBUS_EN_OFF()        SP_VBUS_EN(0)

extern u8 pwr_invertor;
hdPin_O_DefineSetup(POWER_PROG_EN,    B,  0, hd_gptp_OUT_PP,     0);    //#define POWER_PROG_EN(_val_)     GPIOB->BSRR = dH_PORT_BSSR( 0,_val_)
#define POWER_PROG_EN(_v)             dH_PORT_Write(POWER_PROG_EN,_v)
#define POWER_PROG_EN_ON()            POWER_PROG_EN(pwr_invertor ^ 1)
#define POWER_PROG_EN_OFF()           POWER_PROG_EN(pwr_invertor)




hdPin_O_DefineSetup(PWRPROG,          B,  1, hd_gptp_IN_OD,      0);      //ADC1 Channel 9  //hdPin_O_DefineSetup(PWRPROG,        B,  1, hd_gptp_OUT_PP,      1);      //ADC1 Channel 9
#define PWRPROG_GET()                 dH_PORT_Read(PWRPROG)
#define PWRPROG(_v)                   dH_PORT_Write(PWRPROG,_v)
#define PWRPROG_SET_IN()              dH_MODEIO_Set(PWRPROG, dhportF1XX_MODE_INO)//GPIOB->CRL = (GPIOB->CRL & 0xFFFFFF0F) | 0x00000040
#define PWRPROG_SET_INPD()            dH_MODEIO_Set(PWRPROG, dhportF1XX_MODE_INP); dH_ODR_Write(PWRPROG,0)
#define PWRPROG_SET_INPU()            dH_MODEIO_Set(PWRPROG, dhportF1XX_MODE_INP); dH_ODR_Write(PWRPROG,1)
#define PWRPROG_SET_OUT()             dH_MODEIO_Set(PWRPROG, dhportF1XX_MODE_OPP)//GPIOB->CRL = (GPIOB->CRL & 0xFFFFFF0F) | 0x00000030

extern void PWRPROG_VCC_OFF(void);
extern void PWRPROG_VCC_LOW(void);
extern void PWRPROG_VCC_HIGH(void);
extern u32 PWRPROG_VCC_MEASURE(void);

extern void C2_SendStrobeSPI(u8 b);

////////////////////////////////////////////////////////////////////////////////

/*
#define isScannerDebug      0


hdPin_O_DefineSetup(TFP_SCL,        C,  4, hd_gptp_IN_PPU,     0);  //hdPin_O_DefineSetup(I2C1_SCL,       B,  8, hd_gptp_AF_PPU,     1);     //hd_gptp_AF_PPU
hdPin_O_DefineSetup(TFP_SDA,        C,  5, hd_gptp_IN_PPU,     0);  //hdPin_O_DefineSetup(I2C1_SDA,       B,  9, hd_gptp_AF_OD,     1);    //hd_gptp_AF_OD
hdPin_O_DefineSetup(TFP_INT,        C,  8, hd_gptp_IN_PPD,     1);
hdPin_O_DefineSetup(TFP_RST,        C,  9, hd_gptp_OUT_PP,     1);
#define TFP_SCL(_v)                 dH_MODEIO_Write(TFP_SCL,_v)
#define TFP_SCL_RD()                dH_PORT_Read(TFP_SCL)
#define TFP_SDA(_v)                 dH_MODEIO_Write(TFP_SDA,_v)
#define TFP_SDA_RD()                dH_PORT_Read(TFP_SDA)
#define TFP_RST(_v)                 dH_PORT_Write(TFP_RST,_v)
//#define TFP_RST_OFF()          TFP_RST(0)
//#define TFP_RST_ON()           TFP_RST(1)

/////////////////////////////////////////////////////////////////////////////

hdPin_O_DefineSetup(MMODEN,           C, 15, hd_gptp_OUT_PP,     0);
#define MMODEN(_v)                    dH_PORT_Write(MMODEN,_v)
#define MMODEN_OFF()                  MMODEN(0)
#define MMODEN_ON()                   MMODEN(1)

hdPin_O_DefineSetup(SB,               C,  7, hd_gptp_OUT_PP,     0);
#define SB(_v)                        dH_PORT_Write(SB,_v)
#define SB_OFF()                      SB(0)
#define SB_ON()                       SB(1)

hdPin_O_DefineSetup(PAN_LED,          B,  6, hd_gptp_OUT_PP,     0);    //LM on Schematic
#define PAN_LED(_v)                   dH_PORT_Write(PAN_LED,_v)
hdPin_O_DefineSetup(PAN_KEY,          B,  7, hd_gptp_IN_OD,      1);    //LL on Schematic
#define PAN_KEY()                     dH_PORT_Read(PAN_KEY)


#if isScannerDebug==0
 hdPin_O_DefineSetup(RTS,              B,  0, hd_gptp_IN_OD,      1);
 #define RTS()                         dH_PORT_Read(RTS)
 hdPin_O_DefineSetup(CTS,              B,  1, hd_gptp_OUT_OD,     1);
 #define CTS(_v)                       dH_PORT_Write(CTS,_v)
#else
 hdPin_O_DefineSetup(DBGCLK,           B,  0, hd_gptp_IN_PPD,      1);   //RTS
 #define DBGCLK()                      dH_PORT_Read(DBGCLK)
 hdPin_O_DefineSetup(DBGMISO,          B,  1, hd_gptp_IN_PPD,      1);   //CTS
 #define DBGMISO(_v)                   dH_PORT_Read(DBGMISO)
#endif

//hdPin_O_DefineSetup(U1_SEL,         C,  3, hd_gptp_OUT_PP,     1);
//#define U1_SEL(_v)                  dH_PORT_Write(U1_SEL,_v)
//#define U1_SEL_ESP()                U1_SEL(0)
//#define U1_SEL_USB()                U1_SEL(1)

//hdPin_O_DefineSetup(SPI0_SE0,       C,  0, hd_gptp_OUT_PP,     1);
//#define SPI0_SE0(_v)                dH_PORT_Write(SPI0_SE0,_v)
//#define SPI0_SE0_SD()               SPI0_SE0(0)
//#define SPI0_SE0_STE()              SPI0_SE0(1)

//hdPin_O_DefineSetup(SPI2_SE0,       C,  5, hd_gptp_OUT_PP,     1);
//#define SPI2_SE0(_v)                dH_PORT_Write(SPI2_SE0,_v)
//#define SPI2_SE0_ESP()              SPI2_SE0(0)
//#define SPI2_SE0_FLASH()            SPI2_SE0(1)

hdPin_O_DefineSetup(STECS,          A, 15, hd_gptp_OUT_PP,     1);
#define STECS(_v)                   dH_PORT_Write(STECS,_v)
#define STECS_SHIFT()               //STECS(0)
#define STECS_LOAD()                //STECS(1)
#define STECS_RD()                  dH_PORT_Read(STECS)

hdPin_O_DefineSetup(SOUND_PWM,      A,  0, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SVOLC_PWM,      A,  1, hd_gptp_AF_PPU,     1);
#define SoundFreq(_frq)             AYxFRQupd(SoundTNE, _frq)
#define SoundVolume(_vol255)        PWM_T2C2_BYTE(_vol255)

hdPin_O_DefineSetup(XPWM_A,         A,  8, hd_gptp_AF_OD,      1);      //hd_gptp_AF_PPU
hdPin_O_DefineSetup(XPWM_B,         B,  8, hd_gptp_AF_OD,      1);
#define XPWM_A(_v255)               PWM_T1C1_BYTE(_v255)
#define XPWM_B(_v255)               PWM_T4C3_BYTE(_v255)
//#define XPWM_VCCMAX(_v,_mi,_ma,_mu) (_v)<(_m)?((_v)*((_mu)-1)/(_m)):((_mu)-1)
extern u8 XPWM_VCCMAX(s16 v, s16 mi, s16 ma, u16 mu);
#define XPWM_AVCC(_v)               XPWM_A( XPWM_VCCMAX(_v, 106,4812,256) )
#define XPWM_BVCC(_v)               XPWM_B( XPWM_VCCMAX(_v, 124,4844,256) )

typedef struct {
  s16   trv;
  s16   izv;
}tPwmDacCorrect;

#define macAddPWD_COR(_tr, _iz)     \
  {.trv = _tr,                      \
   .izv = _iz}

typedef struct {
  u16               doCorrect;
  u16               DAC_C;
  u16               DACCC;
  u16               cnl;
  tPwmDacCorrect   *CorParam;
}tPwmDacCNL;

#define macPwmDacDEF(_cn, _corPar)                  \
  { .DAC_C       = 0,                               \
    .doCorrect   = 1,                               \
    .cnl         = _cn,                             \
    .CorParam    = (void *)(&_corPar), }

extern tPwmDacCNL PwmDacCNL[];
    
extern s16 XPWM_VCCCOR(const tPwmDacCorrect *cor, s16 val);
extern void XPWM_CorrectSet(tPwmDacCNL *PWCN, s16 val);

    
#define XPWM_COAG_NORMAL()      XPWM_CorrectSet(&PwmDacCNL[1], 4450)
#define XPWM_COAG_TUR()         XPWM_CorrectSet(&PwmDacCNL[1], 4890)
    
    
//hdPin_O_DefineSetup(SD_POWER,       C,  4, hd_gptp_OUT_PP,     0);
//#define SD_POWER(_v)                dH_PORT_Write(SD_POWER,_v)

hdPin_O_DefineSetup(E422EN,         C, 14, hd_gptp_OUT_PP,     0);
#define E422EN(_v)                  dH_PORT_Write(E422EN,_v)
#define E422EN_ON()                 E422EN(1)
#define E422EN_OFF()                E422EN(0)

//hdPin_O_DefineSetup(E0SPIEN,        D, 13, hd_gptp_OUT_PP,     0);
//#define E0SPIEN(_v)                 dH_PORT_Write(E0SPIEN,_v)
//#define E0SPIEN_ON()                E0SPIEN(1)
//#define E0SPIEN_OFF()               E0SPIEN(0)

////////////////////////////////////////////////////////////////////////////////

hdPin_O_DefineSetup(ADCEA0,         C,  0, hd_gptp_IN_OD,      0);
hdPin_O_DefineSetup(ADCEA1,         C,  1, hd_gptp_IN_OD,      0);
hdPin_O_DefineSetup(ADCEB0,         C,  2, hd_gptp_IN_OD,      0);
hdPin_O_DefineSetup(ADCEB1,         C,  3, hd_gptp_IN_OD,      0);


////////////////////////////////////////////////////////////////////////////////

    hdPin_O_DefineSetup(USB_DM_ON,     A, 11, hd_gptp_IN_OD,      1);
    hdPin_O_DefineSetup(USB_DM_OFF,    A, 11, hd_gptp_OUT_PP,     0);
    hdPin_O_DefineSetup(USB_DP_ON,     A, 12, hd_gptp_IN_OD,      1);
    hdPin_O_DefineSetup(USB_DP_OFF,    A, 12, hd_gptp_OUT_PP,     0);
//    hdPin_O_DefineSetup(USB_EN,        C,  5, hd_gptp_OUT_OD,     0);
//    #define USB_EN(_v)                 dH_PORT_Write(USB_EN,_v)
//    #define USB_EN_OFF()               USB_EN(0)
//    #define USB_EN_ON()                USB_EN(1)
    extern void USBEN_OFF(void);
    extern void USBEN_ON(void);
    
/////////////////////////////////////////////////////////////////////////////

//#define LEDPWM_SET(_val)    PWM_T3C4_BYTE(_val)
extern void PWM_T1C1_BYTE(u8 val);
extern void PWM_T4C3_BYTE(u8 val);
extern void PWM_T3C4_BYTE(u8 val);
extern void PWM_T2C2_BYTE(u8 val);
extern void PWM_T5C1_BYTE(u8 val);

//#define TFTLED_OFF()    ExtBufO.TFTLED_ON = 0
//#define TFTLED_ON()     ExtBufO.TFTLED_ON = 1


//hdPin_O_DefineSetup(LEDB,           C,  8, hd_gptp_OUT_PP,     0);
//#define LEDB(_v)                    dH_PORT_Write(LEDB,_v)

//hdPin_O_DefineSetup(KEYA,           A,  0, hd_gptp_IN_OD,      1);
//#define KEYA()                      dH_PORT_Read(KEYA)

////////////////////////////////////////////////////////////////////////////////
//ADC Config
////////////////////////////////////////////////////////////////////////////////
//hdPin_O_DefineSetup(ADCINX,         C,  1, hd_gptp_IN_PPD,     0);
//hdPin_O_DefineSetup(ADCINY,         C,  2, hd_gptp_IN_PPD,     0);

//#define Read_ADC_INY()    Read_ADC_GroupBIT(ADCINXwp_bit)       //X AND Y SWITCHED WHY ???
//#define Read_ADC_INX()    Read_ADC_GroupBIT(ADCINYwp_bit)       //X AND Y SWITCHED WHY ???
*/
/////////////////////////////////////////////////////////////////////////////
// Extern Procedures
/////////////////////////////////////////////////////////////////////////////

//extern void SD_InitGPIO(void);
//extern void SD_PowerOFF(void);
//extern void SD_PowerON(void);

//extern void MCO_Config(u8 spd);
extern void Init(void);




#endif
