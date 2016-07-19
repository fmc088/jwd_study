/*------------------------------------------------------------------------------

	File Name:  vc_ctrl_host.h

Description:  API for voice clean host i2c ctrl code
------------------------------------------------------------------------------*/
#ifndef VOICE_CLEAN_CTRL_HOST_H_
#define VOICE_CLEAN_CTRL_HOST_H_

#include "vc_i2c_command.h"

#ifdef __cplusplus
extern "C" { // only need to export C interface if
// used by C++ source code
#endif 

#define MODULE_RC_I2C_FIRST	-2048

typedef enum IvnrI2CRetTypeTag {
	IV_I2C_RET_ACK_CMD = MODULE_RC_I2C_FIRST - 4,
	IV_I2C_RET_NOSUPPORT = MODULE_RC_I2C_FIRST -3,
	IV_I2C_RET_FAILED = MODULE_RC_I2C_FIRST - 2,
	IV_I2C_RET_BUSY = MODULE_RC_I2C_FIRST -1,
	IV_I2C_RET_SUCCESS_STANDBY = 0x00,
} IvnrI2CRetType;

// 复位操作耗时1s
extern int VCReset(Command_t * command);
/*------------------------------------------------------------------------------
  Function: VCWaitReady

  Purpose:    waiting for servant i2c control module ready. 400ms after servant
              turning on, it is ready for receiving orders from the host. so we 
			  need to send command to check ready. if servant is ready, function 
			  returns 1 immediately, otherwise it will continue to check until 
			  the timeout, and return 0.

  Inputs:   cmd				- i2c command struct object pointer
			msecond			- timeout length, in milliseconds

  Outputs:  return BOOL 		- servant ready : 1; 
                                  waiting timeout : 0
------------------------------------------------------------------------------*/
extern BOOL VCWaitReady(Command_t * command, int msecond);

/*------------------------------------------------------------------------------
  Function: VCGetVersion

  Purpose:  区分讯飞静音模块是二版（仅降噪）还是三版（带唤醒、回声消除）

  Inputs:   command				- i2c command struct object pointer

  Outputs:  return				- 2  二版（仅降噪）
								- 3  三版（带唤醒、回声消除）
								- 0  未知，或I2C读取失败
								  
------------------------------------------------------------------------------*/
extern int VCGetVersion(Command_t * command);

/*------------------------------------------------------------------------------
  Function: VCChangeWorkMode

  Purpose:  send command to Servant for changing the working mode : 
                         1：外挂模式、       2：顶灯模式、       3：车机模式

  Inputs:   cmd                                - i2c command struct object pointer
           work_mode_id    - id of work mode
                            
         WORK_MODE_PERIPHERAL: 外挂模式
         WORK_MODE_TOPLIGHT: 顶灯模式
         WORK_MODE_HOST：车机模式

  Outputs:       return                         - succeed : 0; 
         `                                                       failed  : error code < 0
------------------------------------------------------------------------------*/

typedef enum
{
	WORK_MODE_PERIPHERAL				= 0x1,
	WORK_MODE_TOPLIGHT					= 0x2,
	WORK_MODE_HOST						= 0x3
} cmd_work_mode_id;

extern int VCChangeWorkMode(Command_t *command, cmd_work_mode_id work_mode_id);

/*------------------------------------------------------------------------------
  Function: VCGetWorkMode

  Purpose:  send command to Servant to get the working mode : 

  Inputs:   cmd                                - i2c command struct object pointer

  Outputs:       return                    - failed  : error code < 0
											succeed  :	1：外挂模式
														2：顶灯模式
														3：车机模式 
------------------------------------------------------------------------------*/
extern int VCGetWorkMode(Command_t * command);

/*------------------------------------------------------------------------------
  Function: VCChangeFunc

  Purpose:  send command to servant for changing the function mode

  Inputs:   cmd				- i2c command struct object pointer
            mode_id	- id of function mode
			
	FUNC_MODE_PASSBY: record witout algorithm 
	FUNC_MODE_NOISECLEAN: noise reduction record function
	FUNC_MODE_PHONE: echo reduction for telephone conversation function
	FUNC_MODE_WAKEUP: echo reduction and wakeup algorithm function

  Outputs:	return			- succeed : 0; 
	`						  failed  : error code < 0
------------------------------------------------------------------------------*/
typedef enum
{
	FUNC_MODE_PASSBY				= 0x0,	//所有功能关闭直接录音
	FUNC_MODE_NOISECLEAN			= 0x1,	//降噪功能
	FUNC_MODE_PHONE					= 0x2,	//通话回声消除功能
	FUNC_MODE_WAKEUP				= 0x3,	//唤醒回声消除功能
} cmd_func_id;
extern int VCChangeFunc(Command_t * command, cmd_func_id func_id);

/*------------------------------------------------------------------------------
  Function: VCGetFunc

  Purpose:  send command to Servant to get the function mode : 

  Inputs:   cmd                                - i2c command struct object pointer

  Outputs:       return                    - failed  : error code < 0
											succeed  :	0：所有功能关闭直接录音
														1：降噪功能
														2：通话回声消除功能
														3：唤醒回声消除功能 
------------------------------------------------------------------------------*/
extern int VCGetFunc(Command_t * command);


/*------------------------------------------------------------------------------
  Function: VCEnableWakeupGpio

  Purpose:  sending a command to servant for wakeup gpio signal config
            in echo reduction and wakeup function

  Inputs:   command				- i2c command struct object pointer
			key_level			- key level is low or high level
									  high level  : 1
									  low level   : 0
		    msecond				- key level length 

  Outputs:  return				- succeed : 0
								  failed  : error code < 0
------------------------------------------------------------------------------*/
extern int VCConfigWakeupGpio(Command_t * command, BOOL key_level, int msecond);

/*------------------------------------------------------------------------------
  Function: VCConfigWakeupID

  Purpose:  sending a command to servant for wakeup id config
            in echo reduction and wakeup function

  Inputs:   command				- i2c command struct object pointer
			wakeup_id			- id of wakeup word
									  0					: iflytek use 你好语音助理
									  0xea0d0471: kai yue zai xian 凯越在线

  Outputs:  return				- succeed : 0
								  failed  : error code < 0
------------------------------------------------------------------------------*/
extern int VCConfigWakeupID(Command_t * command, int wakeup_id);

/*------------------------------------------------------------------------------
  Function: VCGetWakeupSign

  Purpose:  send command to Servant to get the last wakeup sign

  Inputs:   cmd                 - i2c command struct object pointer

  Outputs: 
			return              - failed : error code < 0
								  succeed : 0：调用函数前没有唤醒发生
											1：调用函数前有唤醒发生
			* pmsecond			- the last wakeup signal happened (*pmsecond) millisecond before.
								0 <=（*pmsecond）<= 30000ms
  函数调用时间间隔建议大于200毫秒。函数调用获取最近一次的唤醒信息，调用后，系统会清除唤醒标志和时间信息。
  当唤醒30秒内没有调用函数读取信号，（*pmsecond）将停止在30000，不再增加，因此这个最大值没有实际使用意义。  
------------------------------------------------------------------------------*/
extern int VCGetWakeupSign(Command_t * command, int * pmsecond);

/*------------------------------------------------------------------------------
  Function: VCEnableNR

  Purpose:  sending a command to servant for turning on/off the noise reduction
            algorithm in noise reduction function

  Inputs:   command				- i2c command struct object pointer
			enable_nr			- enable or disable noise reduction algorithm
									  enable  : 1
									  disable : 0

  Outputs:  return				- succeed : 0
								  failed  : error code < 0
------------------------------------------------------------------------------*/
/*************************************************************************************/
/*   VCEnableNR 仅用于版本二，于版本三中调用会造成异常   */
/*************************************************************************************/
extern int VCEnableNR(Command_t * command, BOOL enable_nr);

/*------------------------------------------------------------------------------
  Function: VCSetDACVolume

  Purpose:  sending a command to servant to set output dac gain

  Inputs:   command			- i2c command struct object pointer
            cmd_adc_gain_id	- id of dac gain, for WOKING_MODE_NR_ANALOG

  Outputs:	return			- succeed : 0; 
							  failed  : error code < 0
------------------------------------------------------------------------------*/
typedef enum
{
	DAC_GAIN_N23          = 34,	//-23	dB
	DAC_GAIN_N22          = 35,	//-22	dB
	DAC_GAIN_N21          = 36,	//-21	dB
	DAC_GAIN_N20          = 37,	//-20	dB
	DAC_GAIN_N19          = 38,	//-19	dB
	DAC_GAIN_N18          = 39,	//-18	dB
	DAC_GAIN_N17          = 40,	//-17	dB
	DAC_GAIN_N16          = 41,	//-16	dB
	DAC_GAIN_N15          = 42,	//-15	dB
	DAC_GAIN_N14          = 43,	//-14	dB
	DAC_GAIN_N13          = 44,	//-13	dB
	DAC_GAIN_N12          = 45,	//-12	dB
	DAC_GAIN_N11          = 46,	//-11	dB
	DAC_GAIN_N10          = 47,	//-10	dB
	DAC_GAIN_N09          = 48,	//-9	dB
	DAC_GAIN_N08          = 49,	//-8	dB
	DAC_GAIN_N07          = 50,	//-7	dB	default
	DAC_GAIN_N06          = 51,	//-6	dB
	DAC_GAIN_N05          = 52,	//-5	dB
	DAC_GAIN_N04          = 53,	//-4	dB
	DAC_GAIN_N03          = 54,	//-3	dB
	DAC_GAIN_N02          = 55,	//-2	dB
	DAC_GAIN_N01          = 56,	//-1	dB
	DAC_GAIN_000          = 57,	// 0	dB
	DAC_GAIN_001          = 58,	//+1	dB
	DAC_GAIN_002          = 59,	//+2	dB
	DAC_GAIN_003          = 60,	//+3	dB
	DAC_GAIN_004          = 61,	//+4	dB  麦克风模组，顶灯模式
	DAC_GAIN_005          = 62,	//+5	dB
	DAC_GAIN_006_MAX      = 63	//+6	dB
} cmd_dac_gain_id;
extern int VCSetDACVolume(Command_t * command, cmd_dac_gain_id dac_gain_id);

#ifdef __cplusplus
}
#endif 
#endif  // VOICE_CLEAN_CTRL_HOST_H_
