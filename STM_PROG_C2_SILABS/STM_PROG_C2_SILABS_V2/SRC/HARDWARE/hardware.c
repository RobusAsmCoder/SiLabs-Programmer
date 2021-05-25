//////////////////////////////////////////////////////////////////////////////////////
// 
// Low level for STM32F100
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "hardware.h"
#include "STM32_Init.h"
//#include "rb_mmc_cfg.h"
#include "rb_util.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


u8 pwr_invertor = 0;

void PWRPROG_VCC_OFF(void)
{
    PWRPROG_SET_INPD();
    POWER_PROG_EN_OFF();
}
void PWRPROG_VCC_LOW(void)
{
    POWER_PROG_EN_OFF();
    PWRPROG_SET_OUT();
    PWRPROG(1);
}
void PWRPROG_VCC_HIGH(void)
{
    PWRPROG_SET_IN();
    POWER_PROG_EN_ON();
}


u32 PWRPROG_VCC_MEASURE(void)
{
    PWRPROG_VCC_OFF();
//    PWRPROG_SET_IN();
    PUP_EN_OFF();
    
    
    
    PWRPROG_SET_INPD();
//    DelayMS(10);
//    GPIO_InitTypeDef  GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    DelayMS(10);
  


    return MeasureVoltagePWRPROG();//Read_ADC1(9);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////// 

void C2_SendStrobeSPI(u8 b)
{
    SPI1_WriteBlock(&b, 1);
    //DelayMS(10);
    //__nop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////// 
 
void PWM_T1C1(u16 val)
{
    TIM1->CCR1 = val;
}
void PWM_T1C1_BYTE(u8 val)
{
    PWM_T1C1( (TIM1->ARR*val)/255 );
}

void PWM_T4C3(u16 val)
{
    TIM4->CCR3 = val;
}
void PWM_T4C3_BYTE(u8 val)
{
    PWM_T4C3( (TIM4->ARR*val)/255 );
}


void PWM_T2C2(u16 val)
{
    TIM2->CCR2 = val;
}
void PWM_T2C2_BYTE(u8 val)
{
    PWM_T2C2( (TIM2->ARR*val)/255 );
}

void PWM_T5C1(u16 val)
{
    TIM5->CCR1 = val;
}
void PWM_T5C1_BYTE(u8 val)
{
    PWM_T5C1( (TIM5->ARR*val)/255 );
}


void PWM_T3C4(u16 val)
{
    TIM3->CCR4 = val;
}
void PWM_T3C4_BYTE(u8 val)
{
    PWM_T3C4( (TIM3->ARR*val)/255 );
}

void PWM_T8C3(u16 val)
{
    TIM8->CCR3 = val;
}
void PWM_T8C3_BYTE(u8 val)
{
    PWM_T8C3( (TIM8->ARR*val)/255 );
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void TIM3_IRQHandler(void) __irq
{

      //Code HERE
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}






//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


u8 CheckHardVersion(void)
{
      u8 n;
      for (n=0; n<10; n++)
      {
        u8 v=n&1;
        UART_SET_CTS(v);
        DelayMS(1);
        if (HERD_GET_VER()!=v) return 0; //HARD OLD
      }
      return 1;
}

void Setup_PWR(void)
{
  
//  GPIO_InitTypeDef GPIO_InitStructure;

  // GPIOC clock enable
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


  // Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------      
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      // CONFIG 
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//      GPIO_Init(GPIOC, &GPIO_InitStructure);

      
      pwr_invertor=0;
      switch (CheckHardVersion())
      {
        case 0:
          break;
        case 1:
          pwr_invertor=1;
          break;
      }
      UART_SET_CTS(1);
}




//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void SystemInit(void)
{
        stm32_Init();
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



void SelectInitGPIO(void)
{
      GpioInit_Led0();
      GpioInit_Led1();
  
      GpioInit_UART_SET_CTS();
      GpioInit_UART_SET_CD();
      GpioInit_UART_SET_DSR();
      GpioInit_UART_SET_RI();
      GpioInit_UART_GET_DTR();
      GpioInit_UART_GET_RTS();

      GpioInit_HERD_GET_VER();
  
      GpioInit_SET_TMS();
      GpioInit_SET_TDO();
      GpioInit_C2_DAT();

      GpioInit_PUP_EN();
      GpioInit_PEN_TDO();
      GpioInit_PEN_TMS();
      GpioInit_PEN_C2D();
      GpioInit_PEN_C2CK();
      GpioInit_PUP_TDO();
      GpioInit_PUP_TMS();
      GpioInit_PUP_C2D();
      GpioInit_PUP_C2CK();
      
      
      
      GpioInit_POWER_PROG_EN();
      GpioInit_PWRPROG();

//      GpioInit_LEDA();
//      GpioInit_LEDB();
//      GpioInit_KEYA();

//      GpioInit_PAN_LED();
//      GpioInit_PAN_KEY();
//      GpioInit_E422EN();
//      GpioInit_STECS();
//  
//#if isScannerDebug==0
//      GpioInit_RTS();
//      GpioInit_CTS();
//#else
//      GpioInit_DBGCLK();
//      GpioInit_DBGMISO();
//#endif
//  
//      GpioInit_SOUND_PWM();
//      GpioInit_SVOLC_PWM();
//      GpioInit_MMODEN();
//      GpioInit_SB();
//      GpioInit_E422EN();
//  
//      GpioInit_ADCEA0();
//      GpioInit_ADCEA1();
//      GpioInit_ADCEB0();
//      GpioInit_ADCEB1();
//      
//      GpioInit_XPWM_A();
//      GpioInit_XPWM_B();
      
      
}
void Init(void)
{
        RCC_GetClocksFreq((void *)&MPU_ClocksStatus);
//        PCLK1=RCC_ClocksStatus.PCLK1_Frequency;// stm32_GetPCLK1();
//        PCLK2=RCC_ClocksStatus.PCLK2_Frequency;// stm32_GetPCLK2();
        PCLK1_NOP=MPU_ClocksStatus.PCLK1_Frequency/1000000;
//        TIMER0_PER=stm32_TimerGetReload(0);
//        TIMER1_PER=stm32_TimerGetReload(1);
//        TIMER2_PER=stm32_TimerGetReload(2);
//        TIMER3_PER=stm32_TimerGetReload(3);
//        TIMER4_PER=stm32_TimerGetReload(4);

      //First Init SPI1 !!!
        SPI1_Init();
        //SPI1_DMA_Init();  //!!! NOT INIT AT ANY TIME !!!
        SPI1_GPIO_ON();
  
        SelectInitGPIO();
/*
  
        SPI2_Init();
        SPI2_DMA_Init();
        SPI2_GPIO_SOFT_ON();  //SPI2_GPIO_SOFT_ON();
        SPI2_CFG_SOFT(1,1, 6);

        SPI3_Init();
        SPI3_DMA_Init();
        SPI3_GPIO_ON();

//        PWM_Timer_Config(3,4, 100);
//        PWM_T3C4_BYTE(200);

//        PWM_Timer_Config(5,1, 1000);
//        PWM_T5C1_BYTE(200);
        PWM_Timer_Config(2,2, 10);    //Volume Control
        PWM_T2C2_BYTE(0);

        PWM_Timer_Config(1,1, 10);
        PWM_T1C1_BYTE(0);
        PWM_Timer_Config(4,3, 10);
        PWM_T4C3_BYTE(0);

          hdTimerHardwareInit();
          SoundTNE_Init();
//          AYxPERupd(SoundVOL, 1024);   //AYxPERcur(SoundVOL) = 1;
//          AYxPWMupd(SoundVOL, 255);

          AYxFRQupd(SoundTNE, 1000);//AYxPERupD(SoundTNE, 2000);   //AYxPERcur(SoundTNE) = 2000;
          AYxPWMupd(SoundTNE,  128);

*/
        Setup_ADC();
        Setup_PWR();
}





//


