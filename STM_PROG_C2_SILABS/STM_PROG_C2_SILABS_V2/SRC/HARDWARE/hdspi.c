//////////////////////////////////////////////////////////////////////////////////////
// 
// Low level for STM32F100
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "hdspi.h"
#include <string.h>


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

u8 SPI0_AB_SEL=0;

//#define SPIx_DMA_Start()            SPIx_DMA_Start(2)
//#define SPIx_DMA_IsBusy()           SPIx_DMA_IsBusy(2)
//#define SPIx_DMA_End()              SPIx_DMA_End(2)
//#define SPIx_DMA_IsDisable()        SPIx_DMA_IsDisable(2)
//#define SPIx_DMA_InitM()            SPIx_DMA_InitM(2)
//#define SPIx_DMA_SetUp(_SPIx_TX_MemAdr, _RX_MemAdr, _Size, _TX_MemInc, _RX_MemInc)    SPIx_DMA_SetUp(2, _TX_MemAdr, _RX_MemAdr, _Size, _TX_MemInc, _RX_MemInc)

void SPI_RCC_APB_PeriphClockCmd(u8 num)
{
    switch (num)
    {
      case 0:
      case 1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        break;
      case 2:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        break;
      case 3:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
        break;
    }
}

//////////////////////////////////////////////////////////////
#define SPI0              SPI1
#define SPI0vCHRX         SPI1vCHRX
#define SPI0vCHTX         SPI1vCHTX
#define SPI0vDMA          SPI1vDMA
#define SPI0vFLAGRX       SPI1vFLAGRX
#define SPI0vFLAGTX       SPI1vFLAGTX
#define SPI0vAHBPeriph    SPI1vAHBPeriph
//////////////////////////////////////////////////////////////
void hdSPI_SoftPoolGoToSTART(thdSPIWorkStruct *HSW)
{
    HSW->Step = 0;//cfHoldCountST
}

void hdSPI_SoftPoolGoToBYTEPROCESS(thdSPIWorkStruct *HSW)
{
    HSW->Step = 1;//cfHoldCountST
}
// IN:MISOBIT Every Scan And Locked If Need !!!
//OUT:         1E 47  E1 b8
u32 hdSPI_SoftPool(thdSPIWorkStruct *HSW, u32 MISOBIT)
{
    u32 res = hdSPI_SoftState_NOP;
    if (HSW->Mode==hdSPI_WorkMode_HardSoft)
    {
      if (HSW->BaudCounter>=HSW->BaudDiv)
      {
        u16 w;
        u16 stp = HSW->Step;
        HSW->BaudCounter=0;
        //u16 cnt = HSW->Cnt;
        //u8 cct = cnt>>1;
        //u16 ccm = 1<<cct;
        u8 cct;
        u16 ccm;
        switch (HSW->Step)
        {
          case 0x0000:
            res = hdSPI_SoftState_Start;
            HSW->cfHoldCountSTSES = HSW->cfHoldCountST;
            stp++;
            break;
          case 0x0001:
            HSW->HoldCNT = HSW->cfHoldCountSTSES;
            stp++;
          case 0x0002:
            if (HSW->HoldCNT)
            {
              HSW->HoldCNT--;
              break;
            } else {
              HSW->HoldCNT = HSW->cfHoldCountEN;
              stp++;
            }
          case 0x0003:
            if (HSW->BUFSize==0)
            {
              if (HSW->HoldCNT)
              {
                HSW->HoldCNT--;
              } else {
                stp=0x0000;
                res = hdSPI_SoftState_End;
              }
              res|= ((1^(HSW->cfCPOL?1:0))==0 ? hdSPI_SoftGPIO_CLK_1 : hdSPI_SoftGPIO_CLK_0);
              //res|= hdSPI_SoftGPIO_MOSI_0;
              break;
            }
            HSW->BUFTXWORD = HSW->BUFTX ? *HSW->BUFTX++ : 0xFF;
            HSW->Cnt=0;
            stp++;
          case 0x0004:
            cct = HSW->Cnt>>1;
            ccm = HSW->cfFirstMSBLSB ? (0x01<<cct) : (0x80>>cct);
            w=HSW->BUFTXWORD & ccm;
            res|= (((HSW->Cnt&1)^(HSW->cfCPOL?1:0))==0 ? hdSPI_SoftGPIO_CLK_0 : hdSPI_SoftGPIO_CLK_1) | (w ? hdSPI_SoftGPIO_MOSI_1 : hdSPI_SoftGPIO_MOSI_0);
            if ( ((HSW->Cnt&1)^(HSW->cfCPHA?1:0))==0 )
            {
              w = HSW->BUFRXWORD;
              w =  (MISOBIT ? (w | ccm) : (w & (~ccm)) );
              HSW->BUFRXWORD = w;
              //if (cct==7) res|= hdSPI_SoftState_Byte;
            }
            HSW->Cnt++;
            if (HSW->Cnt==16)
            {
              res|= hdSPI_SoftState_Byte;
              if (HSW->BUFRX)
              {
                *HSW->BUFRX++ = HSW->BUFRXWORD;
              }
              HSW->BUFSize--;
              stp--;
            }
            break;
        }
        HSW->Step = stp;
      } else {
        HSW->BaudCounter++;
      }
    }
    HSW->EventState=res;
    return res;
}


