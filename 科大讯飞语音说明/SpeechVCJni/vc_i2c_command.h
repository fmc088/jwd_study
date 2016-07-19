#ifndef VOICE_CLEAN_IIC_COMMAND_H_
#define VOICE_CLEAN_IIC_COMMAND_H_

#ifndef BOOL
#define BOOL int
#endif

#define FALSE               0
#define TRUE                1

typedef signed   short      int16_t   ;
typedef unsigned short      uint16_t  ;
typedef signed   long       vc_int32_t   ;
typedef unsigned long       vc_uint32_t  ;

#ifdef __cplusplus
extern "C" { 
#endif 
/*------------------------------------------------------------------------------
  The following macros should be used with each function's definition
------------------------------------------------------------------------------*/
#define GLOBAL
#define STATIC              static

/*------------------------------------------------------------------------------
  The following definitions should be used for each parameter in a
  function's parameters list
------------------------------------------------------------------------------*/
#define IN_                 const
#define OUT
#define IO_

/*------------------------------------------------------------------------------
  Definitions of memory types - irrelevant for host side code
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Definitions of basic types
------------------------------------------------------------------------------*/

#define CMD_SET(item)   ((item) & ~0x0100)
#define CMD_GET(item)   ((item) |  0x0100)
#define CMD_MASK        (~(CMD_SET(0)|CMD_GET(0)))
#define CMD_ITEM(cmd)   ((cmd) & CMD_MASK)
#define CMD_REPLY 1
#define mkword(reg, val)	((val >> 8) | (reg << 1) | ((val & 0xFF) << 8))


// Retrieve the app and module id from an app_module_id
#define GET_APP_ID(app_module_id)    ((app_module_id)&~0xFF)
#define GET_MODULE_ID(app_module_id) ((app_module_id)& 0xFF)

// Reserved app IDs
#define APP_ID(a,b,c,d) (vc_uint32_t)(((vc_uint32_t)((a)-0x20)<<8)| \
                                   ((vc_uint32_t)((b)-0x20)<<14)| \
								   ((vc_uint32_t)((c)-0x20)<<20)| \
								   ((vc_uint32_t)((d)-0x20)<<26))
#define APP_ID_SOS  APP_ID('S','O','S',' ')
#define APP_ID_CTRL APP_ID('C','T','R','L')
#define APP_ID_STRM APP_ID('S','T','R','M')
#define APP_ID_CAPT APP_ID('C','A','P','T')
#define APP_ID_GVAD APP_ID('G','V','A','D')
#define APP_ID_PLBK APP_ID('P','L','B','K')
#define APP_ID_BROADCAST     0xFFFFFF00 // to broadcast commands to all apps

// Reserved module IDs
#define MODULE_ID_APP        0    // to send commands to the app
#define MODULE_ID_BROADCAST  0xFF // to broadcast commands to all modules
#define Q8_23(v) ((v) << 23)


typedef enum {
	capt_input_mic   =  1,
	capt_nr          =  2,
	capt_output      =  3
} CaptModuleId;

#define mic_to_capt		0
#define capt_to_host	1
#define capt_to_i2s		1
#define capt_to_i2sf	1
#define i2sf_to_plbk	2
#define plbk_to_i2s		3

typedef enum {
	MODULE_APP_CMD_RESERVED        =0,
	MODULE_APP_MODULE_CREATE       =1,
	MODULE_APP_ROUTE_SIGNAL        =2,
	MODULE_APP_EXECUTION_ORDER     =4,
	MODULE_APP_STRUCT_INT16_SIZE_OF=5,
	MODULE_APP_RATE                =6,
	MODULE_APP_INIT_APP            =7,
	MODULE_APP_SCRATCH_PARAMS      =8,
	MODULE_APP_MODULES_HEADER      =9,
	MODULE_APP_MODULE_CONNECTIONS  =10,
	MODULE_APP_FULL_CONFIG         =11,
	MODULE_APP_RESTORE_APP         =12,
	MODULE_APP_SCRATCH_PTR         =13,
	MODULE_APP_LOGGING_INIT        =15,
	MODULE_APP_LOGGING_ENABLE      =16,
	MODULE_APP_HEAP_CHECK          =17,
	MODULE_APP_FRAME_CYCLES        =18
} ModuleAppCmd;

typedef enum {
	MODULE_CMD_RESERVED               = 0x0000,
	MODULE_CMD_STRUCT_INT16_SIZE_OF   = 0x0001,
	MODULE_CMD_INIT_DATA              = 0x0002,
	MODULE_CMD_EXTRA_INT16_SIZE_OF    = 0x0003,
	MODULE_CMD_INITIALIZE             = 0x0004,
	MODULE_CMD_CLEAR_STATE            = 0x0005,
	MODULE_CMD_DEFAULT_CONFIG         = 0x0006,
	MODULE_CMD_ACTIVE                 = 0x0007,
	MODULE_CMD_SAMPLE_RATE            = 0x0008,
	MODULE_CMD_INPUT_RATE             = 0x0009,
	MODULE_CMD_FULL_CONFIG            = 0x000A,
	MODULE_CMD_MODULE_TYPE            = 0x000B,
	MODULE_CMD_VERSION_ID             = 0x000C,
	MODULE_CMD_OUTPUT_STREAM          = 0x000F,
	MODULE_CMD_INPUT_STREAM0          = 0x0010,
	MODULE_CMD_INPUT_STREAM1          = 0x0011,
	MODULE_CMD_INPUT_STREAM2          = 0x0012,
	MODULE_CMD_INPUT_STREAM3          = 0x0013,
	MODULE_CMD_INPUT_STREAM4          = 0x0014,
	MODULE_CMD_INPUT_STREAM5          = 0x0015,
	MODULE_CMD_INPUT_STREAM6          = 0x0016,
	MODULE_CMD_INPUT_STREAM7          = 0x0017,
	MODULE_CMD_SCRATCH_ALIGNMENT_OF   = 0x0026,
	MODULE_CMD_SCRATCH_INT16_SIZE_OF  = 0x0027,
	MODULE_CMD_LOGGING_INIT           = 0x0029,
	MODULE_CMD_LOGGING_ENABLE         = 0x002A,
	MODULE_CMD_FRAME_CYCLES           = 0x002C
} ControlModuleCommandCode;

