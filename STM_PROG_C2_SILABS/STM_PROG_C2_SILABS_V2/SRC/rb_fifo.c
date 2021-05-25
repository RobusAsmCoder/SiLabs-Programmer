//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
// V.1.03
//  + Copy Blobk use system memcpy
//  + FIFO group procedures(echo effect)
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "rb_fifo.h"
#include <string.h>

#define USE_BLOCK_COPY    1

u8 rb_fifo_rd(Trb_fifo *fif)
{
    u8 b=0xFF;
    if (fif->buf)
    {
      b=fif->buf[fif->rd++];
      fif->rd &= fif->size_mask;
    }
    return b;
}

/*
u8 rb_fifo_ofs_rd(Trb_fifo *fif, u16 ofs)
{
    u8 b=0xFF;
    if (fif->buf)
    {
      b=(fif->buf[(fif->rd+ofs) & fif->size_mask]);
    }
    return b;
}

u16 rb_fifo_ofs_rd_W(Trb_fifo *fif, u16 ofs)
{
    u16 w=rb_fifo_ofs_rd(fif, ofs+0) | (rb_fifo_ofs_rd(fif, ofs+1)<<8);
    return w;
}

u32 rb_fifo_ofs_rd_T(Trb_fifo *fif, u16 ofs)
{
    u32 d=rb_fifo_ofs_rd_W(fif, ofs+0) | (rb_fifo_ofs_rd(fif, ofs+2)<<16);
    return d;
}

u32 rb_fifo_ofs_rd_D(Trb_fifo *fif, u16 ofs)
{
    u32 d=rb_fifo_ofs_rd_W(fif, ofs+0) | (rb_fifo_ofs_rd_W(fif, ofs+2)<<16);
    return d;
}
 
*/ 

 u16 rb_fifo_ofs_rd_blk_is(Trb_fifo *fif, u16 ofss, void *blk, u16 siz, u8 isFlush)
{
    u16 size = rb_fifo_size(fif);
    u16 rsiz = 0;
    if ( (fif->buf) && (ofss<size) )
    {
      u8 *s=blk;
      u16 rd = fif->rd + ofss;
      if ( (ofss+siz)>size ) siz=size-ofss;
      rsiz = ofss;
      if (fif->buf)
      {
        u16 mask =  fif->size_mask;
        u16 ofs=rd & mask;
        if (size>siz) size=siz;
        siz=size;
        while (siz)
        {
          u16 sizm = (mask+1) - ofs;   // max block copy
          if (sizm>siz) sizm=siz;
          memcpy(s, &fif->buf[ofs], sizm);
          ofs = (ofs+sizm) & mask;
          siz-=sizm;
          rsiz+=sizm;
          s+=sizm;
        }
        if (isFlush) fif->rd = ofs;
      }
    }
    return rsiz;
}

u16 rb_fifo_ofs_rd_blk(Trb_fifo *fif, u16 ofss, void *blk, u16 siz)
{
    return rb_fifo_ofs_rd_blk_is(fif, ofss, blk, siz, 0);
}

u8 rb_fifo_ofs_rd(Trb_fifo *fif, u16 ofss)
{
    u8 b=0;
    rb_fifo_ofs_rd_blk(fif, ofss, &b, 1);
    return b;
}

u16 rb_fifo_ofs_rd_W(Trb_fifo *fif, u16 ofss)
{
    u16 w=0;
    rb_fifo_ofs_rd_blk(fif, ofss, &w, 2);
    return w;
}

u32 rb_fifo_ofs_rd_T(Trb_fifo *fif, u16 ofss)
{
    u32 d=0;
    rb_fifo_ofs_rd_blk(fif, ofss, &d, 3);
    return d;
}

u32 rb_fifo_ofs_rd_D(Trb_fifo *fif, u16 ofss)
{
    u32 d=0;
    rb_fifo_ofs_rd_blk(fif, ofss, &d, 4);
    return d;
}



////////////////////////////////////////////////////////////////////////

