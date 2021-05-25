//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "main_loop.h"

#include "hardware.h"
#include "interface.h"
#include "rb_fifo.h"
#include "rb_util.h"
#include "C2_Interface.h"
#include "C2_Interface_CFG.h"
#include "HD_PARS.h"
#include "core_UID.h"
#include <string.h>


#include "progall.h"












#include "PARS\at_cfg.h"
const u8 at_cfg_cmd[]={ 
    #include "PARS\at_cfg.inc"
};
Thd_pars_def at_cfg_parser;







volatile u16 DEVREVID=0;
volatile u8 BUF[16];
volatile u8 trig=0;

volatile u8 DEVSADR=0;
volatile u16 DEVSDAT=0;
volatile u8 RD_AD=0;
volatile u8 RD_DA=0;


u8 C2_ReadReg(u8 reg)
{
   C2_ADDR_WR(&C2_SILA, reg);
   return C2_DATA_RDB(&C2_SILA);
}
void C2_WriteReg(u8 reg, u8 val)
{
   C2_ADDR_WR(&C2_SILA, reg);
   C2_DATA_WRB(&C2_SILA, val);
} 

/*
// this e-mail's getting long, I'll try to compress these functions
u8 c2_blkread_cmd = C2CMD_REG_READ;
u8 C2_ReadRegBlock(u8 BlockStart, u8 BlockSize, u8 *BufPtr)
{
   C2_WriteReg(intr->FPDAT, c2_blkread_cmd );
   C2_Poll_InBusy();//WaitForInReady();
   C2_Poll_OutReady();//WaitForOutReady();
   if (C2_DATA_RDB() != C2STA_COMMAND_OK)
     return 0;
   C2_DATA_WRB(BlockStart);
   C2_Poll_InBusy();//WaitForInReady();
   C2_DATA_WRB(BlockSize);
   C2_Poll_InBusy();//WaitForInReady();
   do
   {
     C2_Poll_OutReady();//WaitForOutReady();
     *BufPtr++ = C2_DATA_RDB();
   } while (--BlockSize);
   return 1;
} 
*/

void Exper_000(void)
{
      u32 n;
  
      C2_RES(&C2_SILA);
      DEVREVID=C2_GetDevRevID(&C2_SILA);
      C2_Init(&C2_SILA);

      C2_ADDR_WR(&C2_SILA, 0xA9);
      C2_DATA_WRB(&C2_SILA, 0x34);
//      DelayMS(10);
  
      C2_ADDR_WR(&C2_SILA, DEVSADR);
      if  (DEVSDAT & 0xFF00)
      {
        DEVSDAT -= 0x0100;
        C2_DATA_WRB(&C2_SILA, DEVSDAT & 0x00FF);
      }
      
/*      
      C2_ADDR_WR(DEVSADR);
      RD_DA=C2_DATA_RDB();
      C2_ADDR_WR(DEVSADR);
      RD_AD=C2_ADDR_RD();
      C2_ADDR_WR(DEVSADR);
//      C2_DATA_WRB(DEVSDAT);             // Send FLASH block read command
*/
      DelayMS(1);
      
      RD_AD=C2_ADDR_RD(&C2_SILA);
      RD_DA=C2_DATA_RDB(&C2_SILA);

      DelayMS(10);
      C2_ADDR_WR(&C2_SILA, C2_SILA.FPDAT);
      C2_DATA_WRB(&C2_SILA, C2CMD_BLOCK_READ);
      
      for (n=0; n<248; n++) C2_Pool_Get_Status(&C2_SILA);
  
}


void Exper_001(void)
{
//      C2_RES(&C2_SILA);
      C2_Init(&C2_SILA);
/*
      if (C2_ReadRegBlock(DEVSADR, sizeof(BUF), (u8 *)(&BUF)))
      {
        Led0(0);
      } else {
        Led0(1);
      }
*/
      
      C2_ADDR_WR(&C2_SILA, C2CMD_PCL_COPY);
//      RD_AD=C2_ADDR_RD(&C2_SILA);
      RD_DA=C2_DATA_RDB(&C2_SILA);
      
      
/*  
        if (C2_BlockRead((u8*)(BUF), 0x0000, sizeof(BUF)))
        {
          Led0(0);
        } else {
          Led0(1);
        };
  */
      
      
      
      
//      C2_ADDR_WR(C2CMD_REVID);
//      DRVID=C2_DATA_RD(1);
      
      C2_WriteReg(C2CMD_FPCTL,C2FPCTL_RESUME_EXEC);
      DelayMS(100);
      C2_StrobeCLK(&C2_SILA);
      
      
}


