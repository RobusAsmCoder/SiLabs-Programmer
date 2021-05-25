//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HDPSPI_H
#define __HDPSPI_H

#include "SysType.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "hdports.h"

////////////////////////////////////////////////////////////////////////////////

enum{
  hdSPI_WorkMode_Hardware       = 0 ,
  hdSPI_WorkMode_HardSoft           ,
  hdSPI_WorkMode_HardSoftNext       ,
  hdSPI_GPIOMode_ON             = 0 ,
  hdSPI_GPIOMode_OFF                ,
  hdSPI_GPIOMode_SFT_ON             ,
  hdSPI_GPIOMode_SFT_OFF            ,
  hdSPI_GPIOMode_ReadMISO           ,
  hdSPI_GPIOMode_WriteMOSI_0        ,
  hdSPI_GPIOMode_WriteMOSI_1        ,
  hdSPI_GPIOMode_WriteCLK_0         ,
  hdSPI_GPIOMode_WriteCLK_1         ,
  hdSPI_SoftState_NOP           = 0 ,
  hdSPI_SoftState_UpdateGPIO    = 1 ,
  hdSPI_SoftState_Start         = 2 ,
  hdSPI_SoftState_End           = 4 ,
  hdSPI_SoftState_Byte          = 8 ,
  hdSPI_SoftGPIO_MOSI_0         = 0x0200  | hdSPI_SoftState_UpdateGPIO,
  hdSPI_SoftGPIO_MOSI_1         = 0x0300  | hdSPI_SoftState_UpdateGPIO,
  hdSPI_SoftGPIO_CLK_0          = 0x0800  | hdSPI_SoftState_UpdateGPIO,
  hdSPI_SoftGPIO_CLK_1          = 0x0C00  | hdSPI_SoftState_UpdateGPIO,
};

typedef struct {
  u16       Mode;
  u16       Step;
  u16       Cnt;
  u32       EventState;
  u8       *BUFTX;
  u8       *BUFRX;
  u16       BUFSize;
  u16       BUFTXWORD;
  u16       BUFRXWORD;
  u8        incTX;
  u8        incRX;
  u8        cfHoldCountST;
  u8        cfHoldCountSTSES;
  u8        cfHoldCountEN;
  u8        HoldCNT;
  u16       BaudDiv;
  u16       BaudCounter;
   u16      cfCPOL          :1;
   u16      cfCPHA          :1;
   u16      cfFirstMSBLSB   :1;
   u16      doInt           :1;
   u16                      :4;
   u16                      :8;
}thdSPIWorkStruct;

////////////////////////////////////////////////////////////////////////////////
#define dH_SPI_REG_CR(_spi, _spi_CPOL, _spi_CPHA, _spi_FIRBIT, _spi_NSS, _spi_BAUD, _spi_DSIZE )       _spi->CR1 = (_spi->CR1 & (~(SPI_CPOL_High | SPI_CPHA_2Edge | SPI_FirstBit_LSB | SPI_NSS_Soft | SPI_BaudRatePrescaler_256 | SPI_DataSize_16b ))) | (_spi_CPOL | _spi_CPHA | _spi_FIRBIT | _spi_NSS | _spi_BAUD | _spi_DSIZE)
////////////////////////////////////////////////////////////////////////////////
#define SPI_DMA_CCR(_val_)    (DMA_M2M_Disable | DMA_Priority_VeryHigh | DMA_MemoryDataSize_Byte | DMA_PeripheralDataSize_Byte | DMA_PeripheralInc_Disable | DMA_Mode_Normal | _val_)
#define DmaMemSel(_MemInc)      (_MemInc==0 ? DMA_MemoryInc_Disable : DMA_MemoryInc_Enable)
////////////////////////////////////////////////////////////////////////////////
//SPI0 CONFIG
////////////////////////////////////////////////////////////////////////////////
#define SPI0_CFG_DSIZE      SPI_DataSize_8b
#define SPI0_CFG_CPOL       SPI_CPOL_High
#define SPI0_CFG_CPHA       SPI_CPHA_2Edge
#define SPI0_CFG_NSS        SPI_NSS_Soft
#define SPI0_CFG_BAUD       SPI_BaudRatePrescaler_16
#define SPI0_CFG_FIRSTBIT   SPI_FirstBit_MSB

