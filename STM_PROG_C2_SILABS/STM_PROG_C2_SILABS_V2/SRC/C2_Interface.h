//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __C2_INTERFACE_H
#define __C2_INTERFACE_H

#include "SysType.h"



typedef enum { enC2_DBG_NOP                         = 0x00,
               enC2_DBG_RUN                         = 0x01,
               enC2_DBG_BRKP                        = 0x02,
               enC2_DBG_BRKP0                       = 0x03,
               enC2_DBG_BRKP1                       = 0x04,
               enC2_DBG_BRKP2                       = 0x05,
               enC2_DBG_BRKP3                       = 0x06,
               enC2_DBG_MAX                         = 0xFF
} enC2_DBG_Commands;




#define C2CMD_DEVICEID 0x00 // fixed device ID value
#define C2CMD_REVID 0x01 // fixed revision ID (I've only seen 0x00)
#define C2CMD_FPCTL 0x02 // Flash Programming Control (also for debugging)
#define C2CMD_PCL_COPY 0x20 // copy of Program Counter, low byte, while debugging
#define C2CMD_PCH_COPY 0x21 // copy of Program Counter, high byte, while debugging
#define C2CMD_PSW_COPY 0x23 // copy of PSW while debugging
#define C2CMD_R0_COPY 0x24 // copy of R0 (bank 0) while debugging
#define C2CMD_R1_COPY 0x25 // copy of R1 (bank 0) while debugging
#define C2CMD_R2_COPY 0x26 // copy of R2 (bank 0) while debugging
#define C2CMD_XRAMD 0x84 // XRAM Data (address auto-increments)
#define C2CMD_BRKP0L 0x85 // Breakpoint 0, low byte
#define C2CMD_BRKP0H 0x86 // Breakpoint 0, high byte
#define C2CMD_BRKP1L 0xAB // Breakpoint 1, low byte
#define C2CMD_BRKP1H 0xAC // Breakpoint 1, high byte
#define C2CMD_XRAMAL 0xAD // XRAM Address, low byte (address auto-increments)
#define C2CMD_FPDAT 0xB4 // 0xAD //Flash Programming Data (also for debugging)
#define C2CMD_XRAMAH 0xC7 // XRAM Address, high byte (address auto-increments)
#define C2CMD_BRKP2L 0xCE // Breakpoint 2, low byte
#define C2CMD_BRKP2H 0xCF // Breakpoint 2, high byte
#define C2CMD_BRKP3L 0xD2 // Breakpoint 3, low byte
#define C2CMD_BRKP3H 0xD3 // Breakpoint 3, high byte

// FPCTL codes
#define C2FPCTL_INIT_KEY1 0x02 // first key
#define C2FPCTL_INIT_KEY2 0x01 // second key
#define C2FPCTL_HALT_CORE 0x04 // halt CPU code execution
#define C2FPCTL_RESUME_EXEC 0x08 // resume code execution

#define C2FPCTL_RUNNING_CORE      0x00
#define C2FPCTL_RESET             0x02
#define C2FPCTL_RESET_CORE        0x04

// FPDAT commands
#define C2CMD_DEVICE_ERASE 0x03
#define C2CMD_BLOCK_READ 0x06
#define C2CMD_BLOCK_WRITE 0x07
#define C2CMD_PAGE_ERASE 0x08
#define C2CMD_REG_READ 0x09
#define C2CMD_REG_WRITE 0x0A
#define C2CMD_RAM_READ 0x0B
#define C2CMD_RAM_WRITE 0x0C

// C2 status return codes
#define C2STA_INVALID_COMMAND   0x00
#define C2STA_COMMAND_FAILED    0x02
#define C2STA_COMMAND_OK        0x0D

enum {
    c2_inty_C2_DATA_GET        = 0,
    c2_inty_C2_DATA_SET           ,
    c2_inty_C2_DATA_MODE_IN       ,
    c2_inty_C2_DATA_MODE_OUT      ,
    c2_inty_C2_StrobeSendWait     ,
    c2_inty_C2_StrobeLength       ,
};


typedef u8(tTC_ProcC2_Interface)(u8, u16);


