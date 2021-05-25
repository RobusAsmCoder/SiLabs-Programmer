//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////
#include "SysType.h"
#include "hardware.h"
#include "C2_Interface_CFG.h"
#include "C2_Interface.h"

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/*
u16 c2_clk_delay=0;
void C2iStrobeDoDelay(void)
{
    for (s32 n=0; n<(c2_clk_delay); n++) __nop();
}
*/
void C2iStrobeSendWait(u8 b)
{
    C2_StrobeSendWait(b);
}

/*
void C2iStrobeSendWait(u8 b)
{
//    for (u16 n=0; n<8; n++)
//    {
//      const u8 bb=(b&0x80)?1:0;
//      SI_C2CK( bb );
//      C2iStrobeDoDelay();
//      b<<=1;
//    }
    if (b)
    {
      __disable_irq();
      SI_C2CK(0);
      C2iStrobeDoDelay();
      SI_C2CK(1);
      __enable_irq();
    } else {
      SI_C2CK(0);
      C2iStrobeDoDelay();
    }
   C2iStrobeDoDelay();
  
  
}
*/

void C2iStrobeLength(u16 v)
{
//    if (v<8)
//    {
//      c2_clk_delay = 1<<v;
//    } else {
//      c2_clk_delay = v;
//    }
    u16 bd=SPI_BaudRatePrescaler_2;
    switch(v)
    {
      default:
      case 0: bd=SPI_BaudRatePrescaler_2; break;
      case 1: bd=SPI_BaudRatePrescaler_4; break;
      case 2: bd=SPI_BaudRatePrescaler_8; break;
      case 3: bd=SPI_BaudRatePrescaler_16; break;
      case 4: bd=SPI_BaudRatePrescaler_32; break;
      case 5: bd=SPI_BaudRatePrescaler_64; break;
      case 6: bd=SPI_BaudRatePrescaler_128; break;
      case 7: bd=SPI_BaudRatePrescaler_256; break;
    }
    SPI_CLKSPEED(bd);
}


//////////////////////////////////////////////////////////////////////////////////////

u8 C2_ProcInterface(u8 c2inty, u16 v)
{
    u8 r=0;
    
    switch(c2inty)
    {
      case c2_inty_C2_DATA_GET:
        r = SIrC2D();
        break;
      case c2_inty_C2_DATA_SET:
        SIwC2D( (v?1:0) );
        break;
      case c2_inty_C2_DATA_MODE_IN:
        SI_C2D_IN();
        SIwC2D(1);
        break;
      case c2_inty_C2_DATA_MODE_OUT:
        SI_C2D_OUT();
        break;
      case c2_inty_C2_StrobeSendWait:
        C2iStrobeSendWait(v);
        break;
      case c2_inty_C2_StrobeLength:
        C2iStrobeLength(v);
        break;
    }
    
    return r;
}


