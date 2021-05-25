//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#include "SysType.h"
#include "hardware.h"
#include "rb_fifo.h"
#include "rb_util.h"
#include "HD_PARS.h"


#include "PARS\parssys.h"

//extern u32 Timer_1ms_Counter;

//OUT:0x0001XXXX Script Command  + Enter Detected
//    0x00000000 Packet Not Full
//    0x00010000 Packet Not Full + Enter Detected
u32 HD_ParsFind(Thd_pars_def *par)
{
      u16 pap=0;    //pasmd pos
      u8 si;
      if (par->par_SIZ!=0)
      {
        rb_fifo_flush (par->fifo, par->par_SIZ);
        par->par_SIZ=0;
      }
      if (par->domode == hdPARS_mode_line_en)
      {
        while ( (si=rb_fifo_size(par->fifo)) != 0 )
        {
          u8 b=rb_fifo_rd(par->fifo);//rb_fifo_ofs_rd(par->fifo, 0);
          if ((b==0x0D) || (b==0x0A))
          {
            par->domode=0;
            return(0x00010000);
          }
        }
        return(0);
      }
      si=rb_fifo_size(par->fifo);
      if (par->mode == hdPARS_mode_line_en)
      { 
        while (si)
        {
          u8 b=rb_fifo_ofs_rd(par->fifo, 0);
          if ((b==0x0D) || (b==0x0A))
          {
            rb_fifo_flush(par->fifo,1);
          } else {
            break;
          }
          si=rb_fifo_size(par->fifo);
        }
      }
      if (si)
      {
        u8 pasi;
        while ((pasi=par->pacmd[pap]) != 0)
        {
          u8 f=0;   //fifo offset
          u8 p=1;   //pacmd offset
          u8 v=0;
          par->domode = 0;
          while (1)
          {
            u8 c=par->pacmd[pap+p];
            u8 b;
            par->varc=0;
            p++;
            if (c)
            { //compare byte
              if (f>=si) return(0x0000); //need bytes in fifo
              b=rb_fifo_ofs_rd(par->fifo, f);
              if (c!=b)
              {
                if (par->domode != hdPARS_mode_process_VAR)
                {
                  goto next_par;
                } else {
                  if (par->mode == hdPARS_mode_line_en)
                  {
                    if ((b==0x0D) || (b==0x0A))
                    {
                      goto next_par;
                    }
                  }
                  p--;
                }
              } else {
                if (par->domode == hdPARS_mode_process_VAR)
                {
                  par->vars[v]=f-par->varp[v];
                  v++;
                }
                par->domode = 0;
              }
              f++;
            } else {
              c=par->pacmd[pap+p];
              p++;
              switch (c)
              {
                case SYSCMD_EN_OR_LF:        //Compare ENTER
                  if (f>=si) return(0x0000); //need bytes in fifo
                  b=rb_fifo_ofs_rd(par->fifo, f);
                  if (!((b==0x0D) || (b==0x0A) ))
                  {
                    if (par->domode != hdPARS_mode_process_VAR)
                    {
                      goto next_par;
                    } else {
                      p-=2;
                    }
                  } else {
                    if (par->domode == hdPARS_mode_process_VAR)
                    {
                      par->vars[v]=f-par->varp[v];
                      v++;
                    }
                  }
                  f++;
                  break;
                case SYSCMD_VAR:
                  par->varp[v]=f;
                  par->domode = hdPARS_mode_process_VAR;
                  break;
                case SYSCMD_ID_END:
                  par->par_ID  = par->pacmd[pap+p+0] | (par->pacmd[pap+p+1]<<8);
                  par->par_SIZ = f;
                  par->varc = v;
                  while(v<PARS_VARS) par->vars[v++]=0;
                  return(par->par_ID);
              }
            }
          }
          //return(0x0000); //need bytes in fifo
next_par: pap += pasi;
        }
      } else {
        return(0x0000);   //need bytes in fifo
      }
      switch (par->mode)
      {
        case hdPARS_mode_line_en:
          par->domode = hdPARS_mode_line_en;
          break;
        default:
          par->par_SIZ = 1;
          break;
      }
      return(0xFFFF*0);     //flush one byte from buffer
}