#define SPI1_CFG_DSIZE      SPI_DataSize_8b
#define SPI1_CFG_CPOL       SPI_CPOL_High
#define SPI1_CFG_CPHA       SPI_CPHA_2Edge
#define SPI1_CFG_NSS        SPI_NSS_Soft
#define SPI1_CFG_BAUD       SPI_BaudRatePrescaler_32
#define SPI1_CFG_FIRSTBIT   SPI_FirstBit_MSB
//SPI0 TOOLS
//#define SPI0_NSS(_val_)     if (SPI0_AB_SEL) {GPIOA->BSRR = dH_PORT_BSSR(15,(_val_));} else {GPIOA->BSRR = dH_PORT_BSSR(4,(_val_));}
//#define SPI0_NSS(_val_)     GPIOA->BSRR = dH_PORT_BSSR(4,(_val_))

#define ON_SPI1_Ax              dH_PORT_CFG_MD(GPIOB->CRL, 3, 3 | (0<<2));                              \
                                SPI1->CR1 |=  SPI_CPOL_High;                                            \
                                AFIO->MAPR = (AFIO->MAPR & (~GPIO_Remap_SPI1)) | 0x07000000;            \
                                dH_PORT_CFG_MD(GPIOA->CRL, 5, 3 | (2<<2))

#define ON_SPI1_Bx              dH_PORT_CFG_MD(GPIOA->CRL, 5, 3 | (0<<2));                              \
                                SPI1->CR1 &= ~SPI_CPOL_High;                                            \
                                AFIO->MAPR |=  GPIO_Remap_SPI1 | 0x07000000;                            \
                                dH_PORT_CFG_MD(GPIOB->CRL, 3, 3 | (2<<2))

extern u8 SPI0_AB_SEL;

#ifdef SPI0_CFG_DSIZE
static __inline void ON_SPI0_ACTIVE(void)
{
/*      dH_PORT_CFG_MD(GPIOB->CRL, 3, 3 | (0<<2));    //PB3 = IN + PULLING
      dH_SPI_REG_CR(SPI1, SPI0_CFG_CPOL, SPI0_CFG_CPHA, SPI0_CFG_FIRSTBIT, SPI0_CFG_NSS, SPI0_CFG_BAUD, SPI0_CFG_DSIZE);
      AFIO->MAPR = (AFIO->MAPR & (~GPIO_Remap_SPI1)) | 0x07000000;
      dH_PORT_CFG_MD(GPIOA->CRL, 5, 3 | (2<<2));    //PA5 = OUTAF + PUSH PULL
      SPI0_AB_SEL=0;*/
}
#endif
#ifdef SPI1_CFG_DSIZE
static __inline void ON_SPI1_ACTIVE(void)
{
      //dH_PORT_CFG_MD(GPIOA->CRL, 5, 3 | (0<<2));    //PA5 = IN + PULLING
//      SPI1->CR1 &= ~SPI_CPOL_High;     //CPOL = 0
      dH_SPI_REG_CR(SPI1, SPI1_CFG_CPOL, SPI1_CFG_CPHA, SPI1_CFG_FIRSTBIT, SPI1_CFG_NSS, SPI1_CFG_BAUD, SPI1_CFG_DSIZE);
      AFIO->MAPR |=  GPIO_Remap_SPI1 | 0x07000000;
      dH_PORT_CFG_MD(GPIOB->CRL, 3, 3 | (2<<2));    //PB3 = OUTAF + PUSH PULL
      SPI0_AB_SEL=1;
}
#endif
static __inline void ON_SPI_ACTIVE(u8 num)
{
  //ON_SPI1_ACTIVE();
      if (num==0)
      {
        if (SPI0_AB_SEL!=0) ON_SPI0_ACTIVE();
      } else {
        if (SPI0_AB_SEL==0) ON_SPI1_ACTIVE();
      }
}


////////////////////////////////////////////////////////////////////////////////
//Procedures Events Defines
////////////////////////////////////////////////////////////////////////////////
typedef u32 (tSPI_EventProc)(u32);

