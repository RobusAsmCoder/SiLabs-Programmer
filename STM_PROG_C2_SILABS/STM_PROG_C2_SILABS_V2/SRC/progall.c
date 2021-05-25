//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "rb_util.h"
#include "progall.h"
#include "string.h"

const u8 IDDEV_CONST[]={ 
    #include "IDDEV\IDDEV.inc"
};

//OUT:TRUE = WellDone
u8 ProgAllReset(tC2_Interface *intr)
{
    C2_RES(intr);
    return 1;
}
const u8 CPU_unknown[]="UNKNOWN";

u8 ProgAll_UpDate_IDDEV_REC(tC2_Interface *intr)
{
    u32 i;
    u8 siz;
    u8 prmo=0;
    u8 *cpu_prod=(void *)(0);
    
    intr->FlashAddr = 0;
    intr->FlashSize = 0;
    intr->FlashPageSize = 0;
    intr->CPU_Family = (u8 *)(&CPU_unknown);
    intr->CPU_Prod = (u8 *)(&CPU_unknown);
    intr->FPDAT = 0xB4;
  
    i=0;
    while ((siz=IDDEV_CONST[i])!=0)
    {
      if (siz & 0x80)
      { // cmd
        switch (IDDEV_CONST[i+1])
        {
          case IDDEV_CMD_CPU_TYPE:
            prmo=IDDEV_CONST[i+2];
            //intr->CPU_Family
            cpu_prod = (u8 *)(&IDDEV_CONST[i+3]);
            break;
          default:
            break;
        }
      } else {
        if (prmo==0)
        {
          u32 iddev = IDDEV_CONST[i+ 1] | (IDDEV_CONST[i+ 2]<<8) | (IDDEV_CONST[i+ 3]<<16) | (IDDEV_CONST[i+ 4]<<24);
          //if (iddev==intr->Prog_DEVREVID)
          if ((iddev & 0xFF)==(intr->Prog_DEVREVID & 0xFF))
          {
            u32 pagsize=0;
            intr->FlashAddr = IDDEV_CONST[i+ 5] | (IDDEV_CONST[i+ 6]<<8) | (IDDEV_CONST[i+ 7]<<16) | (IDDEV_CONST[i+ 8]<<24);
            intr->FlashSize = IDDEV_CONST[i+ 9] | (IDDEV_CONST[i+10]<<8) | (IDDEV_CONST[i+11]<<16) | (IDDEV_CONST[i+12]<<24);
            switch (IDDEV_CONST[i+13])
            {
              case ProgAllFlashSize_T256:   pagsize=256;   break;
              case ProgAllFlashSize_T512:   pagsize=512;   break;
              case ProgAllFlashSize_T1024:  pagsize=1024;  break;
            }
            intr->FlashPageSize = pagsize;
            intr->CPU_Family = (u8 *)(&IDDEV_CONST[i+14]);
            intr->FPDAT = IDDEV_CONST[i+14 + strlen((char *)(intr->CPU_Family))+1];
            intr->CPU_Prod = cpu_prod;
            return 1;
          }
        }
      }
      i+=siz & 0x7F;
    }
    return 0;
}



//OUT:Device ID
u32 ProgAllReadID(tC2_Interface *intr)
{
    intr->Prog_DEVREVID=0xFFFFFFFF;
    intr->Prog_DEVREVID=C2_GetDevRevID(intr);
    if (intr->Prog_DEVREVID==0x0000FFFF) intr->Prog_DEVREVID=0xFFFFFFFF;
    ProgAll_UpDate_IDDEV_REC(intr);
    return(intr->Prog_DEVREVID);
}

//OUT:TRUE = WellDone
u8 ProgAllInit(tC2_Interface *intr)
{
    C2_InitX(intr, 1,0);
    return 1;
}


// IN:del = delay tiks before erase
//    muller = for tiks delayer
//    type = 0-erase by page, 1-erase all chip
//OUT:TRUE = WellDone
u8 ProgAllErase(tC2_Interface *intr, u32 del, u32 muller, u8 type)
{
    u32 m;
    u8 res=0;
    u32 adr=0;
        ProgAllReset(intr);
        if (del) for (m=0; m<(del*muller); m++) __asm("nop");
//        ProgAllInit();
        ProgAllReadID(intr);
        C2_InitX(intr, 1,0);//ProgAllInit();
//   ProgAllReset();   
//   C2_ADDR_WR(C2CMD_FPCTL);            // Target the C2 FLASH Programming
//   C2_DATA_WRB(C2FPCTL_INIT_KEY1);     // Write the first key code to enable 
//   C2_DATA_WRB(C2FPCTL_HALT_CORE);     // HALT CODE
//   C2_DATA_WRB(C2FPCTL_INIT_KEY2);     // Write the second key code to enable 
      
        //C2_WriteRegB(0xEF, 0, C2CMD_REG_WRITE);
      
        if (0)
        {
          vu8 bu[8];
          bu[0] = C2_ReadDirect(intr, 0xA7);
          bu[1] = C2_ReadDirect(intr, 0xB2);
          bu[2] = C2_ReadDirect(intr, 0xA9);
          C2_WriteDirect(intr, 0xA7, 0x00);
          C2_WriteDirect(intr, 0xB2, 0x8F);
          C2_WriteDirect(intr, 0xA9, 0x00);
          bu[4] = C2_ReadDirect(intr, 0xA7);
          bu[5] = C2_ReadDirect(intr, 0xB2);
          bu[6] = C2_ReadDirect(intr, 0xA9);
          __nop();
        }
      
        if (type==0)
        {
          adr=0;
          while (adr<intr->FlashSize)
          {
            u8 page=adr/intr->FlashPageSize;
            //ProgAllInit(intr);
            if (C2_PageErase(intr, page))
            //if ( C2_FLASH_PageErase(intr, adr) )
            {
              res++;
            } else {
              res=0;
              break;
            }
            adr+=intr->FlashPageSize;
          }
        } else {
          if (C2_DeviceErase(intr))
          {
            res++;//if (C2_pool_status_val==0x80) return 1;
          }
        }
    return res;
}