// Return value HighByte = Size Of VAR, LowByte = Data
u16 HD_ParsGetVarOFS(Thd_pars_def *par, u8 v, u8 ofs)
{
      u8 f=par->varp[v];
      u16 w=0x0000;
      if (v < par->varc)
      {
        u8 si=par->vars[v];
        if (ofs<si)
        {
          w=rb_fifo_ofs_rd(par->fifo, f+ofs) | (si<<8);
        }
      }
      return w;
}

void HD_ParsGetVarSTRMM(Thd_pars_def *par, u8 v, void *outdata, u32 MaxSizeM)
{
      u8 MaxSize=MaxSizeM;
      u8 md = MaxSizeM>>8;
      u8 *s=outdata;
      if (v < par->varc)
      {
        u8 n;
        u8 f=par->varp[v];
        u8 varsiz=par->vars[v];
        if (md)
        {// Find Spaces
          while(varsiz)
          {
            u8 b=rb_fifo_ofs_rd(par->fifo, f);
            if (b!=' ') break;
            f++;
            varsiz--;
          }
          while(varsiz)
          {
            u8 b=rb_fifo_ofs_rd(par->fifo, f+varsiz-1);
            if (b!=' ') break;
            varsiz--;
          }
        }
        if (MaxSize > varsiz) MaxSize = varsiz;
        for (n=0; n<MaxSize; n++ ) s++[0]=rb_fifo_ofs_rd(par->fifo, f+n);
      }
      s[0]=0;
}

void HD_ParsGetVarSTRSPM(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize)
{
      HD_ParsGetVarSTRMM(par, v, outdata, MaxSize | 0x0100);
}

void HD_ParsGetVarSTRSP(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize)
{
      HD_ParsGetVarSTRMM(par, v, outdata, 0x01FF);
}

void HD_ParsGetVarSTRM(Thd_pars_def *par, u8 v, void *outdata, u8 MaxSize)
{
      HD_ParsGetVarSTRMM(par, v, outdata, MaxSize);
}

void HD_ParsGetVarSTR(Thd_pars_def *par, u8 v, void *outdata)
{
      HD_ParsGetVarSTRM(par, v, outdata, 255);
}

u8 HD_ParsGetVarSize(Thd_pars_def *par, u8 v)
{
    if (v<par->varc) return(par->vars[v]);
    return(0);
}

u8 HD_ParsGetVarCount(Thd_pars_def *par)
{
    return(par->varc);
}

s32 HD_ParsGetVarDEC(Thd_pars_def *par, u8 v)
{
      s32 d=0;
      u8 sig=0;
      u8 n;
      u8 f=par->varp[v];
      //par->ParsGetVar_ERROR=0;
      if (v < par->varc)
      {
        for (n=0; n<par->vars[v]; n++) 
        {
          u8 b=rb_fifo_ofs_rd(par->fifo, f+n);
          if ((b>='0') && (b<='9'))
          {
            d *= 10;
            d += b & 0x0F;
          } else {
            if ((b=='-') && (d==0)) {
              sig^=1;
            } else if ( ((b=='+')&&(d==0)) || (b==' ')){
            } else {
              par->ParsGetVar_ERROR=1;
            }
          }
        }
      }
      if (sig) d=-d;
      return(d);
}
u16 HD_ParsConvertCharHEX(u8 b)
{
  
    if ((b>='0') && (b<='9')) {
      return(b-'0');
    } else if ((b>='a') && (b<='f')) {
      return(b-'a'+10);
    } else if ((b>='A') && (b<='F')) {
      return(b-'A'+10);
    }
    return(0xFF00);
  
  /*
    b=rb_HexGet(b);
    if ((b&0xF0)==0) return b;
    return 0xFF00;
  */
}

u32 HD_ParsGetVarHEX(Thd_pars_def *par, u8 v)
{
      u32 d=0;
      u8 n;
      u8 f=par->varp[v];
      //par->ParsGetVar_ERROR=0;
      if (v < par->varc)
      {
        for (n=0; n<par->vars[v]; n++) 
        {
          u16 w=HD_ParsConvertCharHEX( rb_fifo_ofs_rd(par->fifo, f+n) );
          if(w & 0xFF00)
          {
            par->ParsGetVar_ERROR=1;
          } else {
            d <<= 4;
            d |= w;
          }

        }
      }
      return(d);
}

