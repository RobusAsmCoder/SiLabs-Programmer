//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "SysType.h"
#include "hardware.h"
#include "interface.h"
#include "core_UID.h"
#include "main_loop.h"

#if (CoCOS_EN==1)
#include "task_main.h"
#else
#endif


void TestPWRPROG(void)
{
    static u32 tmr=0;
  
    if ( WaitMS_CheckContinue(&tmr, 500) )
    {
      
      /*
      C2_StrobeSendWait(0x4F);
      PWRPROG_SET_OUT();
      PWRPROG(0);
      DelayMS(10);
      PWRPROG_SET_IN();
      DelayMS(10);
      u32 d=PWRPROG_VCC_MEASURE(); //MeasureVoltagePWRPROG();
      Send_RS_DEC(d);
      Send_RS_String("\x0D\x0A");
      */
      static u8 v=0;
      //POWER_PROG_EN(v);
      v^=1;
    }
  
}


int main(void)
{
    Init();
  
    Uart_RS_Init(460800,0x01);
  
//    Send_RS_String("\x0D\x0A");
//    Send_RS_String("Enture Group\x0D\x0A");
//    {
//      Send_RS_String("Device ");
//      Send_RS_HEX(CORE_UID->U_ID[0],8);
//      Send_RS_Byte(' ');
//      Send_RS_HEX(CORE_UID->U_ID[1],8);
//      Send_RS_Byte(' ');
//      Send_RS_HEX(CORE_UID->U_ID[2],8);
//      //05DF2D34 34335746 43033515
//      Send_RS_String("\x0D\x0A");
//      //DelayMS(500);
//    }

    
    MainLoopInit();

    
    //Echo_RS_Enable();
    
    
    while(1)
    {
//      static u32 tmr=0;

      
      //TestPWRPROG();
      
      MainLoopPool();

      
    }
    
    
    
}



