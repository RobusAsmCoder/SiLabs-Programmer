//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __RB_UTIL_H
#define __RB_UTIL_H

#include "SysType.h"

#define rb_def_flip8bit

#define rb_istr_mode_DEC  0x0000
#define rb_istr_mode_HEX  0x1000
#define rb_istr_mode_BIN  0x2000
#define rb_istr_mode_STR  0x8000

#define rb_istr_mode_zerro  0x0000
#define rb_istr_mode_space  0x0200
#define rb_istr_mode_dot    0x02E0

extern void rb_IntToProc(u32 v, u16 mode, void *outdata);
extern void rb_IntToStr(u32 v, u16 mode, void *s);
extern void rb_IntToHEX(u32 v, u16 mode, void *s);
extern void rb_IntToBIN(u32 v, u16 mode, void *s);
extern u8 rb_CompareSTR(void *s0, void *s1);
extern u8 rb_LengthSTR(void *s0);
extern u32 rb_CopySTR(void *s0, void *s1);
extern void rb_FillChar(void *s, u32 siz, u8 b);
extern void rb_CopyBlock(void *des, void *src, u32 siz);
extern u32 rb_CopyBlockMaxSize(void *des, void *src, u32 siz, u32 maxsize);
extern u8 rb_HexGet(u8 b);
extern u32 rb_HexToInt(u8 *s);
extern u32 rb_MaxD(u32 a, u32 b);
extern void rb_RolBlock(void *adr, u32 siz);

extern u32 rb_RangeU32(u32 min, u32 val, u32 max);
extern s32 rb_RangeS32(s32 min, s32 val, s32 max);

#ifdef rb_def_flip8bit
extern const u8 rb_buf_flip8b[256];
#else
extern const u8 rb_buf_flip4b[16];
#endif

__inline static u8 rb_Flip8(u8 b)
{
#ifdef rb_def_flip8bit
    return rb_buf_flip8b[b];
#else
    return (rb_buf_flip4b[b&15]<<4) | (rb_buf_flip4b[b>>4]);
#endif
}

__inline static u16 rb_Flip16(u16 w)
{
    return (rb_Flip8(w)<<8) | (rb_Flip8(w>>8));
}

__inline static u32 rb_Flip32(u32 d)
{
    return (rb_Flip16(d)<<16) | (rb_Flip16(d>>16));
}

extern void rb_StrSet(char *sde, const char *sin);
extern void rb_StrIns(char *sde, const char *sin);

#endif