u8 HD_ParsGetVarHEXBIN(Thd_pars_def *par, u8 v, u8 bytes, void *outdata)
{
      u8 n=0;
      u8 f=par->varp[v];
      u8 i=0;
      u8 *s=outdata;
      if (v < par->varc)
      {
        while(((n+2)<=par->vars[v]) && (bytes))
        {
          u16 w0=HD_ParsConvertCharHEX( rb_fifo_ofs_rd(par->fifo, f+(n++)) );
          u16 w1=HD_ParsConvertCharHEX( rb_fifo_ofs_rd(par->fifo, f+(n++)) );
          if ( (w0 | w1) & 0xFF00)
          {
            par->ParsGetVar_ERROR=1;
            break;
          } else {
            s++[0]=w1 | (w0<<4);
            i++;
          }
          bytes--;
        }
        
      }
      return(i);
}

void HD_ClearDataScript(Thd_pars_def *par)
{
      par->scr.delay_timer=0;
      par->scr.error=0;
      par->scr.flag=0;
      par->scr.send_rep=0;
      par->scr.send_timer=0;
      par->scr.send_rep_max=3;      //send repeat
      par->scr.send_time_max=500;   //send time for answer
}

u16 HD_GoToScript(Thd_pars_def *par, char *name)
{
    u16 n;
    u16 si;

    for (n=0; par->pacmd[n]!=0; n+=par->pacmd[n]);
    n++;
    par->scr.ptr=n;
  
    for (n=par->scr.ptr; (si=par->pacmd[n] | (par->pacmd[n+1]<<8))!=0; n+=si )
    {
      u8 m=0;
      while(1)
      {
        if (name[m]!=par->pacmd[n+2+m]) break;
        if (name[m]==0)
        {
          n=n+2+m+1;
          par->scr.ptr=n;
          HD_ClearDataScript(par);
          return(n);
        }
        m++;
      }
    }
    return(0);
}

u16 HD_DoParsFind(Thd_pars_def *par)
{
    u16 id;
    
    id=HD_ParsFind(par);
    if (id) par->answer_id=id;
    return(id);
}


void HD_ScriptFlagCF(Thd_pars_def *par, u8 flag)
{
    if (flag)
    {
      par->scr.flag |=  hdPARS_flag_CF;
    } else {
      par->scr.flag &= ~hdPARS_flag_CF;
    }
}