typedef enum { PrPoolMode_NOP                         = 0x00000000,
               PrPoolMode_ReadID                      = 0x00000010,
               PrPoolMode_SLIDE_ERASE                 = 0x00000020,
               PrPoolMode_READ_BLOCK                  = 0x00000030,
               PrPoolMode_WRITE_BLOCKSTART            = 0x00000040,
               PrPoolMode_WRITE_BLOCK                 = 0x00000050,
               PrPoolMode_WRITE_HEXSTART              = 0x00000060,
               PrPoolMode_WRITE_HEXLINE               = 0x00000070,
               PrPoolMode_WRITE_HEXLAST               = 0x00000080,
               PrPoolMode_READ_REG                    = 0x00000100,
               PrPoolMode_MAX                         = 0x7FFFFFFF
} enProgPoolModes;

vu32 ProgPoolMode=PrPoolMode_NOP;
#define PoProgNEXT()    ProgPoolMode++
#define PoProgEND()     ProgPoolMode=PrPoolMode_NOP

vu32 ProgPoolVARS[8];
vu8 ProgPoolVAR_BUF[64];
vu8 ProgPoolVAR_BUF_SIZE=0;
vu32 ProgPoolLastATID=0;
vu8 WHX_EchoMode=0;
//D:\ROBUS\Dig-gsm.REB\*.HEX

void showReg(u32 adr, u32 siz)
{
        u32 n;
          Send_RS_String("+TESTREG: ");
          for (n=0; n<siz; n++)
          {
            u8 b=C2_ReadRegB(&C2_SILA, adr+n, C2CMD_REG_READ);
            if ( (((n) & 0xF)==0) && (n!=0) )
            {
              Send_RS_String("\x0D\x0A          ");
            }
            Send_RS_HEX(b,2);
          }
          Send_RS_String("\x0D\x0A");
}

enum{
    pr_md_Nop           = 0,
    pr_md_Reset         = 1,
    pr_md_Init          = 2,
    pr_md_ResetInit     = pr_md_Reset | pr_md_Init,
};

u32 PrStandartID(u8 mode)
{
    u32 res=0xFFFFFFFF;
    u16 cnt=20;
    u32 del=1;
    while ( res==0xFFFFFFFF )
    {
      if (mode & pr_md_Reset)
      {
        u16 delstp=1;
        ProgAllReset(&C2_SILA);
        if (del>=10)
        {
          delstp=10;
          if (del>=100)
          {
            delstp=100;
            if (del>=1000)
            {
              delstp=1000;
            }
          }
        }
        del+=delstp;
        if (del<1000)
        {
          DelayMCS(del);
        } else {
          DelayMS(del/1000);
        }
      }
      if (mode & pr_md_Init)
      {
        ProgAllInit(&C2_SILA);
      }
      res = ProgAllReadID(&C2_SILA);
      if ( C2_SILA.CPU_Family == (u8 *)&CPU_unknown )
      {
        res = 0xFFFFFFFF;
        mode = pr_md_ResetInit;
        if (cnt)
        {
          cnt--;
        } else {
          break;
        }
      } else {
        break;
      }
    }
    return res;
}
u8 PRGslideEraseMode = 0;

