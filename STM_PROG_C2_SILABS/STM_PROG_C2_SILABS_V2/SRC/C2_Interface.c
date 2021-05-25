//////////////////////////////////////////////////////////////////////////////////////
// 
// Middle level interface for C2 Silabs Inerface
// 
// By Rob F. / Entire Group
// 
//////////////////////////////////////////////////////////////////////////////////////


#include "SysType.h"
#include "hardware.h"
#include "C2_Interface.h"
#include "C2_Interface_CFG.h"

//////////////////////////////////////////////////////////////////////////////////////
u16 TC2_CLK_FORM=0x01;
u16 TC2_CLK_BAUDRATE=1;
u16 TC2_CLK_RESET=7;
//////////////////////////////////////////////////////////////////////////////////////


tC2_Interface C2_SILA={
  .wait       = 0,
  //.C2_INTR    = (tTC_ProcC2_Interface *)(&C2_ProcInterface),//(tTC_ProcC2_Interface *)(&C2_ProcInterface),
  //.C2_INTR    = &C2_ProcInterface,
  .C2Error    = c2err_OK,
    .Prog_DEVREVID    = 0xFFFFFFFF,
    .errorCounter     = 0,
  .FPDAT              = 0xB4,
};


//eC2Errors C2Error = c2err_OK;

#define returnE(_intr, _err)         return ExitError(_intr, _err)

u8 ExitError(tC2_Interface *intr, eC2Errors err)
{
    u8 res = 0;  //this error
    intr->C2Error = err;
    switch(err)
    {
      case c2err_OK:
        res = 1;
        break;
      default:
        break;
    }
    return res;
}


void C2_StrobeCLK(tC2_Interface *intr)
{
    intr->C2_INTR(c2_inty_C2_StrobeLength, TC2_CLK_BAUDRATE);
    intr->C2_INTR(c2_inty_C2_StrobeSendWait, TC2_CLK_FORM);
}

void C2_RES(tC2_Interface *intr)
{
      intr->C2_INTR(c2_inty_C2_DATA_MODE_OUT, 0);
      intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
      intr->C2_INTR(c2_inty_C2_StrobeLength, TC2_CLK_BAUDRATE);
      intr->C2_INTR(c2_inty_C2_StrobeSendWait, 0x00);
      intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
      DelayMCS(10);
      intr->C2_INTR(c2_inty_C2_DATA_SET, 1);
      intr->C2_INTR(c2_inty_C2_StrobeSendWait, 0x0F);
      intr->C2_INTR(c2_inty_C2_DATA_MODE_IN, 0);
      intr->wait = 0;
      intr->status = 0;
      intr->first = 1;
}

u8 C2_Wait(tC2_Interface *intr)
{
    u32 tmr=0;
    //u16 n=1000;
    u8 b=0;
    intr->C2_INTR(c2_inty_C2_DATA_MODE_IN, 0);
    WaitMS_Start(&tmr, 2);
    do {
      C2_StrobeCLK(intr);
      b = (b<<1) | intr->C2_INTR(c2_inty_C2_DATA_GET, 0);
      if ( (b&3)==0x01 ) break;
    } while ( !WaitMS_Check(&tmr) );
    if ( (b&3)==0x01 )
    {
      return 0;
    } else {
      intr->wait=1;
      return 1;
    }
  
}
///////////////////////////////////////////////
// Read DATA size in bytes
///////////////////////////////////////////////
u32 C2_DATA_RD(tC2_Interface *intr, u8 size)
{
    u8 n;
    u32 dat;

    size--;
    C2_StrobeCLK(intr);     //START
  
    intr->C2_INTR(c2_inty_C2_DATA_MODE_OUT, 0);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
    C2_StrobeCLK(intr);

    intr->C2_INTR(c2_inty_C2_DATA_SET, size & 1);        //00 = Length Byte
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, (size>>1) & 1);
    C2_StrobeCLK(intr);
  
    if (C2_Wait(intr)) return 0xFFFFFFFF;
    
    size=((size & 3)+1)*8;
    u32 bits = 1<<(size-1);
    dat=0;
    for (n=0; n<size; n++)
    {
      dat >>= 1;
      C2_StrobeCLK(intr);
      if (intr->C2_INTR(c2_inty_C2_DATA_GET, 0)) dat |= bits; //0x80;
    }
    C2_StrobeCLK(intr);       //STOP
    return dat;
}