//return real readed size
u16 rb_fifo_rd_blk(Trb_fifo *fif, void *blk, u16 siz)
{
#if !USE_BLOCK_COPY
    u8 *s=blk;
    u16 size=0;
    if (fif->buf)
    {
      while (siz)
      {
        if (rb_fifo_size(fif))
        {
          s[size++]=rb_fifo_rd(fif);
        } else break;
        siz--;
      }
    }
    return size;
#else
    u8 *s=blk;
    u16 size = 0;
    if (fif->buf)
    {
      u16 mask =  fif->size_mask;
      u16 ofs=fif->rd & mask;
      size = rb_fifo_size(fif);
      if (size>siz) size=siz;
      siz=size;
      while (siz)
      {
        u16 sizm = (mask+1) - ofs;   // max block copy
        if (sizm>siz) sizm=siz;
        memcpy(s, &fif->buf[ofs], sizm);
        ofs = (ofs+sizm) & mask;
        siz-=sizm;
        s+=sizm;
      }
      fif->rd = ofs;
    }
    return size;
#endif
}

u16 rb_fifo_rd_W(Trb_fifo *fif)
{
    u16 w=0;
    rb_fifo_rd_blk(fif, &w, 2);
    return w;
}

u32 rb_fifo_rd_D(Trb_fifo *fif)
{
    u32 d=0;
    rb_fifo_rd_blk(fif, &d, 4);
    return d;
}

void rb_fifo_flush(Trb_fifo *fif, u16 ofs)
{
    if (fif->buf)
    {
      u16 siz=rb_fifo_size(fif);
      if ( siz < ofs ) ofs=siz;
      fif->rd = (fif->rd+ofs) & fif->size_mask;
    }
}

void rb_fifo_clear(Trb_fifo *fif)
{
    if (fif->buf)
    {
      fif->rd = fif->wr;
    }
}

void rb_fifo_wr(Trb_fifo *fif, u8 c)
{
    if (fif->buf)
    {
      fif->buf[fif->wr++]=c;
      fif->wr &= fif->size_mask;
    }
    if (fif->bfifo)
    {
      rb_fifo_wr(fif->bfifo, c);
    }
}

void rb_fifo_wr_blkA(Trb_fifo *fif, void *blk, u16 siz)
{
#if !USE_BLOCK_COPY
    u8 *s=blk;
    while (siz)
    {
      rb_fifo_wr(fif, s++[0]);
      siz--;
    }
#else
    u8 *s=blk;
    if (fif->buf)
    {
      u16 mask =  fif->size_mask;
      while (siz)
      {
        vu16 ofs=fif->wr & mask;
        vu16 sizm = (mask+1) - ofs;   // max block copy
        if (sizm>siz) sizm=siz;
        while (rb_fifo_free(fif)<sizm);
        memcpy(&fif->buf[ofs], (u8 *)s, sizm);
        ofs = (ofs+sizm) & mask;
        fif->wr=ofs;
        s+=sizm;
        siz-=sizm;
      }
    }
#endif
}
//Return Size sended to bfifo
u16 rb_fifo_wr_blk(Trb_fifo *fif, void *blk, u16 siz)
{
  //bfifo
  rb_fifo_wr_blkA(fif, blk, siz);
  if (fif->bfifo)
  {
    u16 fifre=rb_fifo_free(fif->bfifo);
    if (siz>fifre) siz=fifre;
    if (siz) rb_fifo_wr_blkA(fif->bfifo, blk, siz);
    return siz;
  }
  return 0;
}

void rb_fifo_wr_W(Trb_fifo *fif, u16 w)
{
    rb_fifo_wr_blk(fif, &w, 2);
}
void rb_fifo_wr_D(Trb_fifo *fif, u32 d)
{
    rb_fifo_wr_blk(fif, &d, 4);
}

u16 rb_fifo_size(Trb_fifo *fif)
{
    return ( (fif->wr-fif->rd) & fif->size_mask );
}

u32 rb_fifo_free(Trb_fifo *fif)
{
    return ( fif->size_mask - rb_fifo_size(fif)  );
}

void rb_fifo_wr_wait(Trb_fifo *fif, u8 c)
{
    while ( rb_fifo_free(fif)==0 );
    rb_fifo_wr(fif, c);
}