void PollProg(void)
{
    static u32 n,m,i,p,adr,siz,mde;
    u32 pp;
    u8 buf[28];
  
    switch (ProgPoolMode)
    {
      case PrPoolMode_NOP:
        break;
      
      case PrPoolMode_ReadID+0:
        PrStandartID(pr_md_ResetInit);
        if (ProgPoolVARS[0]&0x01) ProgAllReset(&C2_SILA); //C2_RES();
        if (ProgPoolVARS[0]&0x02) ProgAllInit(&C2_SILA);
        PoProgNEXT();
        break;
      case PrPoolMode_ReadID+1:
        Send_RS_String("+DEVID:");
        Send_RS_HEX(ProgAllReadID(&C2_SILA),0);
        Send_RS_Byte(',');
        Send_RS_HEX(C2_SILA.FlashAddr,0);
        Send_RS_Byte(',');
        Send_RS_HEX(C2_SILA.FlashSize,0);
        Send_RS_Byte(',');
        Send_RS_HEX(C2_SILA.FlashPageSize,0);
        Send_RS_Byte(',');
        Send_RS_String((char *)(C2_SILA.CPU_Family));
        Send_RS_Byte(',');
        Send_RS_String((char *)(C2_SILA.CPU_Prod));
        Send_RS_String("\x0D\x0A");
        PoProgEND();
        if (0)
        { //test read registers
          C2_RES(&C2_SILA);//ProgAllReset();
          C2_InitX(&C2_SILA, 1,10);
          
          
          showReg(0xE0,32);
          //C2_PageErase(0);
          //C2_DeviceErase();

          u8 b=C2_ReadRegB(&C2_SILA, 0xEF, C2CMD_REG_READ);
          C2_WriteRegB(&C2_SILA, 0xEF, 0, C2CMD_REG_WRITE);
          
          showReg(0xE0,32);
        }
      
      
        break;

      case PrPoolMode_SLIDE_ERASE+0:
        if (PrStandartID(pr_md_ResetInit)!=0xFFFFFFFF)
        {
          n=0;
          PoProgNEXT();
        } else {
          Send_RS_String("+SLDERE:ERROR\x0D\x0A");
          PoProgEND();
        }
        break;
      case PrPoolMode_SLIDE_ERASE+1:
        if (PRGslideEraseMode==0)
        {
          m = 0x01;
        } else {
          m = ProgAllBlankCheckPage(&C2_SILA, n);
        }
        if (m!=0xFF)
        {
          m=ProgAllErasePageWithCheckRepeat(&C2_SILA, n); //m=ProgAllEraseChipWithCheckRepeat(&C2_SILA); //ProgAllErase(&C2_SILA, n, 10, 0);
        } else {
          n = C2_SILA.FlashSize - C2_SILA.FlashPageSize;
        }
//        if (m)
//        {
//          n=i;
//          p=200;
//        }
//        pp=(n*100)/i;
      
        if ( m==0xFF )
        {
          n+= C2_SILA.FlashPageSize;
          p=(n*100)/C2_SILA.FlashSize;
          Send_RS_Byte(0x0D);
          Send_RS_String("+SLDERP:");
          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
          Send_RS_Byte(',');
          Send_RS_DEC(p);
          Send_RS_Byte('%');
          if (n>=C2_SILA.FlashSize)
          {
            Send_RS_String("\x0D\x0A");
            Send_RS_String("+SLDERE:OK\x0D\x0A");
            PoProgEND();
          }
        } else {
          Send_RS_String("\x0D\x0A");
          Send_RS_String("+SLDERE:ERROR\x0D\x0A");
          PoProgEND();
        }
      
      
//        if (p!=pp)
//        {
//          p=pp;
//          Send_RS_Byte(0x0D);
//          Send_RS_String("+SLDERP:");
//          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
//          Send_RS_Byte(',');
//          Send_RS_DEC(p);
//          Send_RS_Byte('%');
//        }
//        if (m)
//        {
//          Send_RS_String("\x0D\x0A");
//          Send_RS_String("+SLDERE:OK\x0D\x0A");
//          PoProgEND();
//          break;
//        } else {
//          n++;
//          //if (n>i)
//          {
//            Send_RS_String("\x0D\x0A");
//            Send_RS_String("+SLDERE:ERROR\x0D\x0A");
//            PoProgEND();
//          }
//        }
        break;
        
      case PrPoolMode_READ_BLOCK+0:
        if (PrStandartID(pr_md_Nop)!=0xFFFFFFFF)
        {
          ProgAllInit(&C2_SILA);
          adr=ProgPoolVARS[0];
          siz=ProgPoolVARS[1];
          mde=ProgPoolVARS[2];
          if (siz==0) siz=C2_SILA.FlashSize;
          if (adr==0) adr=C2_SILA.FlashAddr;
          Send_RS_String("+RBLKS:");
          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
          Send_RS_Byte(',');
          Send_RS_HEX(adr,8);
          Send_RS_Byte(',');
          Send_RS_HEX(siz,8);
          Send_RS_String("\x0D\x0A");
          PoProgNEXT();
        } else {
          Send_RS_String("+RBLKE:ERROR DEVICEID\x0D\x0A");
          PoProgEND();
        }
        break;
      case PrPoolMode_READ_BLOCK+1:
        pp=ProgPoolVARS[3];
        if ((pp>sizeof(buf)) || (pp<4)) pp=sizeof(buf);
        if (pp>siz) pp=siz;
        if (pp)
        {
          if (ProgAllReadBlock(&C2_SILA, buf, adr, pp))
          {
            n=0;
            i=pp;
            //for (m=0; m<i; m++) checkFF &= buf[m];
            if (mde==1)
            {
              while(i)
              {
                if (buf[i-1]!=0xFF) break;
                i--;
              }
              if ((pp-i)<=4) i=pp;
              if (i)
              {
                for(m=0; m<i; m++)
                {
                  if (buf[m]!=0xFF) break;
                  n++;
                }
                i-=n;
              }
              if ((pp-i)<=4)
              {
                n=0;
                i=pp;
              }
            }
            if (i)
            {
              Send_RS_String("+RBLKP:");
              Send_RS_HEX(adr+n,8);
              Send_RS_Byte(',');
              Send_RS_HEX(i,2);
              Send_RS_Byte(',');
              for (m=0; m<i; m++) Send_RS_HEX(buf[n+m],2);
              Send_RS_String("\x0D\x0A");
            }
            adr+=pp;
            siz-=pp;
          } else {
            Send_RS_String("+RBLKP:");
            Send_RS_HEX(adr,8);
            Send_RS_Byte(',');
            Send_RS_String("ERROR");
            Send_RS_String("\x0D\x0A");
            PoProgEND();
          }
          
        } else {
          Send_RS_String("+RBLKE:OK\x0D\x0A");
          PoProgEND();
        }
        break;
        
      case PrPoolMode_WRITE_BLOCKSTART+0:
        Send_RS_String("+WBLKS:");
        if (PrStandartID(pr_md_Nop)!=0xFFFFFFFF)
        {
          ProgAllInit(&C2_SILA);
          adr=ProgPoolVARS[0];
          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
          Send_RS_Byte(',');
          Send_RS_HEX(adr,8);
        } else {
          Send_RS_String("ERROR DEVICEID");
        }
        Send_RS_String("\x0D\x0A");
        PoProgEND();
        break;
      case PrPoolMode_WRITE_BLOCK+0:
        adr=ProgPoolVARS[0];
        Send_RS_String("+WBLKP:");
        Send_RS_HEX(adr,8);
        Send_RS_Byte(',');
        Send_RS_HEX(ProgPoolVAR_BUF_SIZE,0);
        Send_RS_Byte(',');
        if (ProgPoolVAR_BUF_SIZE)
        {
          C2_Init(&C2_SILA);
          if (ProgAllWriteBlock(&C2_SILA, (u8 *)(&ProgPoolVAR_BUF), adr, ProgPoolVAR_BUF_SIZE))
          {
            for (m=0; m<ProgPoolVAR_BUF_SIZE; m++) Send_RS_HEX(ProgPoolVAR_BUF[m],2);
            ProgPoolVARS[0]+=ProgPoolVAR_BUF_SIZE;
          } else {
            Send_RS_String("ERROR WRITE");
          }
        }
        Send_RS_String("\x0D\x0A");
        PoProgEND();
        break;
        
      case PrPoolMode_WRITE_HEXSTART+0:
        Send_RS_String("+WHEXS:");
        if (PrStandartID(pr_md_Nop)!=0xFFFFFFFF)
        {
          ProgAllInit(&C2_SILA);
          Send_RS_HEX(WHX_EchoMode,0);
          Send_RS_Byte(',');
          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
          Send_RS_Byte(',');
          Send_RS_String((char *)(C2_SILA.CPU_Family));
          Send_RS_Byte(',');
          Send_RS_String((char *)(C2_SILA.CPU_Prod));
        } else {
          Send_RS_String("ERROR DEVICEID");
          ProgPoolLastATID=0;
        }
        Send_RS_String("\x0D\x0A");
        PoProgEND();
        break;
        
      case PrPoolMode_WRITE_HEXLINE+0:
        Send_RS_String("+WHEXL:");
        if (ProgPoolVAR_BUF_SIZE)
        {
          if (ProgPoolVAR_BUF_SIZE>=6)
          {
            siz=ProgPoolVAR_BUF[0];
            adr=(ProgPoolVAR_BUF[1]<<8) | (ProgPoolVAR_BUF[2]<<0) | (ProgPoolVAR_BUF[3]<<16);
            if (WHX_EchoMode)
            {
              if (WHX_EchoMode<2)
              {
                Send_RS_HEX(adr,8);
                Send_RS_Byte(',');
                Send_RS_HEX(siz,0);
                Send_RS_Byte(',');
              }
            }
            if (ProgAllWriteBlock(&C2_SILA, (u8 *)(&ProgPoolVAR_BUF[4]), adr, siz))
            {
              if (WHX_EchoMode<2)
              {
                for (m=0; m<siz; m++) Send_RS_HEX(ProgPoolVAR_BUF[4+m],2);
              } else {
                Send_RS_String("+");
              }
            } else {
              Send_RS_String("ERROR WRITE");
            }
          } else {
            Send_RS_String("ERROR LINE SIZE");
          }
        }
        Send_RS_String("\x0D\x0A");
        PoProgEND();
        break;
        
      case PrPoolMode_READ_REG+0:
        if (PrStandartID(pr_md_Nop)!=0xFFFFFFFF)
        {
          ProgAllInit(&C2_SILA);
          adr=ProgPoolVARS[0];
          siz=ProgPoolVARS[1];
          if (siz==0) siz=768;
          if (adr==0) adr=0;
          Send_RS_String("+RREGS:");
          Send_RS_HEX(C2_SILA.Prog_DEVREVID,8);
          Send_RS_Byte(',');
          Send_RS_HEX(adr,8);
          Send_RS_Byte(',');
          Send_RS_HEX(siz,8);
          Send_RS_String("\x0D\x0A");
          PoProgNEXT();
        } else {
          Send_RS_String("+RREGE:ERROR DEVICEID\x0D\x0A");
          PoProgEND();
        }
        break;
      case PrPoolMode_READ_REG+1:
        i=16;
        if (i>siz) i=siz;
        if (i)
        {
          Send_RS_String("+RREGP:");
          Send_RS_HEX(adr,8);
          Send_RS_Byte(',');
          Send_RS_HEX(i,2);
          Send_RS_Byte(',');
          if (ProgAllReadBlock(&C2_SILA, buf, adr+0x00010000, i))
//          if (C2_ReadRegBlock(buf, adr, i, C2CMD_REG_READ))
          {
            for (m=0; m<i; m++) Send_RS_HEX(buf[m],2);
            Send_RS_String("\x0D\x0A");
            adr+=i;
            siz-=i;
          } else {
            Send_RS_String("ERROR\x0D\x0A");
            Send_RS_String("+RREGE:ERROR\x0D\x0A");
            PoProgEND();
          }
        } else {
          Send_RS_String("+RREGE:OK\x0D\x0A");
          PoProgEND();
        }
        break;
          
        
      default:
        ProgPoolMode=PrPoolMode_NOP;
        break;
      
    }


}