typedef enum {

	CONTROL_APP_CMD_RESERVED      =  0,
	//  CONTROL_APP_SUSPEND         =  1,
	//  CONTROL_APP_RESUME          =  2,
	CONTROL_APP_VERSION           =  3,
	CONTROL_APP_EXEC_FILE         =  4,
	CONTROL_APP_OCLA_ENABLE       =  5,
	CONTROL_APP_MEMORY            =  6,
	CONTROL_APP_CLOCK	        =  7,

	CONTROL_APP_I2C_TUNNEL_CONFIG =  8,
	CONTROL_APP_I2C_TUNNEL_DATA   =  9,
	CONTROL_APP_I2C_TUNNEL_APPLY  = 10,

	CONTROL_APP_LOGGING_INIT      = 20,
	CONTROL_APP_LOGGING_ENABLE    = 21,

	SOS_TASK_CREATE               = 35,
	SOS_TASK_PRIORITY             = 36,
	SOS_EXEC_DUP                  = 37,
	SOS_EXEC_FREE                 = 38,
	SOS_EXEC_PARM                 = 39,
	SOS_JIFFIES                   = 41,
	SOS_SIGNAL                    = 42,
	SOS_SIGNAL_ALL                = 43,
	SOS_ENABLE_IRQ_NR             = 44,
	SOS_DISABLE_IRQ_NR            = 45,
	SOS_RESOURCE                  = 47,
	SOS_TIME                      = 50,
	SOS_TASK_SLEEP_JIFFIES        = 51,

	CONTROL_APP_USB_START         = 60,
	CONTROL_APP_USB_STOP	        = 61,

} ControlAppCommandCode;

typedef enum IvnrCmdCodeTag {
	IVNR_CMD_SIN_OUTPUT = 0x0040,
	IVNR_CMD_NR = 0x0041,
	IVNR_CMD_WATER_MARK = 0x0042,
	IVNR_CMD_BLINK = 0x0043,
	IVNR_CMD_IO_CONTROL = 0x0044,
	IVNR_CMD_NR_OUTPUT = 0x0045,
	IVNR_CMD_SESSION_DATA = 0x0046,
	IVNR_CMD_WATCHDOG = 0x0047,
	IVNR_CMD_I2C_CONTROL = 0x0048

} IvnrCmdCode;
#define MODULE_RC_FIRST -1024
#define STREAMER_APP_RESET                      1
#define STREAMER_APP_MODULE_APP_ENABLE			22
#define STREAMER_APP_MODULE_APP_DESTROY			23
#define STREAMER_APP_MODULE_APP_HEADER			24
#define STREAMER_APP_STREAM_DESTROY				33
#define MODULE_RC_STREAM_ID_NOT_CREATED			(MODULE_RC_FIRST-12)
#define MODULE_RC_BAD_APP_ID					(MODULE_RC_FIRST- 7)
#define STREAMER_APP_GET_MODULE_APP_HEADER	CMD_GET(STREAMER_APP_MODULE_APP_HEADER)
#define OUTPUT_DIGIT_X24					2048
#define IVNR_CMD_GET_WATCHDOG				CMD_GET(IVNR_CMD_WATCHDOG)
#define IVNR_CMD_SET_WATCHDOG				CMD_SET(IVNR_CMD_WATCHDOG)
#define IVNR_CMD_GET_I2C_CONTROL			CMD_GET(IVNR_CMD_I2C_CONTROL)
#define IVNR_CMD_SET_I2C_CONTROL			CMD_SET(IVNR_CMD_I2C_CONTROL)
// The Command type may be used to point to commands of arbitrary
// sizes, for example:
// COMMAND_OF_SIZE(5) cmd
// Command *ptr = (Command *)&cmd;

#define MAX_COMMAND_SIZE 13
#if 0
#define COMMAND_OF_SIZE(n)   \
struct {                     \
	vc_int32_t   num_16b_words:16;\
	vc_uint32_t  command_id:15;   \
	vc_uint32_t  reply:1;         \
	vc_uint32_t  app_module_id;   \
	vc_uint32_t  data[n] ;        \
}
#else

#define COMMAND_OF_SIZE(n)   \
struct {                     \
	int16_t   num_16b_words;   \
	uint16_t  command_id;      \
	vc_uint32_t  app_module_id;   \
	vc_uint32_t  data[n] ;        \
}
#endif
// The maximum number of 32-bit data elements that a command can contain
typedef COMMAND_OF_SIZE(MAX_COMMAND_SIZE) Command_t;


/*
typedef struct { 
	vc_int32_t   num_32b_words:16;
	vc_uint32_t  command_id:15;   
	vc_uint32_t  reply:1;         
	vc_uint32_t  app_module_id;   
	vc_uint32_t  data[MAX_COMMAND_SIZE];
} Command_t;
*/
void reset_time_out(void);
int get_time_wait(void);
int test_version(Command_t *command);
int test_ret(Command_t *command);
int send_cmd (Command_t *cmd, vc_uint32_t  app_module_id, vc_uint32_t  command_id, vc_uint32_t num_32b_words);
#ifdef __cplusplus
}
#endif 
#endif // VOICE_CLEAN_IIC_COMMAND_H_