typedef enum {
  c2err_OK                                = 0 ,
  c2err_UNCNOWN                               ,
  c2err_SEND_FLASH_BLOCK_WRITE                ,// Send FLASH block write command
  c2err_WAIT_STATUS_INFORMATION               ,// Wait for status information
  c2err_FLASH_PROGRAMMING                     ,// Read FLASH programming interface status
  c2err_FPDAT_H                               ,// FPDAT HIGH
  c2err_FPDAT_L                               ,// FPDAT LOW
  c2err_BLOCK_SIZE                            ,// Block Size
  c2err_FLASH_PROGRAMMING_INTERFACE           ,// Read FLASH programming interface status
  c2err_FPDAT_REG                             ,// Write data to the FPDAT register
  c2err_SEND_PAGE_ERASE                       ,// Send Page Erase command
  c2err_SEND_DEVICE_ERASE                     ,// Send Device Erase command
  c2err_ARMING_DE                             ,// Device Erase ARMING DE
  c2err_ARMING_AD                             ,// Device Erase ARMING AD
  c2err_ARMING_A5                             ,// Device Erase ARMING A5
  c2err_ERASE_ERROR                           ,// Erase Error Page or Device
  c2err_CMD                                   ,
  c2err_ADDRESS_OUT_OF_RANGE                  ,
  c2err_PAGE_ERASE_TIMEOUT                    ,
  c2err_COMMAND_OK                            ,
  c2err_BAD_DEBUG_COMMAND                     ,
}eC2Errors;


//extern eC2Errors C2Error;

typedef struct {
   u16                           wait      :1;
   u16                           status    :1;
   u16                           first     :1;
   u16                                     :13;
  tTC_ProcC2_Interface          *C2_INTR;
  eC2Errors                      C2Error;
    u32 Prog_DEVREVID;
    u32 FlashAddr;
    u32 FlashSize;
    u32 FlashPageSize;
    u8  *CPU_Family;
    u8  *CPU_Prod;
    u8  FPDAT;
    u32 errorCounter;
}tC2_Interface;

extern tC2_Interface C2_SILA; //tC2_Interface C2_INTR;  //extern TC2_error C2_error;


extern void C2_StrobeCLK(tC2_Interface *intr);
extern void C2_RES(tC2_Interface *intr);
//extern u8 C2_Wait(void);

extern u32 C2_DATA_RD(tC2_Interface *intr, u8 size);
extern u32 C2_DATA_RDB(tC2_Interface *intr);
extern u32 C2_DATA_RDW(tC2_Interface *intr);
extern u32 C2_DATA_RDT(tC2_Interface *intr);
extern u32 C2_DATA_RDD(tC2_Interface *intr);

extern u32 C2_DATA_WR(tC2_Interface *intr, u32 dat, u8 size);
extern u32 C2_DATA_WRB(tC2_Interface *intr, u32 dat);
extern u32 C2_DATA_WRW(tC2_Interface *intr, u32 dat);
extern u32 C2_DATA_WRT(tC2_Interface *intr, u32 dat);
extern u32 C2_DATA_WRD(tC2_Interface *intr, u32 dat);

extern u8 C2_ADDR_RD(tC2_Interface *intr);
extern void C2_ADDR_WR(tC2_Interface *intr, u8 addr);
extern void C2_REG_WRB(tC2_Interface *intr, u8 addr, u8 dat);
extern u8 C2_REG_RDB(tC2_Interface *intr, u8 addr);

extern u8 C2_GetDevID(tC2_Interface *intr);
extern u8 C2_GetRevID(tC2_Interface *intr);
extern u16 C2_GetDevRevID(tC2_Interface *intr);

extern void C2_DBG_CORE_CMD(tC2_Interface *intr, u8 corecmd, u32 del);
extern void C2_InitX(tC2_Interface *intr, u8 ishalt,u32 del);
extern void C2_Init(tC2_Interface *intr);

extern vu8 C2_pool_status_val;
extern u8 C2_Pool_Get_Status(tC2_Interface *intr);
extern u8 C2_Poll_OutReady(tC2_Interface *intr);
extern u8 C2_Poll_InBusy(tC2_Interface *intr);

extern u16 C2_BlockRead(tC2_Interface *intr, u8 *buf, u32 adr, u32 size);
extern u8 C2_BlockWrite(tC2_Interface *intr, u8 *buf, u32 adr, u32 size);
extern u8 C2_ReadRegBlock(tC2_Interface *intr, u8 *C2_PTR, u32 BLK_ADDR, u8 NUM_BYTES, u8 c2_blkcmd);
extern u8 C2_ReadRegB(tC2_Interface *intr, u32 addr, u8 c2_blkcmd);
extern u8 C2_WriteRegB(tC2_Interface *intr, u32 addr, u8 b, u8 c2_blkcmd);
extern u8 C2_WriteDirect(tC2_Interface *intr, u8 ad, u8 b);
extern u8 C2_ReadDirect(tC2_Interface *intr, u8 ad);

extern u8 C2_PageErase(tC2_Interface *intr, u8 page);
extern u8 C2_FLASH_PageErase(tC2_Interface *intr, u32 addr);
extern u8 C2_DeviceErase(tC2_Interface *intr);

extern u8 C2_DBG_ReadRegBlock(tC2_Interface *intr, u8 *C2_PTR, u8 BLK_ADDR, u8 NUM_BYTES);

extern u32 C2_DBG_CMD_PAR(tC2_Interface *intr, u8 cm, u32 par);
extern u32 C2_DBG_CMD(tC2_Interface *intr, u8 cm);


#endif