void PollCFG(void)
{
    u32 id;
    u32 d,n,m,i;
    char s[64];
    id=HD_ParsFind(&at_cfg_parser);
    at_cfg_parser.ParsGetVar_ERROR=0;
    if (id) Send_RS_String("\x0D\x0A");
    switch (id)
    {
      case 0x00000000:
        break;
      case AT_CFG_KEY0:
        Send_RS_String("+KEY:");
        HD_ParsGetVarSTR(&at_cfg_parser, 0, s);
        Send_RS_String(s);
        Send_RS_String(",");
        HD_ParsGetVarSTR(&at_cfg_parser, 1, s);
        Send_RS_String(s);
        Send_RS_String("\x0D\x0A");
        Send_RS_String("OK\x0D\x0A");
        break;
      case AT_CFG_AT:
        Send_RS_String("OK\x0D\x0A");
        break;
      case AT_CFG_ATE0: //ATE0
        Echo_RS_Disable();
        Send_RS_String("OK\x0D\x0A");
        break;
      case AT_CFG_ATE1: //ATE1
        Echo_RS_Enable();
        Send_RS_String("OK\x0D\x0A");
        break;
      case AT_CFG_READ_ID:
        ProgPoolVARS[0]=0;
        if (HD_ParsGetVarCount(&at_cfg_parser)>0) ProgPoolVARS[0]=HD_ParsGetVarHEX(&at_cfg_parser, 0);
        ProgPoolMode=PrPoolMode_ReadID;
        break;
      case AT_CFG_SLIDEERASE:
        PRGslideEraseMode = 0;
        ProgPoolMode=PrPoolMode_SLIDE_ERASE;
        break;
      case AT_CFG_SLIDEERASEX:
        PRGslideEraseMode = 1;
        ProgPoolMode=PrPoolMode_SLIDE_ERASE;
        break;
      case AT_CFG_READBLOCK:
        ProgPoolVARS[0]=HD_ParsGetVarHEX(&at_cfg_parser, 0);  //addr
        ProgPoolVARS[1]=HD_ParsGetVarHEX(&at_cfg_parser, 1);  //size
        ProgPoolVARS[2]=HD_ParsGetVarHEX(&at_cfg_parser, 2);  //0-All Memory, 1-Skip 0xFF
        ProgPoolVARS[3]=HD_ParsGetVarHEX(&at_cfg_parser, 3);  //Bytes In Line
        switch (HD_ParsGetVarCount(&at_cfg_parser))
        {
          case 0:
            ProgPoolVARS[0]=0;    //addr
          case 1:
            ProgPoolVARS[1]=0;    //size
        }
        if (HD_ParsGetVarSize(&at_cfg_parser,2)==0) ProgPoolVARS[2]=1;
        if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_READ_BLOCK;
        break;
        
      case AT_CFG_WRITEBLOCKSTART:
        ProgPoolVARS[0]=HD_ParsGetVarHEX(&at_cfg_parser, 0);  //addr
        if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_WRITE_BLOCKSTART;
        break;
        
      case AT_CFG_WRITEBLOCK1:
        if (ProgPoolLastATID==AT_CFG_WRITEBLOCKSTART)
        {
          ProgPoolVAR_BUF_SIZE=HD_ParsGetVarHEXBIN(&at_cfg_parser, 0, sizeof(ProgPoolVAR_BUF), (u8 *)(&ProgPoolVAR_BUF));
          if (HD_ParsGetVarSize(&at_cfg_parser,1)==0)
          if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_WRITE_BLOCK;
          id=AT_CFG_WRITEBLOCKSTART;
        } else Send_RS_String("+WBLKP:ERROR FIRST NEED AT+WBLKS\x0D\x0A");
        break;
      case AT_CFG_WRITEBLOCK2:
        if (ProgPoolLastATID==AT_CFG_WRITEBLOCKSTART)
        {
          if (HD_ParsGetVarSize(&at_cfg_parser,0)!=0) ProgPoolVARS[0]=HD_ParsGetVarHEX(&at_cfg_parser, 0);  //addr
          ProgPoolVAR_BUF_SIZE=HD_ParsGetVarHEXBIN(&at_cfg_parser, 1, sizeof(ProgPoolVAR_BUF), (u8 *)(&ProgPoolVAR_BUF));
          if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_WRITE_BLOCK;
          id=AT_CFG_WRITEBLOCKSTART;
        } else Send_RS_String("+WBLKP:ERROR FIRST NEED AT+WBLKS\x0D\x0A");
        break;
      
      case AT_CFG_WRITEHEXSTART:
        WHX_EchoMode=2;
        if (HD_ParsGetVarSize(&at_cfg_parser,0)) WHX_EchoMode=HD_ParsGetVarHEX(&at_cfg_parser, 0);
        ProgPoolMode=PrPoolMode_WRITE_HEXSTART;
        break;
      case AT_CFG_WRITEHEXLINE:
        if (ProgPoolLastATID==AT_CFG_WRITEHEXSTART)
        {
          ProgPoolVAR_BUF_SIZE=HD_ParsGetVarHEXBIN(&at_cfg_parser, 0, sizeof(ProgPoolVAR_BUF), (u8 *)(&ProgPoolVAR_BUF));
          if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_WRITE_HEXLINE;
          id=AT_CFG_WRITEHEXSTART;
        } else  Send_RS_String("+WBLKP:ERROR FIRST NEED AT+WHEX\x0D\x0A");
        break;
      case AT_CFG_WRITEHEXLAST:
//        ProgPoolMode=PrPoolMode_WRITE_HEXLAST;
        Send_RS_String("+WHEXE:OK\x0D\x0A");
        break;
      case AT_CFG_MEASUREVCCPROG:
        Send_RS_String("+MEASVCC:MEASURE,");
        d=PWRPROG_VCC_MEASURE();
        Send_RS_DEC(d);
        Send_RS_Byte(',');
        Send_RS_DEC(d*3300/4096);
        Send_RS_String(",mV\x0D\x0A");
        break;
      case AT_CFG_SETVCCPROG:
        Send_RS_String("+PROGVCC:");
        switch (HD_ParsGetVarHEX(&at_cfg_parser, 0))
        {
          case 0:
            PWRPROG_VCC_OFF();
            Send_RS_String("3.3 OFF");
            break;
          case 1:
            PWRPROG_VCC_LOW();
            Send_RS_String("3.3 LOW");
            break;
          case 2:
            PWRPROG_VCC_HIGH();
            Send_RS_String("3.3 HIGH");
            break;
          case 8:
            SP_VBUS_EN_OFF();
            Send_RS_String("5.0 OFF");
            break;
          case 9:
            SP_VBUS_EN_ON();
            Send_RS_String("5.0 ON");
            break;
          default:
            Send_RS_String("ERROR");
            break;
        }
        Send_RS_String("\x0D\x0A");
        break;
    ///////////
      case AT_CFG_INIT:
        Send_RS_String("+INIT:");
        n=0;
        if (HD_ParsGetVarCount(&at_cfg_parser)>1) n=HD_ParsGetVarHEX(&at_cfg_parser, 1);
        
      
        switch (HD_ParsGetVarHEX(&at_cfg_parser, 0))
        {
          case 0:
            //C2_SILA.Prog_Mode = ProgAllMode_SiLabs_C2;
            //SPI0_Init(1,n);
            Send_RS_String("C2I");
            break;
          default:
            Send_RS_String("ERROR");
            break;
        }
        Send_RS_String("\x0D\x0A");
        break;
    ///////////
      case AT_CFG_RESET:
        ProgAllReset(&C2_SILA);
        //////ProgAllErase
        Send_RS_String("OK\x0D\x0A");
        break;
    ///////////
      case AT_CFG_SPIWRRDSILENT:
        m=0;
        goto spisil0;
      case AT_CFG_SPIWRRD:
        m=1;
spisil0:d=HD_ParsGetVarHEX(&at_cfg_parser, 0);
        n=1;
        if (HD_ParsGetVarCount(&at_cfg_parser)>1) n=HD_ParsGetVarHEX(&at_cfg_parser, 1);
        if (n==0) n=1;
        Send_RS_String("+SPIRD:");
/*      
        i=0x10000;
        while(n)
        {
          SPI0_Send(d);
          if ((i!=0x10000)&&(m)) Send_RS_HEX(i,2);
          i=SPI0_Receive();
          n--;
        }
        if ((i!=0x10000)&&(m)) Send_RS_HEX(i,2);
*/
        while(n--)
        {
          i=SPI_WriteReadByte(d);
          if (m) Send_RS_HEX(i,2);
        }
        Send_RS_String("\x0D\x0A");
        break;

      case AT_CFG_SPINSS:
        Send_RS_String("+SPINSS:");
        d=HD_ParsGetVarHEX(&at_cfg_parser, 0)&1;
        SPI_NSS(d);
        Send_RS_HEX(d,1);
        Send_RS_String("\x0D\x0A");
        break;

      case AT_CFG_READREG:
        ProgPoolVARS[0]=HD_ParsGetVarHEX(&at_cfg_parser, 0);  //addr
        ProgPoolVARS[1]=HD_ParsGetVarHEX(&at_cfg_parser, 1);  //size
        switch (HD_ParsGetVarCount(&at_cfg_parser))
        {
          case 0:
            ProgPoolVARS[0]=0;    //addr
          case 1:
            ProgPoolVARS[1]=0;    //size
        }
        if (at_cfg_parser.ParsGetVar_ERROR==0) ProgPoolMode=PrPoolMode_READ_REG;
        break;

      case AT_CFG_DBG_RUN:
        ProgAll_DBG_CMD(&C2_SILA, enPRAL_DBG_RUN);  
        Send_RS_String("+DBG:RUN\x0D\x0A");
        break;
        
      
      default:
        Send_RS_String("ERROR\x0D\x0A");
        break;
    }
    if (at_cfg_parser.ParsGetVar_ERROR)
    {
      Send_RS_String("ERROR PAR\x0D\x0A");
    }
    if (id) ProgPoolLastATID=id;
    
    switch(ProgPoolLastATID)
    {
      case AT_CFG_WRITEHEXSTART:
        //if (WHX_EchoMode) USART_RS_EchoMode_Mask=0;
        break;
      default:
        //USART_RS_EchoMode_Mask=1;
        break;
    }
    
}



    
void SEND_INFO_CMD(u32 inf, u32 par)
{
    char s[128]="??? uncnown ???";
    u32 d;
//    char p[64]="";
    Send_DBG_String("+INFO:");
    Send_DBG_DEC(inf);
    //strcpy(s,"??? uncnown ???");
    switch(inf)
    {
      case 0:
        strcpy(s,"C2 Programmer");
        break;
      case 1:
        strcpy(s,"Entire Group 2020");
        break;
      case 2:
        d=0;
        switch(par)
        {
          case 0:
            strcpy(s,"ADCCLK = ");
            d=MPU_ClocksStatus.ADCCLK_Frequency;
            break;
          case 1:
            strcpy(s,"HCLK = ");
            d=MPU_ClocksStatus.HCLK_Frequency;
            break;
          case 2:
            strcpy(s,"PCLK1 = ");
            d=MPU_ClocksStatus.PCLK1_Frequency;
            break;
          case 3:
            strcpy(s,"PCLK2 = ");
            d=MPU_ClocksStatus.PCLK2_Frequency;
            break;
          case 4:
            strcpy(s,"SYSCLK = ");
            d=MPU_ClocksStatus.SYSCLK_Frequency;
            break;
        }
        rb_IntToStr(d, rb_istr_mode_DEC, &s[strlen(s)]);
        strcpy(&s[strlen(s)]," Hz ");
        break;
    }
    Send_DBG_Byte(',');
    Send_DBG_String(s);
    Send_DBG_String("\n\r");
  
}

