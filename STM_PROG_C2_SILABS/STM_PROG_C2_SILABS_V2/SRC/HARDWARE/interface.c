//////////////////////////////////////////////////////////////////////////////////////
// 
// Middle level interface for STM32F100
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////
#include "SysType.h"
#include "hardware.h"
#include "interface.h"
//#include "core_cm3.h"
#include "cortex_rb_sys.h"
#include "rb_fifo.h"
#include "rb_util.h"
#include <string.h>


#ifndef USART1_IRQChannel
#define USART1_IRQChannel    USART1_IRQn
#endif
#ifndef USART2_IRQChannel
#define USART2_IRQChannel    USART2_IRQn
#endif
#ifndef USART3_IRQChannel
#define USART3_IRQChannel    USART3_IRQn
#endif

///////////////////////////
// UARTS Allocation
///////////////////////////
vu8 Mode_RS_CFG = 0;      //0=RS 1=CFG
///////////////////////////
// USART RS
///////////////////////////

#define IT_Mask                   ((u16)0x001F)  // USART Interrupt Mask
u32 USART_ITConfigGET(USART_TypeDef* USARTx, u16 USART_IT)//, FunctionalState NewState)
{
  u32 usartreg = 0x00, itpos = 0x00, itmask = 0x00;
  u32 usartxbase = 0x00;

  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_CONFIG_IT(USART_IT));
  assert_param(IS_USART_PERIPH_IT(USARTx, USART_IT)); /* The CTS interrupt is not available for UART4 and UART5 */     
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  usartxbase = (*(u32*)&(USARTx));

  /* Get the USART register index */
  usartreg = (((u8)USART_IT) >> 0x05);

  /* Get the interrupt position */
  itpos = USART_IT & IT_Mask;

  itmask = (((u32)0x01) << itpos);
    
  if (usartreg == 0x01) /* The IT is in CR1 register */
  {
    usartxbase += 0x0C;
  }
  else if (usartreg == 0x02) /* The IT is in CR2 register */
  {
    usartxbase += 0x10;
  }
  else /* The IT is in CR3 register */
  {
    usartxbase += 0x14; 
  }
/*  
  if (NewState != DISABLE)
  {
    *(vu32*)usartxbase  |= itmask;
  }
  else
  {
    *(vu32*)usartxbase &= ~itmask;
  }
  */
  return (u32)(*(vu32*)usartxbase  & itmask);
}
  

