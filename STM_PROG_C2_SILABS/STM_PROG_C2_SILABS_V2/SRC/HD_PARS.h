//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __HD_PARS_H
#define __HD_PARS_H

//#include "stm32f10x_lib.h"
#include "SysType.h"
#include "rb_fifo.h"

#define hdPARS_mode_byte          0x0000
#define hdPARS_mode_line_en       0x0010
#define hdPARS_mode_process_VAR   0x1000

#define hdPARS_flag_CF            0x01
#define hdPARS_flag_send_delay    0x08
#define hdPARS_flag_ZF            0x40

#define PARS_VARS   16
#define SCRPT_VARS   8

typedef struct {
  u8        *pacmd;
  Trb_fifo  *fifo;
	void      (*SendByte)(u8);
  u16       domode;
  u16       par_ID;
  u16       answer_id;
  struct {
    u32     delay_timer;
    u32     send_timer;
    u32     var[SCRPT_VARS];
    u16     send_time_before;
    u16     send_time_max;
    u16     send_rep_ptr;
    u8      send_rep_max;
    u8      send_rep;
    u16     ptr;
    u16     error;
    u8      flag;
  } scr;
  //u16       scrptptr;
  u8        varp[PARS_VARS];  //pos for var
  u8        vars[PARS_VARS];  //size var
  u8        varc;
  u8        ParsGetVar_ERROR;
  u8        par_SIZ;
  u8        mode;
} volatile Thd_pars_def;


extern u32 HD_ParsFind(Thd_pars_def *par);
extern u16 HD_GoToScript(Thd_pars_def *par, char *name);
extern u16 HD_DoScript(Thd_pars_def *par);
extern void HD_ParsInit(Thd_pars_def *par, Trb_fifo  *fifo, const u8 *pacmd, u8 mode, void *proc);
extern void HD_ScriptFlagCF(Thd_pars_def *par, u8 flag);
extern void HD_ScriptFlagZF(Thd_pars_def *par, u8 flag);
extern u16 HD_ParsGetVarOFS(Thd_pars_def *par, u8 v, u8 ofs);
extern void HD_ParsGetVarSTRMM(Thd_pars_def *par, u8 v, void *outdata, u32 MaxSizeM);
extern void HD_ParsGetVarSTRSPM(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize);
extern void HD_ParsGetVarSTRSP(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize);
extern void HD_ParsGetVarSTRM(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize);
extern void HD_ParsGetVarSTR(Thd_pars_def *par, u8 v, void *outdata);
extern u8 HD_ParsGetVarSize(Thd_pars_def *par, u8 v);
extern u8 HD_ParsGetVarCount(Thd_pars_def *par);
extern s32 HD_ParsGetVarDEC(Thd_pars_def *par, u8 v);
extern u32 HD_ParsGetVarHEX(Thd_pars_def *par, u8 v);
extern u8 HD_ParsGetVarHEXBIN(Thd_pars_def *par, u8 v, u8 bytes, void *outdata);



#endif