///////////////////////////////////////////////
// Read ADDR
///////////////////////////////////////////////
u8 C2_ADDR_RD(tC2_Interface *intr)
{
    u8 n;
    u8 addr=0;
  
    C2_StrobeCLK(intr);       //START
  
    intr->C2_INTR(c2_inty_C2_DATA_MODE_OUT, 0);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 1);
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_MODE_IN, 0);
  
    for (n=0; n<8; n++)
    {
      addr>>=1;
      C2_StrobeCLK(intr);
      if (intr->C2_INTR(c2_inty_C2_DATA_GET, 0)) addr |= 0x80;
    }
    C2_StrobeCLK(intr);       //STOP
    return addr;
}



///////////////////////////////////////////////
// Write DATA size in bytes
///////////////////////////////////////////////
u32 C2_DATA_WR(tC2_Interface *intr, u32 dat, u8 size)
{
    u8 n;
  
    size--;
    C2_StrobeCLK(intr);       //START
  
    intr->C2_INTR(c2_inty_C2_DATA_MODE_OUT, 0);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 1);
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 0);
    C2_StrobeCLK(intr);


    intr->C2_INTR(c2_inty_C2_DATA_SET, size & 1);        //00 = Length Byte
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, (size>>1) & 1);
    C2_StrobeCLK(intr);
  
    size=((size & 3)+1)*8;
    for (n=0; n<size; n++)
    {
      intr->C2_INTR(c2_inty_C2_DATA_SET, dat & 1);
      C2_StrobeCLK(intr);
      dat >>= 1;
    }

    if (C2_Wait(intr)) return 0xFFFFFFFF;

    C2_StrobeCLK(intr);       //STOP
    return 0;
}


///////////////////////////////////////////////
// Write ADDR
///////////////////////////////////////////////
void C2_ADDR_WR(tC2_Interface *intr, u8 addr)
{
    u8 n;
    C2_StrobeCLK(intr);       //START
  
    intr->C2_INTR(c2_inty_C2_DATA_MODE_OUT, 0);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 1);
    C2_StrobeCLK(intr);
    intr->C2_INTR(c2_inty_C2_DATA_SET, 1);
    C2_StrobeCLK(intr);
  
    for (n=0; n<8; n++)
    {
      intr->C2_INTR(c2_inty_C2_DATA_SET, addr & 1);
      C2_StrobeCLK(intr);
      addr>>=1;
    }
    intr->C2_INTR(c2_inty_C2_DATA_MODE_IN, 0);
    C2_StrobeCLK(intr);       //STOP
  
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

u32 C2_DATA_RDB(tC2_Interface *intr)
{
    return C2_DATA_RD(intr, 1);
}
u32 C2_DATA_RDW(tC2_Interface *intr)
{
    return C2_DATA_RD(intr, 2);
}
u32 C2_DATA_RDT(tC2_Interface *intr)
{
    return C2_DATA_RD(intr, 3);
}
u32 C2_DATA_RDD(tC2_Interface *intr)
{
    return C2_DATA_RD(intr, 4);
}

u32 C2_DATA_WRB(tC2_Interface *intr, u32 dat)
{
    return C2_DATA_WR(intr, dat, 1);
}
u32 C2_DATA_WRW(tC2_Interface *intr, u32 dat)
{
    return C2_DATA_WR(intr, dat, 2);
}
u32 C2_DATA_WRT(tC2_Interface *intr, u32 dat)
{
    return C2_DATA_WR(intr, dat, 3);
}
u32 C2_DATA_WRD(tC2_Interface *intr, u32 dat)
{
    return C2_DATA_WR(intr, dat, 4);
}


void C2_REG_WRB(tC2_Interface *intr, u8 addr, u8 dat)
{
    C2_ADDR_WR(intr, addr);
    C2_DATA_WRB(intr, dat);
}

u8 C2_REG_RDB(tC2_Interface *intr, u8 addr)
{
    C2_ADDR_WR(intr, addr);
    return C2_DATA_RDB(intr);
}


u8 C2_GetDevID(tC2_Interface *intr)
{
   C2_ADDR_WR(intr, C2CMD_DEVICEID);
   return C2_DATA_RD(intr, 1);
}