////////////////////////////////////////////////////////////////////////////////
//SPI Defines
////////////////////////////////////////////////////////////////////////////////
#define SPIxDefineCFG(_SPIx,_DMAN,_CHRX,_CHTX)                                  \
enum {                                                                          \
  SPI##_SPIx##vDMA        = (s32)(DMA##_DMAN),                                  \
  SPI##_SPIx##vCHRX       = (s32)(DMA##_DMAN##_Channel##_CHRX),                 \
  SPI##_SPIx##vCHTX       = (s32)(DMA##_DMAN##_Channel##_CHTX),                 \
  SPI##_SPIx##iCHRX       = (s32)(DMA##_DMAN##_Channel##_CHRX##_IRQChannel),    \
  SPI##_SPIx##iCHTX       = (s32)(DMA##_DMAN##_Channel##_CHTX##_IRQChannel),    \
  SPI##_SPIx##vFLAGRX     = (s32)(DMA##_DMAN##_FLAG_TC##_CHRX),                 \
  SPI##_SPIx##vFLAGTX     = (s32)(DMA##_DMAN##_FLAG_TC##_CHTX),                 \
  SPI##_SPIx##vAHBPeriph  = (s32)(RCC_AHBPeriph_DMA##_DMAN),                    \
}

SPIxDefineCFG(0, 1,2,3);
SPIxDefineCFG(1, 1,2,3);
SPIxDefineCFG(2, 1,4,5);
SPIxDefineCFG(3, 2,1,2);

enum {
  SPI_FLAG_MD_ST            = (s32)(1<<0),
  SPI_FLAG_MD_EN            = (s32)(1<<1),
  SPI_FLAG_MD_RX_EN         = (s32)(1<<2),
  SPI_FLAG_MD_TX_EN         = (s32)(1<<3),
};

#define SPI1_DMA_RX_IRQ         void DMA1_Channel2_IRQHandler(void) __irq
#define SPI1_DMA_TX_IRQ         void DMA1_Channel3_IRQHandler(void) __irq
#define SPI1_DMA_RX_IRQ_CLR()   DMA_ClearITPendingBit(DMA1_IT_TC2)
#define SPI1_DMA_TX_IRQ_CLR()   DMA_ClearITPendingBit(DMA1_IT_TC3)
#define SPI2_DMA_RX_IRQ         void DMA1_Channel4_IRQHandler(void) __irq
#define SPI2_DMA_TX_IRQ         void DMA1_Channel5_IRQHandler(void) __irq
#define SPI2_DMA_RX_IRQ_CLR()   DMA_ClearITPendingBit(DMA1_IT_TC4)
#define SPI2_DMA_TX_IRQ_CLR()   DMA_ClearITPendingBit(DMA1_IT_TC5)
#define SPI3_DMA_RX_IRQ         void DMA2_Channel1_IRQHandler(void) __irq
#define SPI3_DMA_TX_IRQ         void DMA2_Channel2_IRQHandler(void) __irq
#define SPI3_DMA_RX_IRQ_CLR()   DMA_ClearITPendingBit(DMA2_IT_TC1)
#define SPI3_DMA_TX_IRQ_CLR()   DMA_ClearITPendingBit(DMA2_IT_TC2)


#define SPIxDMAx(_SPIx)                   ((DMA_TypeDef *)SPI##_SPIx##vDMA)
#define SPIxDMAxFLAGx(_SPIx,_IOx)         (u32)(SPI##_SPIx##vFLAG##_IOx)
#define SPIxDMAxFLAGxRX(_SPIx)            (u32)(SPIxDMAxFLAGx(_SPIx,RX))
#define SPIxDMAxFLAGxTX(_SPIx)            (u32)(SPIxDMAxFLAGx(_SPIx,TX))
#define SPIxDMAxFLAGxRXTX(_SPIx)          (u32)(SPIxDMAxFLAGx(_SPIx,RX) | SPIxDMAxFLAGx(_SPIx,TX))
#define SPIxDMAxCHANNELx(_SPIx,_IOx)      ((DMA_Channel_TypeDef *)SPI##_SPIx##vCH##_IOx)

