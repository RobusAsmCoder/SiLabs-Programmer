//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_LOOP_H
#define __MAIN_LOOP_H

#include "SysType.h"


#define Send_DBG_HEX(_da, _Dig)       Send_RS_HEX(_da, _Dig)
#define Send_DBG_DEC(_da)             Send_RS_DEC(_da)
#define Send_DBG_Byte(_c)             Send_RS_Byte(_c)
#define Send_DBG_String(_s)           Send_RS_String(_s)
#define Send_DBG_StringEN(_s)         Send_RS_String(_s);  Send_RS_String("\n\r")



extern void MainLoopPool(void);
extern void MainLoopInit(void);

#endif