u8 C2_GetRevID(tC2_Interface *intr)
{
   C2_ADDR_WR(intr, C2CMD_REVID);
   return C2_DATA_RD(intr, 1);
}

u16 C2_GetDevRevID(tC2_Interface *intr)
{
   u16 dat;
   C2_ADDR_WR(intr, C2CMD_DEVICEID);
   dat=C2_DATA_RD(intr, 1);
   C2_ADDR_WR(intr, C2CMD_REVID);
   return dat | (C2_DATA_RD(intr, 1)<<8);
}


vu8 C2_pool_status_val=0;

u8 C2_Pool_Get_Status(tC2_Interface *intr)
{
    C2_pool_status_val=C2_ADDR_RD(intr);
    return C2_pool_status_val;
}

u8 C2_Poll_OutReadyX(tC2_Interface *intr, u32 n)
{
  u32 tmr;
  WaitMS_Start(&tmr, n);
  while( (C2_Pool_Get_Status(intr)&0x01)==0 )
  {
    if ( WaitMS_Check(&tmr) )
    {
      intr->status=1;
      return 1;
    };
  }
  return 0;
}

u8 C2_Poll_OutReady(tC2_Interface *intr)
{
  return C2_Poll_OutReadyX(intr, 100);
}

u8 C2_Poll_InBusyX(tC2_Interface *intr, u32 tms)
{
  //u32 n=10000;
  u32 tmr;
  WaitMS_Start(&tmr, tms);
  while((C2_Pool_Get_Status(intr)&0x02)!=0)
  {
    if ( WaitMS_Check(&tmr) )
    {
      intr->status=1;
      return 1;
    };
  }
  return 0;
}

u8 C2_Poll_InBusy(tC2_Interface *intr)
{
    return C2_Poll_InBusyX(intr, 100);
}





void C2_DBG_CORE_CMD(tC2_Interface *intr, u8 corecmd, u32 del)
{
      if ( corecmd==C2FPCTL_HALT_CORE )
      {
        C2_InitX(intr, 1, del);
      } else {
        C2_ADDR_WR(intr, C2CMD_FPCTL);
        C2_DATA_WRB(intr, C2FPCTL_INIT_KEY1);
        C2_DATA_WRB(intr, C2FPCTL_INIT_KEY2);
        C2_DATA_WRB(intr, corecmd);
      }
}

void C2_InitX(tC2_Interface *intr, u8 ishalt, u32 del)
{
    u8 corecmd = C2FPCTL_RUNNING_CORE;
    switch (ishalt)
    {
      case 1:
        corecmd = C2FPCTL_HALT_CORE;
        break;
      case 2:
        corecmd = C2FPCTL_RESUME_EXEC;
        break;
    }
    C2_ADDR_WR(intr, C2CMD_FPCTL);      // Target the C2 FLASH Programming
                                        // Control register (FPCTL) for C2 Data 
                                        // register accesses
    C2_DATA_WRB(intr, C2FPCTL_INIT_KEY1); // Write the first key code to enable 
                                          // C2 FLASH programming
    C2_DATA_WRB(intr, corecmd); //C2_DATA_WRB(intr, C2FPCTL_HALT_CORE);
  
    C2_DATA_WRB(intr, C2FPCTL_INIT_KEY2);     // Write the second key code to enable 
                                        // C2 FLASH programming
    if (del)
    {
      DelayMCS(del);                      // Delay for at least 20ms to ensure the
                                          // target is ready for C2 FLASH programming
    }
    //C2_DBG_CORE_CMD(intr, corecmd, del);
}

void C2_Init(tC2_Interface *intr)
{
    C2_InitX(intr, 1, 20*1000);
}










