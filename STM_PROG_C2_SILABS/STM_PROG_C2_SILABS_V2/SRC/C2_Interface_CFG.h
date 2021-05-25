//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __C2_INTERFACE_CFG_H
#define __C2_INTERFACE_CFG_H

#include "SysType.h"
#include "hardware.h"


//extern void C2_DAT(u8 b);
//extern u8 C2_DAT_GET(void);
//extern void C2_DAT_MODE_IN(void);
//extern void C2_DAT_MODE_OUT(void);
//extern void C2_StrobeDoDelay(void);
//extern void C2_StrobeSendWait(u8 b);
//extern void C2_StrobeLength(u16 v);

#define SIrC2D()        C2_DAT_GET()
#define SIwC2D(_v)      C2_DAT(_v)
#define SI_C2D_IN()     C2_DAT_MODE_IN()
#define SI_C2D_OUT()    C2_DAT_MODE_OUT()


extern u8 C2_ProcInterface(u8 c2inty, u16 v);


#endif