//OUT: 0x00 - error interface
//     0x01 - not blank
//     0xFF - blank checked
u8 ProgAllBlankCheckPage(tC2_Interface *intr, u32 adr)
{
    u8 res = 0;
    const u32 pagesize = intr->FlashPageSize;
    if (pagesize)
    {
      u8 page = adr / pagesize;
      adr = page * intr->FlashPageSize;
      u16 w = C2_BlockRead(intr, 0, adr, intr->FlashPageSize);
      if (w)
      {
        if ( (u8)(w>>8)==0xFF )
        {
          res=0xFF;
        }
      } else {
        res = 1;
      }
    }
    return res;
}


//OUT: 0x00 - error interface
//     0x01 - not blank
//     0xFF - blank checked
u8 ProgAllErasePageWithCheck(tC2_Interface *intr, u32 adr)
{
    u8 res = 0;
    const u32 pagesize = intr->FlashPageSize;
    if (pagesize)
    {
      u8 page = adr / pagesize;
      if (C2_PageErase(intr, page))
      {
        res = ProgAllBlankCheckPage(intr, adr);
      } else {
        intr->errorCounter++;
        res = 0;
      }
    }
    return res;
}


//OUT: 0x00 - error interface
//     0x01 - not blank
//     0xFF - blank checked
u8 ProgAllErasePageWithCheckRepeat(tC2_Interface *intr, u32 adr)
{
    u8 res = 0;
    u8 rep = 6;
    while(rep)
    {
      res = ProgAllErasePageWithCheck(intr, adr);
      if ( res==0xFF )
      {
        break;
      } else {
        intr->errorCounter++;
        ProgAllReset(intr);
        ProgAllInit(intr);
        rep--;
      }
    }
    return res;
}

//OUT: 0x00 - error interface
//     0x01 - not blank
//     0xFF - blank checked
u8 ProgAllEraseChipWithCheckRepeat(tC2_Interface *intr)
{
    u8 res = 0;
    const u32 pagesize = intr->FlashPageSize;
    if (pagesize)
    {
      u32 adr=0;
      while(adr<intr->FlashSize)
      {
        res = ProgAllErasePageWithCheckRepeat(intr, adr);
        if ( res==0xFF )
        {
          adr+= pagesize;
        } else {
          break;
        }
      }
    }
    return res;
}


u8 ProgAllReadBlock(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz)
{
    u8 res = 0;
    switch ( (u8)(adr>>24) )
    {
      case 0x00:
        res = C2_BlockRead(intr, buf, adr, siz);
        break;
      case 0x01:
        switch((u8)(adr>>16))
        {
          case 0x00:
            res = C2_ReadRegBlock(intr, buf, adr, siz, C2CMD_REG_READ);
            break;
          case 0x01:
            res = C2_ReadRegBlock(intr, buf, adr, siz, C2CMD_RAM_READ);
            break;
          case 0x02:
            res = C2_DBG_ReadRegBlock(intr, buf, adr, siz);
            break;
        }
        break;
    }
    return res;
}

u8 ProgAllReadBlockWithCheckRepeat(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz)
{
    u8 res = 0;
    u8 rep = 6;
    while (rep)
    {
      res = ProgAllReadBlock(intr, buf, adr, siz);
      if ( res ) break;
      intr->errorCounter++;
      ProgAllReset(intr);
      ProgAllInit(intr);
      rep--;
    }
    return res;
}

u8 ProgAllWriteBlock(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz)
{
    u8 res = 0;
    ProgAllInit(intr);
    res = C2_BlockWrite(intr, buf, adr, siz);
    if (res==0)
    { //error
      __nop();
    }
    return res;
}

u8 ProgAllWriteBlockWithCheckRepeat(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz)
{
    u8 res = 0;
    u8 rep = 6;
    while (rep)
    {
      res = ProgAllWriteBlock(intr, buf, adr, siz);
      if ( res ) break;
      intr->errorCounter++;
      ProgAllReset(intr);
      ProgAllInit(intr);
      rep--;
    }
    return res;
}

u8 ProgAll_DBG_CMD(tC2_Interface *intr, u8 cmd)
{
    switch(cmd)
    {
      case enPRAL_DBG_RUN:
        return C2_DBG_CMD(intr, enC2_DBG_RUN);
      default:
        break;
    }
    return 0 ;
}