u16 C2_BlockReadA(tC2_Interface *intr, u8 *C2_PTR, u32 FLASH_ADDR, u8 NUM_BYTES)
{
   u8 i;                    // Counter
   u8 status;               // FPI status information holder

   C2_ADDR_WR(intr, intr->FPDAT);                  // Select the FLASH Programming Data register 
                                       // for C2 Data register accesses
   C2_DATA_WRB(intr, C2CMD_BLOCK_READ);             // Send FLASH block read command
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     return 0;

   // Check status before starting FLASH access sequence
   if (C2_Poll_OutReady(intr))               // Wait for status information
     return 0;
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     return 0;                         // Exit and indicate error
   
   C2_DATA_WRB(intr, FLASH_ADDR >> 8);        // Send address high byte to FPDAT
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge
   C2_DATA_WRB(intr, FLASH_ADDR & 0x00FF);    // Send address low byte to FPDAT
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge
   C2_DATA_WRB(intr, NUM_BYTES);              // Send block size
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge

   // Check status before reading FLASH block
   C2_Poll_OutReady(intr);                      // Wait for status information
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
      return 0;                        // Exit and indicate error
   u8 andb=0xFF;
   // Read FLASH block
   for (i=0;i<NUM_BYTES;i++)
   {
      C2_Poll_OutReady(intr);                   // Wait for data ready indicator
      u8 b = C2_DATA_RDB(intr);         // Read data from the FPDAT register
      andb&= b;
      if (C2_PTR)
      {
        *C2_PTR++ = b;
      }
   }
   return 1 | (andb<<8);                           // Exit and indicate success
}

u16 C2_BlockRead(tC2_Interface *intr, u8 *buf, u32 adr, u32 size)
{
    u16 w=0xFF01;
    while ( size )
    {
      u32 siz=128;
      if (siz>size) siz = size;
      u16 ww=C2_BlockReadA(intr, buf, adr, siz);
      if (ww)
      {
        w = ((w&ww)&0xFF00) | (ww & 0x00FF);
        if (buf) buf+= siz;
        adr+= siz;
        size-= siz;
      } else {
        w = 0;
        break;
      }
    }
    return w;
}

u8 C2_SetAddrSize(tC2_Interface *intr, u32 addr, u8 siz, u8 c2_blkcmd)
{
   u8 status;               // FPI status information holder

   C2_ADDR_WR(intr, intr->FPDAT);                  // Select the FLASH Programming Data register 
                                       // for C2 Data register accesses
   C2_DATA_WRB(intr, c2_blkcmd);             // Send FLASH block read command
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     return 0;

   // Check status before starting FLASH access sequence
   if (C2_Poll_OutReady(intr))               // Wait for status information
     return 0;
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     return 0;                         // Exit and indicate error
   
   C2_DATA_WRB(intr, addr);               // Send address high byte to FPDAT
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge
   C2_DATA_WRB(intr, siz);              // Send block size
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge

   return 1;
}


// this e-mail's getting long, I'll try to compress these functions
//u8 c2_blkcmd = C2CMD_REG_READ;
u8 C2_ReadRegBlock(tC2_Interface *intr, u8 *C2_PTR, u32 BLK_ADDR, u8 NUM_BYTES, u8 c2_blkcmd)
{
   u8 i;                    // Counter

   if (!C2_SetAddrSize(intr, BLK_ADDR, NUM_BYTES, c2_blkcmd))
     return 0;

   // Read REG block
   for (i=0;i<NUM_BYTES;i++)
   {
      C2_Poll_OutReady(intr);                   // Wait for data ready indicator
      *C2_PTR++ = C2_DATA_RDB(intr);         // Read data from the FPDAT register
   }
   return 1;                           // Exit and indicate success
  
  
} 




u8 C2_ReadRegB(tC2_Interface *intr, u32 addr, u8 c2_blkcmd)
{
   u8 b;
   if (!C2_SetAddrSize(intr, addr, 1, c2_blkcmd))
     return 0;
   C2_Poll_OutReady(intr);                   // Wait for data ready indicator
   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   b=C2_DATA_RDB(intr);         // Read data from the FPDAT register
   return b;
}

u8 C2_WriteRegB(tC2_Interface *intr, u32 addr, u8 b, u8 c2_blkcmd)
{
   if (!C2_SetAddrSize(intr, addr, 1, c2_blkcmd))
     return 0;
   C2_DATA_WRB(intr, b);
   C2_Poll_InBusy(intr);                        // Wait for input acknowledge
   return 1;
}

u8 C2_WriteDirect(tC2_Interface *intr, u8 ad, u8 b)
{
    return C2_WriteRegB(intr, ad, b, C2CMD_REG_WRITE);
}

u8 C2_ReadDirect(tC2_Interface *intr, u8 ad)
{
    return C2_ReadRegB(intr, ad, C2CMD_REG_READ);
}