void MainLoopPool(void)
{
      PollCFG();
      PollProg();
}


void MainLoopInit(void)
{

    C2_SILA.C2_INTR = &C2_ProcInterface;
  
  
    HD_ParsInit(&at_cfg_parser, &uart_RS_fifo_in, at_cfg_cmd, hdPARS_mode_line_en, &Send_RS_Byte);
    Send_DBG_Byte(0x0D);
  
    SEND_INFO_CMD(0,0);
    SEND_INFO_CMD(1,0);
//    Send_DBG_String("+INFO:0,C2 Programmer\n\r");
//    Send_DBG_String("+INFO:1,Entire Group 2014\n\r");
    DelayMS(100);
//     Send_DBG_String("+INFO:2,CPU Speed ");
//     Send_DBG_DEC(MPU_ClocksStatus.HCLK_Frequency);

    SEND_INFO_CMD(2,0);
    SEND_INFO_CMD(2,1);
    SEND_INFO_CMD(2,2);
    SEND_INFO_CMD(2,3);
    SEND_INFO_CMD(2,4);
//    Send_DBG_String("+INFO:2,ADCCLK = "); Send_DBG_DEC(MPU_ClocksStatus.ADCCLK_Frequency); Send_DBG_String(" Hz\n\r");
//    Send_DBG_String("+INFO:2,HCLK = ");   Send_DBG_DEC(MPU_ClocksStatus.HCLK_Frequency);   Send_DBG_String(" Hz\n\r");
//    Send_DBG_String("+INFO:2,PCLK1 = ");  Send_DBG_DEC(MPU_ClocksStatus.PCLK1_Frequency);  Send_DBG_String(" Hz\n\r");
//    Send_DBG_String("+INFO:2,PCLK2 = ");  Send_DBG_DEC(MPU_ClocksStatus.PCLK2_Frequency);  Send_DBG_String(" Hz\n\r");
//    Send_DBG_String("+INFO:2,SYSCLK = "); Send_DBG_DEC(MPU_ClocksStatus.SYSCLK_Frequency); Send_DBG_String(" Hz\n\r");
  
  


    DelayMS(100);
    Send_DBG_String("+INFO:3,CPU ID ");
     Send_DBG_HEX(DBGMCU->IDCODE, 8);
    Send_DBG_String("\n\r");
    Send_DBG_String("+INFO:4,DEVICE ID ");
      Send_RS_HEX(CORE_UID->U_ID[0],8);
      Send_RS_Byte(',');
      Send_RS_HEX(CORE_UID->U_ID[1],8);
      Send_RS_Byte(',');
      Send_RS_HEX(CORE_UID->U_ID[2],8);
    Send_DBG_String("\x0D\x0A");
  
  
/*  
    Send_DBG_HEX(SD_GetCmdByte6(0x00,0x00000000), 2);
    Send_DBG_HEX(SD_GetCmdByte6(0x01,0x00000000), 2);
    Send_DBG_String("\n\r");
*/
      Echo_RS_Enable();
    //USART_RS_EchoMode=1;
    //USART_RS_EchoMode_Mask=1;

//    while(1)
//    {
//      GPIOD->BSRR = 0x0000FFFF;
//      DelayMS(500);
//      GPIOD->BSRR = 0xFFFF0000;
//      DelayMS(500);
//    }

    PUP_EN_OFF();
    PWRPROG_VCC_LOW();
    PULL_UP_VCC();
    SP_VBUS_EN_ON();
//    PULL_UP_OFF();
    
/*    
    //SELECT SDCARD
      PWRPROG_VCC_HIGH();
      SPI0_Init(0,5);
//      SPI_CLKSPEED(SPI_BaudRatePrescaler_128);
      PULL_UP_OFF();
//      C2_DAT_PULL_UP();
      SPI_NSS(1);
    ///////////////////
*/  
/*
    //SELECT SDCARD
     PWRPROG_VCC_LOW();
     SPI0_Init(1,8);
     PULL_UP_VCC();
     SPI_NSS(1);
    ///////////////////
*/

    Led0(0);
    Led1(0);


}

