#include "vc_i2c_io_interface.h"
#include "vc_i2c_command.h"
/************************************************************************************
intermediate function
*************************************************************************************/

#define REPLY_POLL_INTERVAL_MSEC     1
#define REPLY_POLL_TIMEOUT_MSEC   200
static int m_time_out_ms;

void reset_time_out(void)
{
	m_time_out_ms = 0;
}

int get_time_wait(void)
{
	return m_time_out_ms; 
}

static void byte_order_swap_cmd_head(unsigned char * head)
{
	unsigned char t;
	unsigned short hint = 0xFEFF;
	unsigned char *p = (unsigned char *)&hint;
	if (p[0] == (unsigned char)0xFE) {
		// Big Endian platform
		p = head;
		//swap p[0] p[1]
		t = p[0];
		p[0] = p[1];
		p[1] = t;
		//swap p[2] p[3]
		t = p[2];
		p[2] = p[3];
		p[3] = t;
	}
}

static void byte_order_swap_cmd_data(unsigned char * data, vc_uint32_t size)
{
	unsigned char t;
	vc_uint32_t i;
	unsigned short hint = 0xFEFF;
	unsigned char *p = (unsigned char *)&hint;
	if (p[0] == (unsigned char)0xFE) {
		// Big Endian platform
		for (i = 0, p = data; i < size; i += 4, p += 4) {
			// swap p[0] p[3]
			t = p[0];
			p[0] = p[3];
			p[3] = t;
			// swap p[1] p[2]
			t = p[1];
			p[1] = p[2];
			p[2] = t;
		}
	}
}

int send_cmd (Command_t *cmd, vc_uint32_t  app_module_id, vc_uint32_t  command_id, vc_uint32_t num_16b_words)
{
	unsigned int* i2c_data = (unsigned int *)cmd;
	int size = (num_16b_words + 2) * 4;
	int elapsed_ms = 0;
	int ret = 0;

	// at least two words of header
	if (num_16b_words > MAX_COMMAND_SIZE)
	{
		return(-1);
	}
	cmd->num_16b_words = (uint16_t)((command_id&CMD_GET(0)) ? MAX_COMMAND_SIZE : num_16b_words);
	cmd->command_id    = (uint16_t)command_id;
//#if 0
//	cmd->reply         = 0;
//#else
	cmd->command_id = cmd->command_id & 0x7FFF;
//#endif
	cmd->app_module_id = app_module_id;

	// write words 1 to N-1 , to addresses 4 to 4+4*N-1
	byte_order_swap_cmd_data((unsigned char *)&i2c_data[1], (size - 4));
	ret = VCI2CWrite(0x4, (unsigned char *)(&i2c_data[1]), (size - 4));
	if (ret == 0) {
		return -1;
	}
	// write word 0 to address 0o
	byte_order_swap_cmd_head((unsigned char *)&i2c_data[0]);
	ret = VCI2CWrite(0x0, (unsigned char *)(&i2c_data[0]), 4);
	if (ret == 0) {
		return -1;
	}

	while (elapsed_ms < REPLY_POLL_TIMEOUT_MSEC)
	{
		// only read the first word and check the reply bit
		ret = VCI2CRead(0x0, (unsigned char *)&i2c_data[0], 4);
		byte_order_swap_cmd_head((unsigned char *)&i2c_data[0]);
//#if 0
//		if (cmd->reply==1)
//#else
		if (ret != 0 && (cmd->command_id & 0x8000) == 0x8000)
    {
    	LOGI("%d,Command read:success.",__LINE__);
       //#endif
			 break;
		}
		VCI2CSleep(REPLY_POLL_INTERVAL_MSEC);
		elapsed_ms += REPLY_POLL_INTERVAL_MSEC;
	}
	m_time_out_ms += elapsed_ms;

//#if 0
//	if (cmd->reply==1)
//#else
	if ((cmd->command_id & 0x8000) == 0x8000)
//#endif
	{
		LOGI("%d,Command read:got command id.",__LINE__);
		if (cmd->num_16b_words > 0)
		{
			LOGI("%d,Command read:num_16b_words > 0 .",__LINE__);
			ret = VCI2CRead(0x8, (unsigned char *)&i2c_data[2], (cmd->num_16b_words)*4);
			byte_order_swap_cmd_data((unsigned char *)&i2c_data[2], (cmd->num_16b_words)*4);
			if (ret == 0) {
				LOGE("%d,Command read:read error.",__LINE__);
				return -1;
			}
		}
//		VCI2CSleep(REPLY_POLL_INTERVAL_MSEC);
    LOGI("%d,final Command read read success.num of words = %d",__LINE__,cmd->num_16b_words);
		return(cmd->num_16b_words);
	}
	return(-1);
}

int test_version(Command_t *command)
{
	int ret_val;
	ret_val = send_cmd(command, (vc_uint32_t)APP_ID_CTRL, CMD_GET(CONTROL_APP_VERSION), 0);

	if (ret_val<0)
	{
//		printf ("failed to get FW version\n");
		return ret_val; /* err code*/
	}

//	printf ("version number: %d.%d.%d.%d\n",command->data[0],
//		command->data[1], command->data[2], command->data[3]);

	return(0);
}