u8 C2_BlockWriteA(tC2_Interface *intr, u8 *C2_PTR, u32 FLASH_ADDR, u8 NUM_BYTES)
{
   u8 i;                    // Counter
   u8 status;               // FPI status information holder

   C2_ADDR_WR(intr, intr->FPDAT);                  // Select the FLASH Programming Data register 
                                       // for C2 Data register accesses
   C2_DATA_WRB(intr, C2CMD_BLOCK_WRITE);            // Send FLASH block write command
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_SEND_FLASH_BLOCK_WRITE);

   // Check status before starting FLASH access sequence
   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING);

   C2_DATA_WRB(intr, FLASH_ADDR >> 8);        // Send address high byte to FPDAT
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_FPDAT_H);
   C2_DATA_WRB(intr, FLASH_ADDR & 0x00FF);    // Send address low byte to FPDAT
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_FPDAT_L);
   C2_DATA_WRB(intr, NUM_BYTES);              // Send block size
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_BLOCK_SIZE);

   // Check status before writing FLASH block
   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);

   // Write FLASH block
   for (i=0;i<NUM_BYTES;i++)
   {
      u8 b = 0xFF;
      if ( C2_PTR )
      {
        b = *C2_PTR++;
      }
      C2_DATA_WRB( intr, b );           // Write data to the FPDAT register
      if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
        returnE(intr, c2err_FPDAT_REG);
   }   

   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
//   status = C2_DATA_RDB();               // Read FLASH programming interface status
//   if (status != C2STA_COMMAND_OK)
//      return 0;                        // Exit and indicate error
   returnE(intr, c2err_OK);                           // Exit and indicate success
}

u8 C2_BlockWrite(tC2_Interface *intr, u8 *buf, u32 adr, u32 size)
{
    u8 res = 1;
    while ( size )
    {
      u32 siz=128;
      if (siz>size) siz = size;
      res = C2_BlockWriteA(intr, buf, adr, siz);
      if (res)
      {
        if (buf) buf+= siz;
        adr+= siz;
        size-= siz;
      } else {
        break;
      }
    }
    return res;
}

//-----------------------------------------------------------------------------
// C2_WriteCommand
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : command
//
// This routine writes a C2 Flash command (WriteDR + INBUSY polling).
//
//-----------------------------------------------------------------------------
u8 C2_WriteCommand(tC2_Interface *intr, u8 command, u16 C2_poll_inbusy_timeout_ms)
{
    u8 result = c2err_OK;
    // Send command                 C2_DATA_WRB
//    if (NO_ERROR != (result = C2_WriteDR(command, C2_WRITE_DR_TIMEOUT_US)))
//      return result;
    C2_DATA_WRB(intr, command);
    // verify acceptance
    if (C2_Poll_InBusyX(intr, C2_poll_inbusy_timeout_ms))                  // Wait for input acknowledge
    {
      return c2err_CMD;
    }
    return result;
}

#define C2_POLL_INBUSY_TIMEOUT_MS           100
#define C2_POLL_OUTREADY_TIMEOUT_MS         100
#define FLASH_PAGE_ERASE_TIMEOUT_MS         40
#define C2_READ_DR_TIMEOUT_US               20000

  
#define NO_ERROR                            c2err_OK
#define ADDRESS_OUT_OF_RANGE                c2err_ADDRESS_OUT_OF_RANGE
#define PAGE_ERASE_TIMEOUT                  c2err_PAGE_ERASE_TIMEOUT
#define COMMAND_OK                          c2err_COMMAND_OK
#define BAD_DEBUG_COMMAND                   c2err_BAD_DEBUG_COMMAND

#define C2_FPDAT_RETURN_INVALID_COMMAND     0x00
#define C2_FPDAT_RETURN_COMMAND_FAILED      0x02
#define C2_FPDAT_RETURN_COMMAND_OK          0x0D



void C2_WriteAR(tC2_Interface *intr, u8 adr)
{
    C2_ADDR_WR(intr, adr);
}

u8 C2_DR;
u8 C2_ReadDR(tC2_Interface *intr, u16 timeout_us)
{
    C2_DR = C2_DATA_RDB(intr);
    return NO_ERROR;
}



