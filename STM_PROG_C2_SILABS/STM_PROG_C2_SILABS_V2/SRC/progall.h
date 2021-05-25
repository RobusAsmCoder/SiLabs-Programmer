//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HD_PROGALL_H
#define __HD_PROGALL_H

#include "SysType.h"
#include "C2_Interface.h"

typedef enum { enPRAL_DBG_NOP                         = 0x00,
               enPRAL_DBG_RUN                               ,
               enPRAL_DBG_MAX                         = 0xFF
} enPRAL_DBG_Commands;


#include "IDDEV\IDDEV.h"

extern const u8 CPU_unknown[];
  
extern u8 ProgAllReset(tC2_Interface *intr);
extern u8 ProgAll_UpDate_IDDEV_REC(tC2_Interface *intr);
extern u32 ProgAllReadID(tC2_Interface *intr);
extern u8 ProgAllInit(tC2_Interface *intr);
extern u8 ProgAllErase(tC2_Interface *intr, u32 del, u32 muller, u8 type);
extern u8 ProgAllErasePageWithCheckRepeat(tC2_Interface *intr, u32 adr);
extern u8 ProgAllBlankCheckPage(tC2_Interface *intr, u32 adr);
extern u8 ProgAllEraseChipWithCheckRepeat(tC2_Interface *intr);
extern u8 ProgAllReadBlock(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz);
extern u8 ProgAllReadBlockWithCheckRepeat(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz);
extern u8 ProgAllWriteBlock(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz);
extern u8 ProgAllWriteBlockWithCheckRepeat(tC2_Interface *intr, u8 *buf, u32 adr, u32 siz);
extern u8 ProgAll_DBG_CMD(tC2_Interface *intr, u8 cmd);

#endif