#define SPIx_DMA_IsDisable(_SPIx)    (SPIxDMAxCHANNELx(_SPIx,TX)->CPAR==0)
#define SPIx_DMA_IsEnable(_SPIx)     (SPIxDMAxCHANNELx(_SPIx,TX)->CPAR!=0)
#define SPIx_DMA_SetUp(_SPIx, _TX_MemAdr, _RX_MemAdr, _Size, _TX_MemInc, _RX_MemInc)              do{               \
  SPIxDMAxCHANNELx(_SPIx,TX)->CNDTR = _Size;                                                                        \
  SPIxDMAxCHANNELx(_SPIx,TX)->CMAR  = (u32)(_TX_MemAdr);                                                            \
  SPIxDMAxCHANNELx(_SPIx,TX)->CCR   = SPI_DMA_CCR(DmaMemSel(_TX_MemInc) | DMA_DIR_PeripheralDST);                   \
  SPIxDMAxCHANNELx(_SPIx,RX)->CNDTR = _Size;                                                                        \
  SPIxDMAxCHANNELx(_SPIx,RX)->CMAR  = (u32)(_RX_MemAdr);                                                            \
  SPIxDMAxCHANNELx(_SPIx,RX)->CCR   = SPI_DMA_CCR(DmaMemSel(_RX_MemInc) | DMA_DIR_PeripheralSRC);                   \
                                                                                                    }while(0)
#define SPIx_DMA_FLAGRX(_SPIx)       ((SPIxDMAxFLAGxRX(_SPIx)) & 0x0FFFFFFF)
#define SPIx_DMA_FLAGTX(_SPIx)       ((SPIxDMAxFLAGxTX(_SPIx)) & 0x0FFFFFFF)
#define SPIx_DMA_FLAG(_SPIx)         ((SPIxDMAxFLAGxRXTX(_SPIx)) & 0x0FFFFFFF)
#define SPIx_DMA_Start(_SPIx)                                                                     do{               \
  ((SPI_TypeDef *)SPI##_SPIx)->CR2 |= (SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx);                                      \
  SPIxDMAxCHANNELx(_SPIx,RX)->CCR |= 0x00000001 | 0x00000002;                                                       \
  SPIxDMAxCHANNELx(_SPIx,TX)->CCR |= 0x00000001 | 0x00000002;                                                       \
                                                                                                    }while(0)
#define SPIx_DMA_IsBusy(_SPIx)       ((SPIxDMAx(_SPIx)->ISR & SPIx_DMA_FLAG(_SPIx))!=SPIx_DMA_FLAG(_SPIx))
#define SPIx_DMA_IsBusyRX(_SPIx)     ((SPIxDMAx(_SPIx)->ISR & SPIx_DMA_FLAGRX(_SPIx))!=SPIx_DMA_FLAGRX(_SPIx))
#define SPIx_DMA_IsBusyTX(_SPIx)     ((SPIxDMAx(_SPIx)->ISR & SPIx_DMA_FLAGTX(_SPIx))!=SPIx_DMA_FLAGTX(_SPIx))
#define SPIx_DMA_End(_SPIx)                                                                       do{               \
  SPIxDMAx(_SPIx)->IFCR = SPIx_DMA_FLAG(_SPIx);                                                                     \
  SPIxDMAxCHANNELx(_SPIx,TX)->CCR &=~0x00000001;                                                                    \
  SPIxDMAxCHANNELx(_SPIx,RX)->CCR &=~0x00000001;                                                                    \
  ((SPI_TypeDef *)SPI##_SPIx)->CR2 &=~(SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx);                                      \
                                                                                                    }while(0)