//-----------------------------------------------------------------------------
// C2_ReadResponse
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters   : None
//
// This routine reads a C2 Flash command response (Outready polling + ReadDR).
//
//-----------------------------------------------------------------------------
u8 C2_ReadResponse(tC2_Interface *intr, u16 C2_poll_outready_timeout_ms)
{
    u8 result;
    // check for response
    if (NO_ERROR == (result = C2_Poll_OutReadyX(intr, C2_poll_outready_timeout_ms))
        // read status
        && NO_ERROR == (result = C2_ReadDR(intr, C2_READ_DR_TIMEOUT_US))
        && C2_DR != COMMAND_OK
      )
    {
      result = ((C2_DR == C2_FPDAT_RETURN_INVALID_COMMAND) ? BAD_DEBUG_COMMAND : C2_DR);
    }
    return result;
}






u8 C2_PageErase(tC2_Interface *intr, u8 page)
{
   u8 status;               // FPI status information holder
  
   C2_ADDR_WR(intr, intr->FPDAT);                   // Select the FLASH Programming Data register 
                                              // for C2 Data register accesses
   C2_DATA_WRB(intr, C2CMD_PAGE_ERASE);             // Send Page Erase command
   if (C2_Poll_InBusy(intr))                      // Wait for input acknowledge
      returnE(intr, c2err_SEND_PAGE_ERASE);

   // Check status before starting FLASH access sequence
   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);

   C2_DATA_WRB(intr, page);                     // Write Page sequence command 1
   if (C2_Poll_InBusy(intr))                  // Wait for input acknowledge
     returnE(intr, c2err_FPDAT_REG);

   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);
   
//   C2_DATA_WRB(intr, page*0);                     // Dummy write to initiate erase
//   if (C2_Poll_InBusy(intr))                  // Wait for input acknowledge
//     returnE(intr, c2err_FPDAT_REG);
   if ( C2_WriteCommand(intr, 0, 100) )
     returnE(intr, c2err_FPDAT_REG);
   
   
   if (C2_Poll_OutReadyX(intr, 200))          // Wait for status information
     returnE(intr, c2err_ERASE_ERROR);
//   if (C2_Poll_OutReady(intr))               // Wait for status information
//     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);
   
   returnE(intr, c2err_OK);                           // Exit and indicate success
}





//-----------------------------------------------------------------------------
// C2_FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : Error code
// Parameters	: None
//
// This function erases the FLASH page containing <addr>.
// Assumes that FPDAT has been determined and the device is in the HALT
// state.
//
//-----------------------------------------------------------------------------
#include "progall.h"
u8 C2_FLASH_PageErase (tC2_Interface *intr, u32 addr)
{

#define C2_FPDAT_PAGE_ERASE                 C2CMD_PAGE_ERASE
  
    u8 page_number;
    //u8 psbank_val;
    u8 return_value = c2err_OK;
    //u16 C2_addr;
    //C2_addr = (u16) addr;

    // Set up for command writes
    C2_WriteAR(intr, intr->FPDAT);

    // Send Page Erase command
    if (NO_ERROR != (return_value = C2_WriteCommand(intr, C2_FPDAT_PAGE_ERASE, C2_POLL_INBUSY_TIMEOUT_MS)))
      return return_value;

    // check status
    if (NO_ERROR != (return_value = C2_ReadResponse(intr, C2_POLL_OUTREADY_TIMEOUT_MS)))
      return return_value;

    // calculate page number
    page_number =  addr/intr->FlashPageSize; //(u32) C2_addr.U16 / (u32) KnownFamilies[FamilyNumber].PAGE_SIZE;

    if (page_number > (
      (u32) intr->FlashSize /
      (u32) intr->FlashPageSize)
      )
      return ADDRESS_OUT_OF_RANGE;

    // now send page number
    if (NO_ERROR != (return_value = C2_WriteCommand (intr, page_number, C2_POLL_INBUSY_TIMEOUT_MS)))
      return return_value;

    // check status
    if (NO_ERROR != (return_value = C2_ReadResponse (intr, C2_POLL_OUTREADY_TIMEOUT_MS)))
      return ADDRESS_OUT_OF_RANGE;

    if (NO_ERROR != (return_value = C2_WriteCommand (intr, 0x00, C2_POLL_INBUSY_TIMEOUT_MS)))
      return return_value;

    // check status
    if (NO_ERROR != (return_value = C2_ReadResponse (intr, FLASH_PAGE_ERASE_TIMEOUT_MS)))
      return PAGE_ERASE_TIMEOUT;

    return return_value;
}