#define spi_SPI_MacroFunctionGenerator(_SPIx)                                       \
vu8 SPI##_SPIx##_tmpx[2]={0xFF,0xFF};                                               \
vu8 SPI##_SPIx##_Flag=0x00;                                                         \
thdSPIWorkStruct SPI##_SPIx##hsSPI_Work = {                                         \
 .Mode = hdSPI_WorkMode_Hardware,                                                   \
 .Step = 0,                                                                         \
 .BUFTX=0,                                                                          \
 .BUFRX=0,                                                                          \
 .BUFSize=0,                                                                        \
 .cfCPOL = (SPI##_SPIx##_CFG_CPOL==SPI_CPOL_Low) ? 0:1,                             \
 .cfCPHA = (SPI##_SPIx##_CFG_CPHA==SPI_CPHA_1Edge) ? 0:1,                           \
 .cfFirstMSBLSB = (SPI##_SPIx##_CFG_FIRSTBIT==SPI_FirstBit_MSB) ? 0:1,              \
 .doInt = 0,                                                                        \
 .cfHoldCountST = 0,                                                                \
 .cfHoldCountEN = 0,                                                                \
 .BaudCounter = 0,                                                                  \
 .BaudDiv = 0,                                                                      \
};                                                                                  \
tSPI_EventProc *SPI##_SPIx##_Event=0;                                               \
void SPI##_SPIx##_SendWait(void)                                                    \
{                                                                                   \
    while (!(SPI##_SPIx->SR & SPI_I2S_FLAG_TXE));                                   \
}                                                                                   \
void SPI##_SPIx##_ReceiveWait(void)                                                 \
{                                                                                   \
    while( !(SPI##_SPIx->SR & SPI_I2S_FLAG_RXNE));                                  \
}                                                                                   \
void SPI##_SPIx##_Send(u16 b)                                                       \
{                                                                                   \
    SPI##_SPIx##_SendWait();                                                        \
    SPI_I2S_SendData(SPI##_SPIx, b);                                                \
}                                                                                   \
u16 SPI##_SPIx##_Receive(void)                                                      \
{                                                                                   \
    SPI##_SPIx##_ReceiveWait();                                                     \
    return SPI_I2S_ReceiveData(SPI##_SPIx);                                         \
}                                                                                   \
u8 SPI##_SPIx##_IsBusy(void)                                                        \
{                                                                                   \
    return SPI##_SPIx->SR & SPI_I2S_FLAG_BSY;                                       \
}                                                                                   \
void SPI##_SPIx##_WaitBusy(void)                                                    \
{                                                                                   \
    while (SPI##_SPIx##_IsBusy());                                                  \
}                                                                                   \
u8 SPI##_SPIx##_WriteReadByte(u8 b)                                                 \
{                                                                                   \
  SPI##_SPIx##_Send(b);                                                             \
  return SPI##_SPIx##_Receive();                                                    \
}                                                                                   \
void SPI##_SPIx##_CFG_SOFT(u8 delST, u8 delEN, u8 baudDIV)                          \
{                                                                                   \
    SPI##_SPIx##hsSPI_Work.cfHoldCountST = delST;                                   \
    SPI##_SPIx##hsSPI_Work.cfHoldCountEN = delEN;                                   \
    SPI##_SPIx##hsSPI_Work.BaudDiv = baudDIV;                                       \
}                                                                                   \
void SPI##_SPIx##_GPIO_SOFT_ON(void)                                                \
{                                                                                   \
      SPI##_SPIx##_GPIO(hdSPI_GPIOMode_SFT_ON);                                     \
      SPI##_SPIx##hsSPI_Work.Mode=hdSPI_WorkMode_HardSoft;                          \
}                                                                                   \
void SPI##_SPIx##_GPIO_ON(void)                                                     \
{                                                                                   \
      SPI##_SPIx##_GPIO(hdSPI_GPIOMode_ON);                                         \
      SPI##_SPIx##hsSPI_Work.Mode=hdSPI_WorkMode_Hardware;                          \
}                                                                                   \
void SPI##_SPIx##_GPIO_OFF(void)                                                    \
{                                                                                   \
      SPI##_SPIx##_GPIO(hdSPI_GPIOMode_OFF);                                        \
      SPI##_SPIx##hsSPI_Work.Mode=hdSPI_WorkMode_Hardware;                          \
}                                                                                   \
void SPI##_SPIx##_Init(void)                                                        \
{                                                                                   \
  SPI_InitTypeDef   SPI_InitStructure;                                              \
  SPI_RCC_APB_PeriphClockCmd(_SPIx);                                                \
  SPI_InitStructure.SPI_Direction           = SPI_Direction_2Lines_FullDuplex;      \
  SPI_InitStructure.SPI_Mode                = SPI_Mode_Master;                      \
  SPI_InitStructure.SPI_DataSize            = SPI##_SPIx##_CFG_DSIZE;               \
  SPI_InitStructure.SPI_CPOL                = SPI##_SPIx##_CFG_CPOL;                \
  SPI_InitStructure.SPI_CPHA                = SPI##_SPIx##_CFG_CPHA;                \
  SPI_InitStructure.SPI_NSS                 = SPI##_SPIx##_CFG_NSS;                 \
  SPI_InitStructure.SPI_BaudRatePrescaler   = SPI##_SPIx##_CFG_BAUD;                \
  SPI_InitStructure.SPI_FirstBit            = SPI##_SPIx##_CFG_FIRSTBIT;            \
  SPI_InitStructure.SPI_CRCPolynomial       = 7;                                    \
  SPI_Init(SPI##_SPIx, &SPI_InitStructure);                                         \
  SPI_Cmd(SPI##_SPIx, ENABLE);                                                      \
}                                                                                   \
void SPI##_SPIx##_DMA_StartHRD(void)                                                \
{                                                                                   \
  SPI##_SPIx##_Flag &= ~(SPI_FLAG_MD_RX_EN | SPI_FLAG_MD_TX_EN);                    \
  SPI##_SPIx##_Flag |=  (SPI_FLAG_MD_ST);                                           \
  SPIx_DMA_Start(_SPIx);                                                            \
}                                                                                   \
void SPI##_SPIx##_DMA_Start(void)                                                   \
{                                                                                   \
  if ( SPI##_SPIx##_Event ) SPI##_SPIx##_Event(SPI_FLAG_MD_ST);                     \
  SPI##_SPIx##_DMA_StartHRD();                                                      \
}                                                                                   \
u8 SPI##_SPIx##_DMA_IsBusyHRD(void)                                                 \
{                                                                                   \
  return SPIx_DMA_IsBusy(_SPIx);                                                    \
}                                                                                   \
u8 SPI##_SPIx##_DMA_IsBusy(void)                                                    \
{                                                                                   \
  return (SPI##_SPIx##_Flag & (SPI_FLAG_MD_RX_EN | SPI_FLAG_MD_TX_EN)) !=           \
                              (SPI_FLAG_MD_RX_EN | SPI_FLAG_MD_TX_EN);              \
}                                                                                   \
void SPI##_SPIx##_DMA_End(void)                                                     \
{                                                                                   \
  SPIx_DMA_End(_SPIx);                                                              \
}                                                                                   \
u8 SPI##_SPIx##_DMA_IsDisable(void)                                                 \
{                                                                                   \
  return SPIx_DMA_IsDisable(_SPIx);                                                 \
}                                                                                   \
u8 SPI##_SPIx##_DMA_IsEnable(void)                                                  \
{                                                                                   \
  return SPIx_DMA_IsEnable(_SPIx);                                                  \
}                                                                                   \
void SPI##_SPIx##_DMA_SetUpM(u16 md, u8 *bpTX, u8 *bpRX, u32 size, u8 incTX, u8 incRX)       \
{                                                                                   \
  switch (md)                                                                       \
  {                                                                                 \
    case hdSPI_WorkMode_Hardware:                                                   \
      SPIx_DMA_SetUp(_SPIx, bpTX, bpRX, size, incTX, incRX);                        \
      break;                                                                        \
    case hdSPI_WorkMode_HardSoft:                                                   \
      SPI##_SPIx##hsSPI_Work.BUFTX = bpTX;                                          \
      SPI##_SPIx##hsSPI_Work.BUFRX = bpRX;                                          \
      SPI##_SPIx##hsSPI_Work.BUFSize = size;                                        \
    case hdSPI_WorkMode_HardSoftNext:                                               \
      SPIx_DMA_SetUp(_SPIx, (u8 *)&SPI##_SPIx##_tmpx[0], (u8 *)&SPI##_SPIx##_tmpx[1], 1, 0, 0);                        \
      SPI##_SPIx##hsSPI_Work.doInt = 0;                                             \
      break;                                                                        \
  }                                                                                 \
}                                                                                   \
void SPI##_SPIx##_DMA_SetUp(u8 *bpTX, u8 *bpRX, u32 size, u8 incTX, u8 incRX)       \
{                                                                                   \
  SPI##_SPIx##_DMA_SetUpM(SPI##_SPIx##hsSPI_Work.Mode,bpTX,bpRX,size,incTX,incRX);  \
}                                                                                   \
void SPI##_SPIx##_DMA_StartWaitEnd(void)                                            \
{                                                                                   \
      SPI##_SPIx##_DMA_Start();                                                     \
      while (SPI##_SPIx##_DMA_IsBusy());                                            \
      SPI##_SPIx##_DMA_End();                                                       \
}                                                                                   \
void SPI##_SPIx##_WriteReadBlock(u8 *bufo, u8 *bufi, u32 size)                      \
{                                                                                   \
  if (size)                                                                         \
  {                                                                                 \
    if (SPI##_SPIx##_DMA_IsDisable())                                               \
    {                                                                               \
      if ( SPI##_SPIx##_Event ) SPI##_SPIx##_Event(SPI_FLAG_MD_ST);                 \
      while (size)                                                                  \
      {                                                                             \
        u8 b = bufo ? *bufo++ : SPI##_SPIx##_tmpx[0];                               \
        b = SPI##_SPIx##_WriteReadByte(b);                                          \
        if (bufi) *bufi++ = b;                                                      \
      }                                                                             \
      SPI##_SPIx##_DMA_DoInterruptRX();                                             \
      SPI##_SPIx##_DMA_DoInterruptTX();                                             \
    } else {                                                                        \
      u8 *pbo = bufo?bufo:((u8 *)&SPI##_SPIx##_tmpx[0]);                            \
      u8 *pbi = bufi?bufi:((u8 *)&SPI##_SPIx##_tmpx[1]);                            \
      u8 inco = bufo?1:0;                                                           \
      u8 inci = bufi?1:0;                                                           \
      SPI##_SPIx##_DMA_SetUp(pbo, pbi, size, inco,inci);                            \
      SPI##_SPIx##_DMA_StartWaitEnd();                                              \
    }                                                                               \
  }                                                                                 \
}                                                                                   \
void SPI##_SPIx##_WriteBlock(u8 *buf, u32 size)                                     \
{                                                                                   \
  SPI##_SPIx##_WriteReadBlock(buf, 0, size);                                        \
}                                                                                   \
void SPI##_SPIx##_ReadBlock(u8 *buf, u32 size)                                      \
{                                                                                   \
  SPI##_SPIx##_WriteReadBlock(0, buf, size);                                        \
}                                                                                   \
void SPI##_SPIx##_Send_String(char *s)                                              \
{                                                                                   \
    u16 siz = strlen(s);                                                            \
    SPI##_SPIx##_WriteBlock((u8 *)s,siz);                                           \
}                                                                                   \
u32 SPI##_SPIx##_DMA_DoInterruptAny(void)                                           \
{                                                                                   \
  u32 res=0;                                                                        \
  u32 ev;                                                                           \
  switch (SPI##_SPIx##hsSPI_Work.Mode)                                              \
  {                                                                                 \
    case hdSPI_WorkMode_Hardware:                                                   \
      break;                                                                        \
    case hdSPI_WorkMode_HardSoft:                                                   \
      res=1;                                                                        \
      if (SPI##_SPIx##hsSPI_Work.doInt==0)                                          \
      {                                                                             \
        ev=hdSPI_SoftPool(&SPI##_SPIx##hsSPI_Work, SPI##_SPIx##_GPIO(hdSPI_GPIOMode_ReadMISO));    \
        if (ev & hdSPI_SoftState_UpdateGPIO)                                        \
        {                                                                           \
          if ((ev&0x0300)==0x0200 ) SPI##_SPIx##_GPIO(hdSPI_GPIOMode_WriteMOSI_0);  \
          if ((ev&0x0300)==0x0300 ) SPI##_SPIx##_GPIO(hdSPI_GPIOMode_WriteMOSI_1);  \
          if ((ev&0x0C00)==0x0800 ) SPI##_SPIx##_GPIO(hdSPI_GPIOMode_WriteCLK_0);   \
          if ((ev&0x0C00)==0x0C00 ) SPI##_SPIx##_GPIO(hdSPI_GPIOMode_WriteCLK_1);   \
        }                                                                           \
        res = (ev&hdSPI_SoftState_End) ? 0 : 1;                                     \
      } else {                                                                      \
        ev=SPI##_SPIx##hsSPI_Work.EventState;                                       \
        res = (ev&hdSPI_SoftState_End) ? 0 : 2;                                     \
      }                                                                             \
      SPI##_SPIx##hsSPI_Work.doInt^= 1;                                             \
      break;                                                                        \
  }                                                                                 \
  return res;                                                                       \
}                                                                                   \
void SPI##_SPIx##_DMA_DoInterruptRX(void)                                           \
{                                                                                   \
    switch (SPI##_SPIx##_DMA_DoInterruptAny())                                      \
    {                                                                               \
      case 0:                                                                       \
        SPI##_SPIx##_Flag |= SPI_FLAG_MD_RX_EN;                                     \
        if ( SPI##_SPIx##_Event )                                                   \
        {                                                                           \
          SPI##_SPIx##_Event(SPI_FLAG_MD_RX_EN);                                    \
          if ( !SPI##_SPIx##_DMA_IsBusy() ) SPI##_SPIx##_Event(SPI_FLAG_MD_EN);     \
        }                                                                           \
        break;                                                                      \
      case 1:                                                                       \
        break;                                                                      \
      case 2:                                                                       \
        SPI##_SPIx##_DMA_End();                                                     \
        SPI##_SPIx##_DMA_SetUpM(hdSPI_WorkMode_HardSoftNext,0,0,0,0,0);             \
        SPI##_SPIx##_DMA_StartHRD();                                                \
        break;                                                                      \
    }                                                                               \
}                                                                                   \
void SPI##_SPIx##_DMA_DoInterruptTX(void)                                           \
{                                                                                   \
    switch (SPI##_SPIx##_DMA_DoInterruptAny())                                      \
    {                                                                               \
      case 0:                                                                       \
        SPI##_SPIx##_Flag |= SPI_FLAG_MD_TX_EN;                                     \
        if ( SPI##_SPIx##_Event )                                                   \
        {                                                                           \
          SPI##_SPIx##_Event(SPI_FLAG_MD_TX_EN);                                    \
          if ( !SPI##_SPIx##_DMA_IsBusy() ) SPI##_SPIx##_Event(SPI_FLAG_MD_EN);     \
        }                                                                           \
        break;                                                                      \
      case 1:                                                                       \
        break;                                                                      \
      case 2:                                                                       \
        SPI##_SPIx##_DMA_End();                                                     \
        SPI##_SPIx##_DMA_SetUpM(hdSPI_WorkMode_HardSoftNext,0,0,0,0,0);             \
        SPI##_SPIx##_DMA_StartHRD();                                                \
        break;                                                                      \
    }                                                                               \
}                                                                                   \
void SPI##_SPIx##_DMA_SetInterrupt(tSPI_EventProc *Event)                           \
{                                                                                   \
  SPI##_SPIx##_Event = Event;                                                       \
}                                                                                   \
void SPI##_SPIx##_DMA_Init(void)                                                    \
{                                                                                   \
    SPIx_DMA_InitM(_SPIx);                                                          \
    SPIx_DMA_InitInterrupt(_SPIx);                                                  \
}




//        if ( ev&hdSPI_SoftState_End )                                               \
        {                                                                           \
          res=0;                                                                    \
        } else {                                                                    \
          res=1;                                                                    \
        }                                                                           \
        if ( ev&hdSPI_SoftState_End )                                               \
        {                                                                           \
          res=0;                                                                    \
        } else {                                                                    \
          res=2;                                                                    \
        }                                                                           \



///////////////////////

SPI1_DMA_RX_IRQ
{
    if (SPI0_AB_SEL==0)
    {
      SPI0_DMA_DoInterruptRX();
    } else {
      SPI1_DMA_DoInterruptRX();
    }
    SPI1_DMA_RX_IRQ_CLR();
}

SPI1_DMA_TX_IRQ
{
    if (SPI0_AB_SEL==0)
    {
      SPI0_DMA_DoInterruptTX();
    } else {
      SPI1_DMA_DoInterruptTX();
    }
    SPI1_DMA_TX_IRQ_CLR();
}

u32 SPI0_GPIO(u8 md)
{
  u32 res=0;
  switch (md)
  {
    case hdSPI_GPIOMode_ON:
      GpioInit_SPI0_CLK();
      GpioInit_SPI0_MISO();
      GpioInit_SPI0_MOSI();
      //GpioInit_SPI0_NSSA();
      //SPI0_GPIO_ON_A();
      ON_SPI_ACTIVE(0);//ON_SPI1_ACTIVE();//ON_SPI1_Bx;
      break;
    case hdSPI_GPIOMode_OFF:
      break;
    case hdSPI_GPIOMode_SFT_ON:
      break;
    case hdSPI_GPIOMode_SFT_OFF:
      break;
  }
  return res;
}

u32 SPI1_GPIO(u8 md)
{
  u32 res=0;
  switch (md)
  {
    case hdSPI_GPIOMode_ON:
//      GpioInit_SPI1_CLK();
//      GpioInit_SPI1_MISO();
      GpioInit_SPI1_MOSI();
      //SPI0_GPIO_ON_A();
      ON_SPI_ACTIVE(1);//GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);//ON_SPI1_ACTIVE();//ON_SPI1_Bx;
      break;
    case hdSPI_GPIOMode_OFF:
      break;
    case hdSPI_GPIOMode_SFT_ON:
      break;
    case hdSPI_GPIOMode_SFT_OFF:
      break;
  }
  return res;
}

SPI2_DMA_RX_IRQ
{
      SPI2_DMA_DoInterruptRX();
      SPI2_DMA_RX_IRQ_CLR();
}
SPI2_DMA_TX_IRQ
{
      SPI2_DMA_DoInterruptTX();
      SPI2_DMA_TX_IRQ_CLR();
}
u32 SPI2_GPIO(u8 md)
{
  u32 res=0;
  switch (md)
  {
    case hdSPI_GPIOMode_ON:
      GpioInit_SPI2_CLK();
      GpioInit_SPI2_MISO();
      GpioInit_SPI2_MOSI();
      GpioInit_SPI2_NSS();
    //  GPIO_PinAFConfig(GPIOB, 13, GPIO_AF_SPI2 );   //CLK
    //  GPIO_PinAFConfig(GPIOB, 14, GPIO_AF_SPI2 );   //MISO
    //  GPIO_PinAFConfig(GPIOB, 15, GPIO_AF_SPI2 );   //MOSI
      break;
    case hdSPI_GPIOMode_OFF:
      break;
    case hdSPI_GPIOMode_SFT_ON:
      GpioInit_SPI2sCLK();
      GpioInit_SPI2sMISO();
      GpioInit_SPI2sMOSI();
      GpioInit_SPI2_NSS();
      break;
    case hdSPI_GPIOMode_SFT_OFF:
      break;
    case hdSPI_GPIOMode_ReadMISO:
      res = SPI2sMISO();
      break;
    case hdSPI_GPIOMode_WriteMOSI_0:
      SPI2sMOSI(0);
      break;
    case hdSPI_GPIOMode_WriteMOSI_1:
      SPI2sMOSI(1);
      break;
    case hdSPI_GPIOMode_WriteCLK_0:
      SPI2sCLK(0);
      break;
    case hdSPI_GPIOMode_WriteCLK_1:
      SPI2sCLK(1);
      break;
  }
  return res;
}


SPI3_DMA_RX_IRQ
{
      SPI3_DMA_DoInterruptRX();
      SPI3_DMA_RX_IRQ_CLR();
}
SPI3_DMA_TX_IRQ
{
      SPI3_DMA_DoInterruptTX();
      SPI3_DMA_TX_IRQ_CLR();
}
u32 SPI3_GPIO(u8 md)
{
  u32 res=0;
  switch (md)
  {
    case hdSPI_GPIOMode_ON:
      GpioInit_SPI3_CLK();
      GpioInit_SPI3_MISO();
      GpioInit_SPI3_MOSI();
      GpioInit_SPI3_NSSA();
    //  GpioInit_SPI3_NSSB();
    //  GpioInit_SPI3_NSSC();
    //  GpioInit_SPI3_NSSD();
      break;
    case hdSPI_GPIOMode_OFF:
      break;
    case hdSPI_GPIOMode_SFT_ON:
      GpioInit_SPI3sCLK();
      GpioInit_SPI3sMISO();
      GpioInit_SPI3sMOSI();
      GpioInit_SPI3_NSSA();
      break;
    case hdSPI_GPIOMode_SFT_OFF:
      break;
    case hdSPI_GPIOMode_ReadMISO:
      res = SPI3sMISO();
      break;
    case hdSPI_GPIOMode_WriteMOSI_0:
      SPI3sMOSI(0);
      break;
    case hdSPI_GPIOMode_WriteMOSI_1:
      SPI3sMOSI(1);
      break;
    case hdSPI_GPIOMode_WriteCLK_0:
      SPI3sCLK(0);
      break;
    case hdSPI_GPIOMode_WriteCLK_1:
      SPI3sCLK(1);
      break;
  }
  return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

spi_SPI_MacroFunctionGenerator(0);
spi_SPI_MacroFunctionGenerator(1);
spi_SPI_MacroFunctionGenerator(2);
spi_SPI_MacroFunctionGenerator(3);

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


//#define SPI0_Send(_b) SPI1_Send(_b)  

u8 CRC_mc_8BIT(u8 crc, u8 b)
{
    u8 j;
    for (j = 0; j < 8; j++)
    {
      crc <<= 1;
      crc ^= ((((b << j) ^ crc) & 0x80) ? 0x9 : 0);
    }
    return crc;
}

vu8 SPI_CRC8;

u8 CRC_mc_8BIT_block(u8 crc, u8 *buf, u32 size)
{
  while(size)
  {
    crc=CRC_mc_8BIT(crc, *buf++);
    size--;
  }
  return crc;
}


u8 SPI_WriteReadByte(u8 b)
{
  return SPI1_WriteReadByte(b);
}

u8 SPI_WriteReadByte_mc_CRC(u8 b)
{
  //SPI0_Send(b);
  SPI_CRC8=CRC_mc_8BIT(SPI_CRC8, b);
  return SPI_WriteReadByte(b);
}

u8 SPI_WriteByte(u16 b)
{
    u8 i=b>>8;
    u8 c=b;
    if (i==0) i++;
    while (i--) c=SPI_WriteReadByte(b);
    return c;
}


void SPI_ReadBlock(u8 *buf, u32 size)
{
  SPI1_ReadBlock(buf,size);
}

void SPI_WriteBlock(u8 *buf, u32 size)
{
  SPI1_WriteBlock(buf, size);
}

