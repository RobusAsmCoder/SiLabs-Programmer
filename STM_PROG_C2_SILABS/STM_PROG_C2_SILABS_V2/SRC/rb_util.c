//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "rb_util.h"


//outdata = string or pointer to procedure
void rb_IntToProc(u32 v, u16 mode, void *outdata)
//void rb_IntToProc(u32 v, u16 mode, void (*SendByte)(u8))
{
      const u8 HexStr[16]="0123456789ABCDEF";
      void (*SendByte)(u8)=(void (*)(u8))outdata;
      u8 FiZe=(mode>>4)&0x00FF;
      u8 Dig=mode & 0x000F;
      u8 *s=outdata;
      u16 Mode = mode & 0x3000;
  
      if (Mode==rb_istr_mode_DEC)
      {
        if (v&0x80000000)
        {
          if (mode & rb_istr_mode_STR)
          { 
            s++[0]='-';
          } else {
            SendByte('-');
          }
          v=-v;
        }
      }
    
      if (FiZe==0) FiZe='0';

      switch (Mode)
      {
        case rb_istr_mode_DEC:   // DEC mode
        {
          u8 dig_c=10;
          u32 dig_div=1000000000;
          u8 fc='0';
          while (dig_c)
          {
            u8 c='0';
            while (dig_div<=v)
            {
              c++;
              v-=dig_div;
            }
            fc|=c;
            if ( (fc=='0') && (c=='0') && (dig_c!=1) ) c=FiZe;
            if ( (dig_c<=Dig) || (dig_c==1) || ( (Dig==0) && (fc!='0') ) )
            {
              if (mode & rb_istr_mode_STR)
              {
                s++[0]=c;
              } else {
                SendByte(c);
              }
            }
            dig_div/=10;
            dig_c--;
          }
        }
        break;
        case rb_istr_mode_HEX:   // HEX mode
        case rb_istr_mode_BIN:   // BIN mode
          if (Dig==0)
          {
            if (v&0xFF000000) {
              Dig=8;
            } else if (v&0x00FF0000) {
              Dig=6;
            } else if (v&0x0000FF00) {
              Dig=4;
            } else {
              Dig=2;
            }
          }
          v<<=(8-Dig)*4;
          while (Dig--)
          {
            if (Mode==rb_istr_mode_HEX)
            {
              u8 c=(HexStr[v>>28]);
              if (mode & rb_istr_mode_STR)
              {
                s++[0]=c;
              } else {
                SendByte(c);
              }
            } else {
              #define rdBitBin(_v,_bit)   (char)( (((_v)>>(_bit))&1) +'0')
              if (mode & rb_istr_mode_STR)
              {
                s++[0]=rdBitBin(v,31);
                s++[0]=rdBitBin(v,30);
                s++[0]=rdBitBin(v,29);
                s++[0]=rdBitBin(v,28);
              } else {
                SendByte(rdBitBin(v,31));
                SendByte(rdBitBin(v,30));
                SendByte(rdBitBin(v,29));
                SendByte(rdBitBin(v,28));
              }
            }
            v<<=4;
          }
        break;
      }
      if (mode & rb_istr_mode_STR) s[0]=0;
}



void rb_IntToStr(u32 v, u16 mode, void *s)
{
      rb_IntToProc(v, mode | rb_istr_mode_STR, (void (*)(u8))s);
}

void rb_IntToHEX(u32 v, u16 mode, void *s)
{
      rb_IntToProc(v, mode | rb_istr_mode_HEX, (void (*)(u8))s);
}

void rb_IntToBIN(u32 v, u16 mode, void *s)
{
      rb_IntToProc(v, mode | rb_istr_mode_BIN, (void (*)(u8))s);
}

u8 rb_CompareSTR(void *s0, void *s1)
{
    u8 *ss0=s0;
    u8 *ss1=s1;
    while (ss0[0]==ss1[0])
    {
      if (ss0[0]==0) return(1);
      ss0++;
      ss1++;
    }
    return(0);
}

u32 rb_CopySTR(void *s0, void *s1)
{
    u8 *ss0=s0;
    u8 *ss1=s1;
    u32 siz=0;
    while (1)
    {
      ss0[0]=ss1[0];
      if (ss0[0]==0) break;
      ss0++;
      ss1++;
      siz++;
    }
    return(siz);
}

u8 rb_LengthSTR(void *s0)
{
    u8 *ss0=s0;
    u32 size=0;
    while (*ss0++) size++;
    return(size);
}

void rb_FillChar(void *s, u32 siz, u8 b)
{
    u8 *ss=s;
    while(siz--) *ss++=b;
}

void rb_CopyBlock(void *des, void *src, u32 siz)
{
    u8 *ss0=des;
    u8 *ss1=src;
    while(siz--) *ss0++=*ss1++;
}

u32 rb_CopyBlockMaxSize(void *des, void *src, u32 siz, u32 maxsize)
{
    u32 size=((siz<maxsize)? siz: maxsize);
    rb_CopyBlock(des, src, size );
    return(size);
}
//Out: 0x00..0x0F = value convert
//     0xF0 = error converting
u8 rb_HexGet(u8 b)
{
    if ((b>='0') && (b<='9')) return b-'0';
    if ((b>='A') && (b<='F')) return b-'A'+10;
    if ((b>='a') && (b<='f')) return b-'a'+10;
    return 0xF0;
}

u32 rb_HexToInt(u8 *s)
{
      u32 d=0;
      u8 b;
      while((b=*s++)!=0)
      {
        if ((b=rb_HexGet(b)) & 0xF0) break;
        d = (d<<4) | b;
      }
      return d;
}

u32 rb_MaxD(u32 a, u32 b)
{
    if (a>b) return a;
    return b;
}

void rb_RolBlock(void *adr, u32 siz)
{
    if (siz)
    {
      u8 *s=adr;
      u8 b=*s;
      siz--;
      while(siz--)
      {
        *s=s[1];
        s++;
      }
      *s=b;
    }


}

u32 rb_RangeU32(u32 min, u32 val, u32 max)
{
    if (val<min) return min;
    if (val>max) return max;
    return val;
}

s32 rb_RangeS32(s32 min, s32 val, s32 max)
{
    if (val<min) return min;
    if (val>max) return max;
    return val;
}

#ifdef rb_def_flip8bit
const u8 rb_buf_flip8b[256]={
  0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
  0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
  0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
  0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
  0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
  0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
  0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
  0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
  0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
  0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
  0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
  0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
  0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
  0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
  0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
  0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF
};
#else
const u8 rb_buf_flip4b[16]={0x0,0x8,0x4,0xC,0x2,0xA,0x6,0xE,0x1,0x9,0x5,0xD,0x3,0xB,0x7,0xF};
#endif


void rb_StrSet(char *sde, const char *sin)
{
      rb_CopySTR((void *)sde, (void *)sin);
}

void rb_StrIns(char *sde, const char *sin)
{
      u32 dsi=rb_LengthSTR((void *)sde);
      u32 isi=rb_LengthSTR((void *)sin);
      u32 n;
      for(n=0; n<(dsi+1); n++)
      {
        sde[dsi+isi-n] = sde[dsi-n];
      }
      for(n=0; n<isi; n++)
      {
        sde[n]=*sin++;
      }
}