u8 C2_DeviceErase(tC2_Interface *intr)
{
   u8 status;               // FPI status information holder

   C2_ADDR_WR(intr, intr->FPDAT);                   // Select the FLASH Programming Data register 
                                              // for C2 Data register accesses
   C2_DATA_WRB(intr, C2CMD_DEVICE_ERASE);           // Send Device Erase command
   if (C2_Poll_InBusy(intr))                        // Wait for input acknowledge
      returnE(intr, c2err_SEND_PAGE_ERASE);

   // Check status before starting FLASH access sequence
   if (C2_Poll_OutReady(intr))               // Wait for status information
     returnE(intr, c2err_WAIT_STATUS_INFORMATION);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);

   // Send a three-byte arming sequence to enable the device erase. If the sequence
   // is not received correctly, the command will be ignored.
   // Sequence: 0xDE, 0xAD, 0xA5.
   C2_DATA_WRB(intr, 0xDE);                   // Arming sequence command 1
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_ARMING_DE);

   C2_DATA_WRB(intr, 0xAD);                   // Arming sequence command 2
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_ARMING_AD);

   C2_DATA_WRB(intr, 0xA5);                   // Arming sequence command 3
   if (C2_Poll_InBusy(intr))                 // Wait for input acknowledge
     returnE(intr, c2err_ARMING_A5);


//    DelayMS(100);
   if (C2_Poll_OutReadyX(intr, 10000))                      // Wait for status information
     returnE(intr, c2err_ERASE_ERROR);
   status = C2_DATA_RDB(intr);               // Read FLASH programming interface status
   if (status != C2STA_COMMAND_OK)
     returnE(intr, c2err_FLASH_PROGRAMMING_INTERFACE);
   returnE(intr, c2err_OK);                           // Exit and indicate success
}




u8 C2_DBG_ReadRegBlock(tC2_Interface *intr, u8 *C2_PTR, u8 BLK_ADDR, u8 NUM_BYTES)
{
   while (NUM_BYTES--) *C2_PTR++ = C2_REG_RDB(intr, BLK_ADDR++);
   return 1;
}



u32 C2_DBG_CMD_PAR(tC2_Interface *intr, u8 cm, u32 par)
{
    switch(cm)
    {
      case enC2_DBG_NOP:
        return par;
      case enC2_DBG_RUN:
        C2_REG_WRB(intr, C2CMD_FPCTL, C2FPCTL_RESUME_EXEC);
        return 1;
      case enC2_DBG_BRKP:
        switch (par>>16)
        {
          case 0:
            C2_REG_WRB(intr, C2CMD_BRKP0L, par>>0);
            C2_REG_WRB(intr, C2CMD_BRKP0H, par>>8);
            return 1;
          case 1:
            C2_REG_WRB(intr, C2CMD_BRKP1L, par>>0);
            C2_REG_WRB(intr, C2CMD_BRKP1H, par>>8);
            return 1;
          case 2:
            C2_REG_WRB(intr, C2CMD_BRKP2L, par>>0);
            C2_REG_WRB(intr, C2CMD_BRKP2H, par>>8);
            return 1;
          case 3:
            C2_REG_WRB(intr, C2CMD_BRKP3L, par>>0);
            C2_REG_WRB(intr, C2CMD_BRKP3H, par>>8);
            return 1;
        }
        break;
      case enC2_DBG_BRKP0:
        return C2_DBG_CMD_PAR(intr, enC2_DBG_BRKP, (par & 0x0000FFFF) | 0x00000000);
      case enC2_DBG_BRKP1:
        return C2_DBG_CMD_PAR(intr, enC2_DBG_BRKP, (par & 0x0000FFFF) | 0x00010000);
      case enC2_DBG_BRKP2:
        return C2_DBG_CMD_PAR(intr, enC2_DBG_BRKP, (par & 0x0000FFFF) | 0x00020000);
      case enC2_DBG_BRKP3:
        return C2_DBG_CMD_PAR(intr, enC2_DBG_BRKP, (par & 0x0000FFFF) | 0x00030000);
    }
    return 0;
}

u32 C2_DBG_CMD(tC2_Interface *intr, u8 cm)
{
    return C2_DBG_CMD_PAR(intr, cm, 0);
}






