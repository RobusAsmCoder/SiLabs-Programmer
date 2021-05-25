


#define SYSCMD_ZERRO            0x00
#define SYSCMD_EN_OR_LF         0x01
#define SYSCMD_VAR              0x02
#define SYSCMD_ID_END           0xFF

#define SCRPT_VAR_DEC           0x01
#define SCRPT_VAR_HEX           0x02
#define SCRPT_VAR_STR           0x03

#define SCRPT_END               0x00

#define SCRPT_JR                0x18
#define SCRPT_JR_Z              0x28
#define SCRPT_JR_NZ             0x20
#define SCRPT_JR_C              0x38
#define SCRPT_JR_NC             0x30

#define SCRPT_JP                0xC3
#define SCRPT_JP_Z              0xCA
#define SCRPT_JP_NZ             0xC2
#define SCRPT_JP_C              0xDA
#define SCRPT_JP_NC             0xD2

#define SCRPT_SEND              0x41
#define SCRPT_DELAY             0x42
#define SCRPT_WAITBEFORESEND    0x43
#define SCRPT_WAITMAX           0x44
#define SCRPT_REPTMAX           0x45
#define SCRPT_ANSWER            0x46
#define SCRPT_EVENT             0x47
#define SCRPT_LET               0x48
#define SCRPT_CMP               0x49

#define SCRPT_LET_SCRPT_VAR         0x10
#define SCRPT_LET_RECIVED_VAR_DEC   0x11
#define SCRPT_LET_RECIVED_VAR_HEX   0x12
#define SCRPT_LET_VALUE             0x13