//void Send_##_name##_Byte(u8 c)                                              \
{                                                                           \
      while ( rb_fifo_free(&uart_##_name##_fifo_out)==0 );                  \
      NVIC_DisableIRQ(_usart##_IRQChannel);                                 \
      rb_fifo_wr(&uart_##_name##_fifo_out, c);                              \
      USART_ITConfig(_usart, USART_IT_TXE, ENABLE);                         \
      NVIC_EnableIRQ(_usart##_IRQChannel);                                  \
}                                                                           \


#define interface_USART_MacroFunctionGenerator(_name,_usart,_insize,_ousize)\
rb_fifo_alloc(uart_##_name##_fifo_in,   _insize);                           \
rb_fifo_alloc(uart_##_name##_fifo_out,  _ousize);                           \
void _usart##uIntDisable(void)                                              \
{                                                                           \
      NVIC_DisableIRQ(_usart##_IRQChannel);                                 \
}                                                                           \
void _usart##uIntSet(void)                                                  \
{                                                                           \
      USART_ITConfig(_usart, USART_IT_TXE, ENABLE);                         \
}                                                                           \
void _usart##uIntEnable(void)                                               \
{                                                                           \
      NVIC_EnableIRQ(_usart##_IRQChannel);                                  \
}                                                                           \
void _usart##_IRQHandler(void) __irq                                        \
{                                                                           \
  if( USART_GetITStatus(_usart, USART_IT_RXNE) != RESET )                   \
  {                                                                         \
    u8 b=USART_ReceiveData(_usart);                                         \
    if ( rb_fifo_free(&uart_##_name##_fifo_in) )                            \
    {                                                                       \
      if ( rb_fifo_wr_blk( &uart_##_name##_fifo_in, &b, 1 ) )               \
      {                                                                     \
      }                                                                     \
    }                                                                       \
    _usart##uIntSet();                                                      \
  }                                                                         \
  if( USART_GetITStatus(_usart, USART_IT_TXE) != RESET )                    \
  {                                                                         \
    if ( rb_fifo_size(&uart_##_name##_fifo_out) )                           \
    {                                                                       \
      USART_SendData(_usart, rb_fifo_rd(&uart_##_name##_fifo_out));         \
    } else {                                                                \
      USART_ITConfig(_usart, USART_IT_TXE, DISABLE);                        \
    }                                                                       \
  }                                                                         \
}                                                                           \
u8 Echo_##_name##_isEnabled(void)                                           \
{                                                                           \
  return uart_##_name##_fifo_in.bfifo!=0;                                   \
}                                                                           \
void Echo_##_name##_Enable(void)                                            \
{                                                                           \
  uart_##_name##_fifo_in.bfifo = (void *)&uart_##_name##_fifo_out;          \
}                                                                           \
void Echo_##_name##_Disable(void)                                           \
{                                                                           \
  uart_##_name##_fifo_in.bfifo = 0;                                         \
}                                                                           \
u16 Read_##_name##_Byte(void)                                               \
{                                                                           \
    if ( rb_fifo_size(&uart_##_name##_fifo_in) )                            \
    {                                                                       \
      return rb_fifo_rd(&uart_##_name##_fifo_in);                           \
    }                                                                       \
    return 0xFFFF;                                                          \
}                                                                           \
void Read_##_name##_Flush(void)                                             \
{                                                                           \
    rb_fifo_clear(&uart_##_name##_fifo_in);                                 \
}                                                                           \
u16 Read_##_name##_Size(void)                                               \
{                                                                           \
    return rb_fifo_size(&uart_##_name##_fifo_in);                           \
}                                                                           \
u16 Read_##_name##_Buf(char *s, u32 siz)                                    \
{                                                                           \
      return rb_fifo_rd_blk(&uart_##_name##_fifo_in, s, siz);               \
}                                                                           \
u16 Send_##_name##_FREE(void)                                               \
{                                                                           \
  return rb_fifo_free(&uart_##_name##_fifo_out);                            \
}                                                                           \
u16 Send_##_name##_isBusy(void)                                             \
{                                                                           \
      if ( rb_fifo_size(&uart_##_name##_fifo_out) ) return 1;               \
      if (USART_GetFlagStatus(_usart, USART_FLAG_TC) == RESET) return 1;    \
      return 0;                                                             \
}                                                                           \
void Send_##_name##_WaitFlush(void)                                         \
{                                                                           \
      while (Send_##_name##_isBusy());                                      \
}                                                                           \
void Send_##_name##_Hold(u16 hold)                                          \
{                                                                           \
      Send_##_name##_WaitFlush();                                           \
      if (hold)                                                             \
      {                                                                     \
        _usart##_GPIO(interface_GPIO_TX_OFF);                               \
        if (hold<50)                                                        \
        {                                                                   \
          while(hold)                                                       \
          {                                                                 \
            Send_##_name##_Byte(0xFF);                                      \
            hold--;                                                         \
          }                                                                 \
          Send_##_name##_WaitFlush();                                       \
        } else {                                                            \
          DelayMCS(hold);                                                   \
        }                                                                   \
        _usart##_GPIO(interface_GPIO_TX_ON);                                \
      }                                                                     \
}                                                                           \
void Send_##_name##_Start(u16 hold)                                         \
{                                                                           \
      Send_##_name##_WaitFlush();                                           \
      _usart##_GPIO(interface_GPIO_DIR_WR);                                 \
      Send_##_name##_Hold(hold);                                            \
}                                                                           \
void Send_##_name##_End(u16 hold)                                           \
{                                                                           \
      Send_##_name##_Hold(hold);                                            \
      _usart##_GPIO(interface_GPIO_DIR_RD);                                 \
}                                                                           \
u16 Send_##_name##_BLK(char *s, u32 siz)                                    \
{                                                                           \
      if ( siz>uart_##_name##_fifo_out.size_mask )                          \
      {                                                                     \
        siz=uart_##_name##_fifo_out.size_mask;                              \
      }                                                                     \
      while ( rb_fifo_free(&uart_##_name##_fifo_out)<siz );                 \
      _usart##uIntDisable();                                                \
      rb_fifo_wr_blk(&uart_##_name##_fifo_out, s, siz);                     \
      _usart##uIntSet();                                                    \
      _usart##uIntEnable();                                                 \
      return siz;                                                           \
}                                                                           \
u16 Send_##_name##_BLKIN(char *s, u32 siz)                                  \
{                                                                           \
      u16 free = rb_fifo_free(&uart_##_name##_fifo_out);                    \
      if (siz>free) siz=free;                                               \
      rb_fifo_wr_blk(&uart_##_name##_fifo_in, s, siz);                      \
      return siz;                                                           \
}                                                                           \
void Send_##_name##_Buf(char *s, u32 siz)                                   \
{                                                                           \
      if (s)                                                                \
      {                                                                     \
        while (siz)                                                         \
        {                                                                   \
          u16 size=Send_##_name##_BLK(s, siz);                              \
          s+=size;                                                          \
          siz-=size;                                                        \
        }                                                                   \
      } else {                                                              \
        if ( (siz>>16)==0 )                                                 \
        {                                                                   \
          Send_##_name##_Start((u16)(siz));                                 \
        } else {                                                            \
          Send_##_name##_End((u16)(siz));                                   \
        }                                                                   \
      }                                                                     \
}                                                                           \
void Send_##_name##_Byte(u8 c)                                              \
{                                                                           \
      Send_##_name##_Buf((char *)&c, 1);                                    \
}                                                                           \
void Send_##_name##_B(u8 c)                                                 \
{                                                                           \
      Send_##_name##_Buf((char *)&c, 1);                                    \
}                                                                           \
void Send_##_name##_W(u16 w)                                                \
{                                                                           \
      Send_##_name##_Buf((char *)&w, 2);                                    \
}                                                                           \
void Send_##_name##_Q(u32 d)                                                \
{                                                                           \
      Send_##_name##_Buf((char *)&d, 3);                                    \
}                                                                           \
void Send_##_name##_D(u32 d)                                                \
{                                                                           \
      Send_##_name##_Buf((char *)&d, 4);                                    \
}                                                                           \
void Send_##_name##_String(char *s)                                         \
{                                                                           \
      Send_##_name##_Buf(s, strlen(s));                                     \
}                                                                           \
void Send_##_name##_HEX(u32 da, u8 Dig)                                     \
{                                                                           \
    rb_IntToProc(da, rb_istr_mode_HEX | Dig, &Send_##_name##_Byte);         \
}                                                                           \
void Send_##_name##_DEC(u32 da)                                             \
{                                                                           \
    rb_IntToProc(da, rb_istr_mode_DEC, &Send_##_name##_Byte);               \
}                                                                           \
void Uart_##_name##_Init(u32 baud, u8 parstop)                              \
{                                                                           \
   NVIC_InitTypeDef NVIC_InitStructure;                                     \
   USART_InitTypeDef USART_InitStructure;                                   \
      _usart##_GPIO(1);                                                     \
      USART_Cmd(_usart, DISABLE);                                           \
      rb_fifo_flush(&uart_##_name##_fifo_in, rb_fifo_size(&uart_##_name##_fifo_in));      \
      Uart_CFG(&USART_InitStructure, baud, parstop);                        \
      USART_Init(_usart, &USART_InitStructure);                             \
      NVIC_InitStructure.NVIC_IRQChannel = _usart##_IRQChannel;             \
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;             \
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                    \
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                       \
      NVIC_Init(&NVIC_InitStructure);                                       \
      USART_ITConfig(_usart, USART_IT_RXNE, ENABLE);                        \
      USART_Cmd(_usart, ENABLE);                                            \
}                                                                           \







// par = 0 - None
//     = 1 - Odd
//     = 2 - Even
//stop = 0 - 0.5
//     = 1 - 1
//     = 2 - 2
//     = 3 - 1.5
void Uart_CFG(USART_InitTypeDef *USART_InitStructure, u32 baud, u8 parstop)
{
      u16 stop = parstop & 0x0F;
      u16 par  = parstop >> 4;
      u16 wlen = USART_WordLength_8b;
  
      switch (par)
      {
        case 1: par=USART_Parity_Odd;  wlen = USART_WordLength_9b; break;
        case 2: par=USART_Parity_Even; wlen = USART_WordLength_9b; break;
        default:par=USART_Parity_No;                               break;
      }
      switch (stop)
      {
        case 0: stop=USART_StopBits_0_5;  break;
        case 2: stop=USART_StopBits_2;    break;
        case 3: stop=USART_StopBits_1_5;  break;
        default:stop=USART_StopBits_1;    break;
      }
  
      USART_InitStructure->USART_BaudRate = baud;
      USART_InitStructure->USART_WordLength = wlen;//USART_WordLength_8b;
      USART_InitStructure->USART_StopBits = stop;//Uart_Get_Stop(parstop & 0x0F);//USART_StopBits_1;
      USART_InitStructure->USART_Parity = par;//Uart_Get_Parity(parstop >> 4);//USART_Parity_No;
      USART_InitStructure->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure->USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
}

/*
void USART1_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
// Включаем тактирование порта А и USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
// TxD (PA9) push-pull With Alternate
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
//Настраиваем UART
  USART1->BRR=0x09C4; //BaudRate 9600
  USART1->CR1 |= USART_CR1_UE; //Разрешаем работу USART1
//  USART1->CR1 |= USART_CR1_TE; //Включаем передатчик
//после настройки усарта настраиваем прервания
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;     //  1_IRQn; //канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //приоритет
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //приоритет субгруппы
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //включаем канал
  NVIC_Init(&NVIC_InitStructure);                             //инициализируем
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    //ENABLE UART RS
}

void USART2_Init(void)
{
}
void USART3_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
// Port B and USART3
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
// TxD (PB10) push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
// RxD (PB11) push-pull
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
//Config UART
  USART3->BRR=0x09C4; //BaudRate 9600
  USART3->CR1 |= USART_CR1_UE; //Разрешаем работу USART1
//  USART1->CR1 |= USART_CR1_TE; //Включаем передатчик
//после настройки усарта настраиваем прервания
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;//  1_IRQn; //канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //приоритет
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//приоритет субгруппы
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //включаем канал
  NVIC_Init(&NVIC_InitStructure); //инициализируем
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    //ENABLE UART RS
}
*/

/*
void Uart_RS_Init(u32 baud, u8 parstop)
{
   USART_InitTypeDef USART_InitStructure;
      USART1_Init();
      rb_fifo_flush(&uart_RS_fifo_in, rb_fifo_size(&uart_RS_fifo_in));
      USART_Cmd(USART1, DISABLE);
      Uart_CFG(&USART_InitStructure, baud, parstop);
      USART_Init(USART1, &USART_InitStructure);
      USART_Cmd(USART1, ENABLE);
}


void Uart_BT_Init(u32 baud, u8 parstop)
{
   USART_InitTypeDef USART_InitStructure;
      //USART2_Init();
      rb_fifo_flush(&uart_BT_fifo_in, rb_fifo_size(&uart_BT_fifo_in));
      USART_Cmd(USART2, DISABLE);
      Uart_CFG(&USART_InitStructure, baud, parstop);
      USART_Init(USART2, &USART_InitStructure);
      USART_Cmd(USART2, ENABLE);
}



void Uart_U3_Init(u32 baud, u8 parstop)
{
   USART_InitTypeDef USART_InitStructure;
      USART3_Init();
      rb_fifo_flush(&uart_U3_fifo_in, rb_fifo_size(&uart_U3_fifo_in));
      USART_Cmd(USART3, DISABLE);
      Uart_CFG(&USART_InitStructure, baud, parstop);
      USART_Init(USART3, &USART_InitStructure);
      USART_Cmd(USART3, ENABLE);
}

*/
//////////////////////////////////////////////////////////////////
hdPin_O_DefineSetup(USART1_TX_ON,   A,  9, hd_gptp_AF_OD,      1);
hdPin_O_DefineSetup(USART1_TX_OFF,  A,  9, hd_gptp_IN_OD,      1);
hdPin_O_DefineSetup(USART1_RX_ON,   A, 10, hd_gptp_IN_PPD,     1);
hdPin_O_DefineSetup(USART1_RX_OFF,  A, 10, hd_gptp_IN_OD,      1);
//hdPin_O_DefineSetup(USART3_DIR,     B,  2, hd_gptp_OUT_PP,     0);    //0-read 1-write
#define USART1_DIR(_v)              //dH_PORT_Write(USART1_DIR,_v)
void USART1_GPIO(u8 v)
{
  switch (v)
  {
    case interface_GPIO_OFF:        //OFF
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
      GpioInit_USART1_TX_OFF();
      GpioInit_USART1_RX_OFF();
      break;
    case interface_GPIO_ON:         //ON
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      GpioInit_USART1_TX_ON();
      GpioInit_USART1_RX_ON();
      break;
    case interface_GPIO_TX_OFF:     //HOLD ENABLE
      GpioInit_USART1_TX_OFF();
      break;
    case interface_GPIO_TX_ON:      //HOLD DISABLE
      GpioInit_USART1_TX_ON();
      break;
    case interface_GPIO_DIR_RD:     //READ ENABLE
      USART1_DIR(0);
      break;
    case interface_GPIO_DIR_WR:     //WRITE ENABLE
      USART1_DIR(1);
      break;
  }
}


/*

//////////////////////////////////////////////////////////////////
hdPin_O_DefineSetup(USART2_TX_ON,      A,  2, hd_gptp_AF_PPU,     1);
hdPin_O_DefineSetup(USART2_TX_OFF,     A,  2, hd_gptp_IN_OD,     1);
hdPin_O_DefineSetup(USART2_RX_ON,      A,  3, hd_gptp_IN_PPU,      1);
hdPin_O_DefineSetup(USART2_RX_OFF,     A,  3, hd_gptp_IN_OD,      1);
void USART2_GPIO(u8 v)
{
  switch (v)
  {
    case interface_GPIO_OFF:      //OFF
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      GpioInit_USART2_TX_OFF();
      GpioInit_USART2_RX_OFF();
      E422EN_OFF();
      break;
    case interface_GPIO_ON:       //ON
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      GpioInit_USART2_TX_ON();
      GpioInit_USART2_RX_ON();
      E422EN_ON();
      break;
  }
}

//////////////////////////////////////////////////////////////////
hdPin_O_DefineSetup(USART3_TX_ON,   B, 10, hd_gptp_AF_OD,      1);
hdPin_O_DefineSetup(USART3_TX_OFF,  B, 10, hd_gptp_IN_OD,      1);
hdPin_O_DefineSetup(USART3_RX_ON,   B, 11, hd_gptp_IN_PPD,      1);
hdPin_O_DefineSetup(USART3_RX_OFF,  B, 11, hd_gptp_IN_OD,      1);
//hdPin_O_DefineSetup(USART3_DIR,     B,  2, hd_gptp_OUT_PP,     0);    //0-read 1-write
#define USART3_DIR(_v)              //dH_PORT_Write(USART3_DIR,_v)
void USART3_GPIO(u8 v)
{
  switch (v)
  {
    case interface_GPIO_OFF:      //OFF
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
      GpioInit_USART3_TX_OFF();
      GpioInit_USART3_RX_OFF();
      //GpioInit_USART3_DIR();
//      E485EN_OFF();
      break;
    case interface_GPIO_ON:       //ON
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      GpioInit_USART3_TX_ON();
      GpioInit_USART3_RX_ON();
      //GpioInit_USART3_DIR();
//      E485EN_ON();
      break;
    case interface_GPIO_TX_OFF:     //HOLD ENABLE
      GpioInit_USART3_TX_OFF();
      break;
    case interface_GPIO_TX_ON:      //HOLD DISABLE
      GpioInit_USART3_TX_ON();
      break;
    case interface_GPIO_DIR_RD:     //READ ENABLE
      USART3_DIR(0);
      break;
    case interface_GPIO_DIR_WR:     //WRITE ENABLE
      USART3_DIR(1);
      break;
  }
}
*/
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



interface_USART_MacroFunctionGenerator(RS,USART1,128,128);







//uartCDC0_fifo_in

void InterfaceInit(void)
{
}