void HD_ScriptFlagZF(Thd_pars_def *par, u8 flag)
{
    if (flag)
    {
      par->scr.flag |=  hdPARS_flag_ZF;
    } else {
      par->scr.flag &= ~hdPARS_flag_ZF;
    }
}
//OUT: 0x0000-In Process
//     0xFFFF=End Process
u16 HD_DoScript(Thd_pars_def *par)
{
    u8 b;
    u16 id;
    u8 a0,a1;
    u32 d,d1;
    
  
    if (par->scr.delay_timer)
    {
      if (!isTimerCompareOverflow(par->scr.delay_timer, Timer_1ms_Counter))//(Timer_1ms_Counter<par->scr.delay_timer)
      {
        return(0x0000);
      }
      par->scr.delay_timer=0;
    }
//rept_switch:
    switch (par->pacmd[par->scr.ptr++])
    {
      case SCRPT_SEND:
        id=par->scr.ptr-1;
        if (par->scr.send_rep_ptr!=id)
        {
          par->answer_id=0;
          par->scr.send_rep_ptr = id;
          par->scr.send_rep = par->scr.send_rep_max;
        }
        if ((par->scr.flag & hdPARS_flag_send_delay)==0)
        {
          par->scr.delay_timer = Timer_1ms_Counter + par->scr.send_time_before;
          par->scr.flag |= hdPARS_flag_send_delay;
          par->scr.ptr--;
          break;
        }
        par->par_SIZ=0;
        rb_fifo_flush(par->fifo,rb_fifo_size(par->fifo));
        par->scr.flag &= ~hdPARS_flag_send_delay;
        par->scr.send_timer = Timer_1ms_Counter + par->scr.send_time_max;
        while ((b=par->pacmd[par->scr.ptr++])!=0)
        {
          switch (b)
          {
            case SCRPT_VAR_DEC:
              b=par->pacmd[par->scr.ptr++] & 0x7F;
              rb_IntToProc(par->scr.var[b], rb_istr_mode_DEC, par->SendByte);
              break;
            case SCRPT_VAR_HEX:
              b=par->pacmd[par->scr.ptr++] & 0x7F;
              rb_IntToProc(par->scr.var[b], rb_istr_mode_HEX, par->SendByte);
              break;
            case SCRPT_VAR_STR:
              b=par->pacmd[par->scr.ptr++] & 0x7F;
              {
                u8 *s=(u8 *)par->scr.var[b];
                while ((b=s++[0])!=0) par->SendByte(b);
              }
              break;
            default:
              par->SendByte(b);
              break;
          }
        }
        break;
      case SCRPT_DELAY:
        par->scr.delay_timer = Timer_1ms_Counter + par->pacmd[par->scr.ptr++]*10;
        break;
      case SCRPT_WAITBEFORESEND:
        par->scr.send_time_before = par->pacmd[par->scr.ptr++]*100;
        break;
      case SCRPT_WAITMAX:
        par->scr.send_time_max = par->pacmd[par->scr.ptr++]*100;
        break;
      case SCRPT_REPTMAX:
        par->scr.send_rep_max = par->pacmd[par->scr.ptr++];
        par->scr.send_rep_ptr=0;
        break;
      case SCRPT_ANSWER:
        HD_DoParsFind(par);
        id = uu16(par->pacmd[par->scr.ptr]);//par->pacmd[par->scr.ptr+0] | (par->pacmd[par->scr.ptr+1]<<8);
        par->scr.ptr--;
        if (id==par->answer_id)
        {
          par->scr.send_rep_ptr=0;
          par->scr.ptr+=3;
//          goto rept_switch;
        } else {
          if (Timer_1ms_Counter>par->scr.send_timer)
          {
            if (par->scr.send_rep<2)
            {
              par->scr.ptr+=3;
              return(0xFFFE);   //error
            }
            par->scr.send_rep--;
            par->scr.ptr = par->scr.send_rep_ptr;
          }
        }
        if (par->answer_id)
        {
          id = par->answer_id;
          par->answer_id=0;
          return(id);
        }
        break;
      case SCRPT_EVENT:
        id = par->pacmd[par->scr.ptr+0] | (par->pacmd[par->scr.ptr+1]<<8);
        par->scr.ptr+=2;
        return(id);
      case SCRPT_LET:
        id = par->pacmd[par->scr.ptr++];
        a0 = par->pacmd[par->scr.ptr++];
        a1 = a0>>4;
        a0 &=0x0F;
        switch (id)
        {
          case SCRPT_LET_SCRPT_VAR:
            d=par->scr.var[a1];
            break;
          case SCRPT_LET_RECIVED_VAR_DEC:
            d=HD_ParsGetVarDEC(par, a1);
            break;
          case SCRPT_LET_RECIVED_VAR_HEX:
            d=HD_ParsGetVarHEX(par, a1);
            break;
          case SCRPT_LET_VALUE:
//            d  = par->pacmd[par->scr.ptr+0] |
//                (par->pacmd[par->scr.ptr+1]<<8) |
//                (par->pacmd[par->scr.ptr+2]<<16) |
//                (par->pacmd[par->scr.ptr+3]<<24);
            d = uu32(par->pacmd[par->scr.ptr]);
            par->scr.ptr+=4;
            break;
        }
        par->scr.var[a0]=d;
        break;
      case SCRPT_CMP:
        id = par->pacmd[par->scr.ptr++];
        a0 = par->pacmd[par->scr.ptr++];
        a1 = a0>>4;
        a0 &=0x0F;
        switch (id)
        {
          case SCRPT_LET_SCRPT_VAR:
            d=par->scr.var[a1];
            break;
          case SCRPT_LET_RECIVED_VAR_DEC:
            d=HD_ParsGetVarDEC(par, a1);
            break;
          case SCRPT_LET_RECIVED_VAR_HEX:
            d=HD_ParsGetVarHEX(par, a1);
            break;
          case SCRPT_LET_VALUE:
//            d  = par->pacmd[par->scr.ptr+0] |
//                (par->pacmd[par->scr.ptr+1]<<8) |
//                (par->pacmd[par->scr.ptr+2]<<16) |
//                (par->pacmd[par->scr.ptr+3]<<24);
            d = uu32(par->pacmd[par->scr.ptr]);
            par->scr.ptr+=4;
            break;
        }
        d1=par->scr.var[a0];
        if(d1<d)
        {
          par->scr.flag |=  hdPARS_flag_CF;
        } else {
          par->scr.flag &= ~hdPARS_flag_CF;
        }
        if(d1==d)
        {
          par->scr.flag |=  hdPARS_flag_ZF;
        } else {
          par->scr.flag &= ~hdPARS_flag_ZF;
        }
        break;
      case SCRPT_JR:
        a0 = par->pacmd[par->scr.ptr++];
        par->scr.ptr+=(s8)a0;
        break;
      case SCRPT_JR_Z:
        a0 = par->pacmd[par->scr.ptr++];
        if ((par->scr.flag & hdPARS_flag_ZF)!=0) par->scr.ptr+=(s8)a0;
        break;
      case SCRPT_JR_NZ:
        a0 = par->pacmd[par->scr.ptr++];
        if ((par->scr.flag & hdPARS_flag_ZF)==0) par->scr.ptr+=(s8)a0;
        break;
      case SCRPT_JR_C:
        a0 = par->pacmd[par->scr.ptr++];
        if ((par->scr.flag & hdPARS_flag_ZF)!=0) par->scr.ptr+=(s8)a0;
        break;
      case SCRPT_JR_NC:
        a0 = par->pacmd[par->scr.ptr++];
        if ((par->scr.flag & hdPARS_flag_ZF)==0) par->scr.ptr+=(s8)a0;
        break;
      case SCRPT_JP:
        d = uu16(par->pacmd[par->scr.ptr]);
        //par->scr.ptr+=2;
        par->scr.ptr=d;
        break;
      case SCRPT_JP_Z:
        d = uu16(par->pacmd[par->scr.ptr]);
        par->scr.ptr+=2;
        if ((par->scr.flag & hdPARS_flag_ZF)!=0) par->scr.ptr=d;
        break;
      case SCRPT_JP_NZ:
        d = uu16(par->pacmd[par->scr.ptr]);
        par->scr.ptr+=2;
        if ((par->scr.flag & hdPARS_flag_ZF)==0) par->scr.ptr=d;
        break;
      case SCRPT_JP_C:
        d = uu16(par->pacmd[par->scr.ptr]);
        par->scr.ptr+=2;
        if ((par->scr.flag & hdPARS_flag_ZF)!=0) par->scr.ptr=d;
        break;
      case SCRPT_JP_NC:
        d = uu16(par->pacmd[par->scr.ptr]);
        par->scr.ptr+=2;
        if ((par->scr.flag & hdPARS_flag_ZF)==0) par->scr.ptr=d;
        break;
      case SCRPT_END:
      default:
        par->scr.ptr--;
        return(0xFFFF);
    }
  
  
    //par->scr.ptr = Timer_1ms_Counter;
    return (0x0000);
}


void HD_ParsInit(Thd_pars_def *par, Trb_fifo  *fifo, const u8 *pacmd, u8 mode, void *proc)
{
//    u16 n;
    par->fifo = fifo;
    par->pacmd = (u8 *)(pacmd);
    par->par_SIZ = 0;
    par->par_ID = 0;
    par->mode = mode;
    par->domode = 0;
    par->SendByte = (void (*)(u8))proc;
    par->scr.send_rep_ptr=0;
    par->scr.send_time_before=1000;
  
//    par->SendByte(0x23);
}




