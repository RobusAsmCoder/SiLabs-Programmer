//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __RB_FIFO_ROBUS_H
#define __RB_FIFO_ROBUS_H
#include "SysType.h"

typedef struct {
  u16       rd;
  u16       wr;
  u16       size_mask;
  u8        *buf;
  void      *bfifo;
} volatile Trb_fifo;

#define rb_fifo_allocA(__x__, __siz__, _fi_)    u8 __x__##_buf[(__siz__)];                           \
                                                Trb_fifo __x__={.rd        =0,                       \
                                                                .wr        =0,                       \
                                                                .size_mask = (__siz__)-1,            \
                                                                .buf       = (void *)(&__x__##_buf), \
                                                                .bfifo     = _fi_}

#define rb_fifo_alloc(__x__,__siz__)            rb_fifo_allocA(__x__, __siz__, 0)

extern u8 rb_fifo_rd(Trb_fifo *fif);
extern u8 rb_fifo_ofs_rd(Trb_fifo *fif, u16 ofs);
#define rb_fifo_ofs_rd_B(_fif, _ofs)    rb_fifo_ofs_rd(_fif, _ofs)
extern u16 rb_fifo_ofs_rd_W(Trb_fifo *fif, u16 ofs);
extern u32 rb_fifo_ofs_rd_T(Trb_fifo *fif, u16 ofs);
extern u32 rb_fifo_ofs_rd_D(Trb_fifo *fif, u16 ofs);
extern void rb_fifo_wr(Trb_fifo *fif, u8 c);
                          
extern u16 rb_fifo_rd_W(Trb_fifo *fif);
extern u32 rb_fifo_rd_D(Trb_fifo *fif);
extern u16 rb_fifo_rd_blk(Trb_fifo *fif, void *blk, u16 siz);
                                                       
extern void rb_fifo_wr_W(Trb_fifo *fif, u16 w);
extern void rb_fifo_wr_D(Trb_fifo *fif, u32 d);
extern u16 rb_fifo_wr_blk(Trb_fifo *fif, void *blk, u16 siz);

extern u16 rb_fifo_size(Trb_fifo *fif);
extern void rb_fifo_flush(Trb_fifo *fif, u16 ofs);
extern void rb_fifo_clear(Trb_fifo *fif);
extern u32 rb_fifo_free(Trb_fifo *fif);
extern void rb_fifo_wr_wait(Trb_fifo *fif, u8 c);


#endif //__RB_FIFO_ROBUS_H