#define SPIx_DMA_InitM(_SPIx)                                                                     do{               \
  RCC_AHBPeriphClockCmd(SPI##_SPIx##vAHBPeriph, ENABLE);                                                            \
  SPIxDMAxCHANNELx(_SPIx,RX)->CPAR  = (u32)(&(SPI##_SPIx->DR));                                                     \
  SPIxDMAxCHANNELx(_SPIx,TX)->CPAR  = (u32)(&(SPI##_SPIx->DR));                                                     \
                                                                                                    }while(0)
#define SPIx_DMA_InitInterrupt(_SPIx)                                                             do{               \
      NVIC_InitTypeDef NVIC_InitStructure;                                                                          \
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                                               \
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                                                     \
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                                                            \
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                                               \
      NVIC_InitStructure.NVIC_IRQChannel = SPI##_SPIx##iCHRX;                                                       \
      NVIC_Init(&NVIC_InitStructure);                                                                               \
      NVIC_InitStructure.NVIC_IRQChannel = SPI##_SPIx##iCHTX;                                                       \
      NVIC_Init(&NVIC_InitStructure);                                                                               \
                                                                                                    }while(0)


////////////////////////////////////////////////////////////////////////////////
//extern CONFIG
////////////////////////////////////////////////////////////////////////////////

#define spi_SPI_MacroFunctionGeneratorConfig(_SPIx)                                         \
extern thdSPIWorkStruct SPI##_SPIx##hsSPI_Work;                                             \
extern void SPI##_SPIx##_CFG_SOFT(u8 delST, u8 delEN, u8 baudDIV);                          \
extern void SPI##_SPIx##_GPIO_SOFT_ON(void);                                                \
extern void SPI##_SPIx##_GPIO_OFF(void);                                                    \
extern void SPI##_SPIx##_GPIO_ON(void);                                                     \
extern void SPI##_SPIx##_Init(void);                                                        \
extern void SPI##_SPIx##_DMA_Start(void);                                                   \
extern u8 SPI##_SPIx##_DMA_IsBusy(void);                                                    \
extern void SPI##_SPIx##_DMA_End(void);                                                     \
extern u8 SPI##_SPIx##_DMA_IsDisable(void);                                                 \
extern u8 SPI##_SPIx##_DMA_IsEnable(void);                                                  \
extern void SPI##_SPIx##_DMA_SetUp(u8 *bpTX, u8 *bpRX, u32 size, u8 incTX, u8 incRX);       \
extern void SPI##_SPIx##_WriteReadBlock(u8 *bufo, u8 *bufi, u32 size);                      \
extern void SPI##_SPIx##_WriteBlock(u8 *buf, u32 size);                                     \
extern void SPI##_SPIx##_ReadBlock(u8 *buf, u32 size);                                      \
extern void SPI##_SPIx##_Send_String(char *s);                                              \
extern void SPI##_SPIx##_DMA_DoInterruptRX(void);                                           \
extern void SPI##_SPIx##_DMA_DoInterruptTX(void);                                           \
extern void SPI##_SPIx##_DMA_SetInterrupt(tSPI_EventProc *Event);                           \
extern void SPI##_SPIx##_DMA_Init(void);                                                    \
extern u32 SPI##_SPIx##_GPIO(u8 isOn)

////////////////////////////////////////////////////////////////////////////////
//SPI0 CONFIG
////////////////////////////////////////////////////////////////////////////////
hdPin_O_DefineSetup(SPI0_CLK,       A,  5, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SPI0_MISO,      A,  6, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI0_MOSI,      A,  7, hd_gptp_AF_PPU,     1);
//hdPin_O_DefineSetup(SPI0_NSSA,      A,  4, hd_gptp_OUT_PP,     1);
//#define SPI0_NSS(_v)                dH_PORT_Write(SPI0_NSSA,_v)

////////////////////////////////////////////////////////////////////////////////
//SPI1 CONFIG
////////////////////////////////////////////////////////////////////////////////
//hdPin_O_DefineSetup(SPI1_CLK,       B,  3, hd_gptp_AF_PPU,     1);
//hdPin_O_DefineSetup(SPI1_MISO,      B,  4, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI1_MOSI,      B,  5, hd_gptp_AF_PPU,     1);    //hd_gptp_AF_PPU
//hdPin_O_DefineSetup(SPI1_NSSA,      A, 15, hd_gptp_OUT_PP,     1);


////////////////////////////////////////////////////////////////////////////////
//SPI2 CONFIG
////////////////////////////////////////////////////////////////////////////////
#define SPI2_CFG_DSIZE      SPI_DataSize_8b
#define SPI2_CFG_CPOL       SPI_CPOL_Low
#define SPI2_CFG_CPHA       SPI_CPHA_1Edge
#define SPI2_CFG_NSS        SPI_NSS_Soft
#define SPI2_CFG_BAUD       SPI_BaudRatePrescaler_128
#define SPI2_CFG_FIRSTBIT   SPI_FirstBit_MSB
hdPin_O_DefineSetup(SPI2_CLK,       B, 13, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SPI2_MISO,      B, 14, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI2_MOSI,      B, 15, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SPI2_NSS,       B, 12, hd_gptp_OUT_PP,     1);
#define SPI2_NSS(_v)                dH_PORT_Write(SPI2_NSS,_v)
#define SPI2_NSS_ON()               SPI2_NSS(0)
#define SPI2_NSS_OFF()              SPI2_NSS(1)
hdPin_O_DefineSetup(SPI2sCLK,       B, 13, hd_gptp_OUT_PP,     1);
hdPin_O_DefineSetup(SPI2sMISO,      B, 14, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI2sMOSI,      B, 15, hd_gptp_OUT_PP,     1);
#define SPI2sCLK(_v)                dH_PORT_Write(SPI2sCLK,_v)
#define SPI2sMISO()                 dH_PORT_Read(SPI2sMISO)
#define SPI2sMOSI(_v)               dH_PORT_Write(SPI2sMOSI,_v)

////////////////////////////////////////////////////////////////////////////////
//SPI3 CONFIG
////////////////////////////////////////////////////////////////////////////////
#define SPI3_CFG_DSIZE      SPI_DataSize_8b
#define SPI3_CFG_CPOL       SPI_CPOL_High
#define SPI3_CFG_CPHA       SPI_CPHA_2Edge
#define SPI3_CFG_NSS        SPI_NSS_Soft//SPI_NSS_Hard//SPI_NSS_Soft
#define SPI3_CFG_BAUD       SPI_BaudRatePrescaler_8
#define SPI3_CFG_FIRSTBIT   SPI_FirstBit_MSB
hdPin_O_DefineSetup(SPI3_CLK,       B,  3, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SPI3_MISO,      B,  4, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI3_MOSI,      B,  5, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(SPI3_NSSA,      A, 15, hd_gptp_OUT_PP,     1);    //FLCSA   //hd_gptp_OUT_PP

hdPin_O_DefineSetup(SPI3sCLK,       B,  3, hd_gptp_OUT_PP,     1);
hdPin_O_DefineSetup(SPI3sMISO,      B,  4, hd_gptp_IN_PPU,     1);
hdPin_O_DefineSetup(SPI3sMOSI,      B,  5, hd_gptp_OUT_PP,     1);
#define SPI3sCLK(_v)                dH_PORT_Write(SPI3sCLK,_v)
#define SPI3sMISO()                 dH_PORT_Read(SPI3sMISO)
#define SPI3sMOSI(_v)               dH_PORT_Write(SPI3sMOSI,_v)



spi_SPI_MacroFunctionGeneratorConfig(0);
spi_SPI_MacroFunctionGeneratorConfig(1);
spi_SPI_MacroFunctionGeneratorConfig(2);
spi_SPI_MacroFunctionGeneratorConfig(3);

extern void SPIT_ReadBlock(u8 *buf, u32 size)                                      ;
extern void SPIT_WriteBlock(u8 *buf, u32 size)                                     ;
extern void SPIT_WriteReadBlock(u8 *bufo, u8 *bufi, u32 size)                      ;

//#define SPI_NSS(_val_)      SPI0_NSS(_val_)
extern vu8 SPI_CRC8;
extern u8 SPI_WriteReadByte(u8 b);
extern u8 SPI_WriteReadByte_mc_CRC(u8 b);
extern u8 SPI_WriteByte(u16 b);
extern void SPI_ReadBlock(u8 *buf, u32 size);
extern void SPI_WriteBlock(u8 *buf, u32 size);

extern void hdSPI_SoftPoolGoToSTART(thdSPIWorkStruct *HSW);
extern void hdSPI_SoftPoolGoToBYTEPROCESS(thdSPIWorkStruct *HSW);
extern u32 hdSPI_SoftPool(thdSPIWorkStruct *HSW, u32 MISOBIT);

#endif
