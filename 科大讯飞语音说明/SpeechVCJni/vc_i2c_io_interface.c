#include "vc_i2c_io_interface.h"
#include "vc_i2c_command.h"
#include <fcntl.h>
//#include "hw_api.h"

#define I2CADDR (0x47 << 1)

/************************************************************************************
intermediate function
*************************************************************************************/
/*------------------------------------------------------------------------------
  Function:   VCI2CWrite

  Purpose:    用户实现的接口函数,功能为向I2C总线按照固定的时序写数据,时序如下图：
  
 |START|0x8E(8bit)|ACK|register_address高8位|ACK|register_address低8位|ACK|buffer[0]|ACK|buffer[1]|ACK|...|buffer[size-1]|ACK|STOP|
		

  Inputs:     register_address	- 16bit的控制地址 
              buffer			- 写入I2C总线数据的存储地址
			  size				- 写入I2C总线数据的长度, 1 for Byte

  Outputs:	  return			- succeed : 1; 
								            failed  : 0;
------------------------------------------------------------------------------*/
int VCI2CWrite(int register_address, unsigned char *buffer, int size)
{
	   //return icI2C_Send(I2CADDR, register_address, buffer, size);
	   LOGI("func:VCI2CWrite  line:%d",__LINE__);
	   unsigned char * i2c_data = buffer;
	   
	   xf6000_data* io_data;
	   int xfLength = sizeof(xf6000_data);
	   int cmd_size = sizeof(Command_t);
	   LOGI("VCI2CWrite->size of struct Command_t : %d",cmd_size);	
	   LOGI("VCI2CWrite->size of struct xf6000_data : %d",xfLength);	  
	   io_data = (xf6000_data*)malloc(xfLength);
	   
	   io_data->offset = register_address;
	   io_data->size = size;
	   memcpy(io_data->buf,buffer,size);
	   LOGI("func:VCI2CWrite  line:%d  ,buf size = %d",__LINE__,size);	
	   int fd;
	   int err,ret=0;
	   fd = open(XF6000_DEV_PATH,O_RDWR);
	   if(fd>0)
	   	{
	   		  err = ioctl(fd,XF6000_IOC_WRITE,(unsigned long)io_data);
	   		  if(err <0)
	   			{
	   				 LOGE("func:VCI2CWrite  line:%d  ,ioctl err = %d",__LINE__,err);
	   				 ret = 0; 
	   			}else
	   			{
	   				
	   				 LOGI("func:VCI2CWrite  line:%d  ,ioctl success",__LINE__);	
             ret = 1;
	   			}
	   			close(fd);
	   	}else
	   	{
	   			LOGE("func:VCI2CWrite  line:%d  ,open dev failed",__LINE__);
	   			ret = 0;
	   	}
	   
	   free(io_data);
	   return ret;
}


/*------------------------------------------------------------------------------
  Function:   VCI2CRead

  Purpose:     用户实现的接口函数,功能为从I2C总线按照固定的时序读数据,时序如下图：

 |START|0x8E(8bit)|ACK|register_address高8位|ACK|register_address低8位|ACK|START|0x8F(8bit)|ACK|buffer[0]|ACK|buffer[1]|ACK|...|buffer[size-1]|ACK|STOP|


  Inputs:     register_address	- 16bit的控制地址
              buffer			- 从I2C总线读取数据的存储地址
			  size				- 从I2C总线读取数据的长度, 1 for Byte

  Outputs:	  return			- succeed : 1; 
								  failed  : 0;
------------------------------------------------------------------------------*/
int VCI2CRead(int register_address, unsigned char*buffer, int size)
{
	   //return icI2C_Recv(I2CADDR, register_address, buffer, size);
	   LOGI("func:VCI2CRead  line:%d",__LINE__);

	   unsigned char * i2c_data = buffer;
	   
	   xf6000_data* io_data;
	   int xfLength = sizeof(xf6000_data);
	   LOGI("VCI2CRead->size of struct xf6000_data : %d",xfLength);
	   io_data = (xf6000_data*)malloc(xfLength);
	   io_data->offset = register_address;
	   io_data->size = size;
	   memcpy(io_data->buf,buffer,size);
	   
	   int fd;
	   int err,ret=0;
	   fd = open(XF6000_DEV_PATH,O_RDWR);
	   if(fd>0)
	   	{
	   		  err = ioctl(fd,XF6000_IOC_READ,(unsigned long)io_data);
	   		  if(err <0)
	   			{
	   				 LOGE("func:VCI2CRead line:%d  ,ioctl err = %d",__LINE__,err);
	   				 ret = 0; 
	   			}else
	   			{
	   				  LOGI("func:VCI2CRead line:%d  ,ioctl success,size = %d",__LINE__,io_data->size);	
	   				  
	   				  memcpy(i2c_data,io_data->buf,io_data->size);
	   				  
	   				  ret = 1;
	   			}
	   			 close(fd);
	   	}else
	   	{
	   			LOGE("func:VCI2CRead  line:%d  ,open dev failed",__LINE__);
	   			ret = 0;
	   	}
	   free(io_data);
	   return ret;
	
}


/*------------------------------------------------------------------------------
  Function:   VCI2CSleep

  Purpose:    The I2C driver layer interface which user need to develop.This function
            just call the sleep fun on host. 

  Inputs:     intreval_ms		- time for sleep, 1 for 1 millisecond 
 
  Outputs:	  return			- void
------------------------------------------------------------------------------*/
void VCI2CSleep (int intreval_ms)
{
	LOGI("func:VCI2CSleep  line:%d",__LINE__);
	
	   /*
	   xf6000_data* io_data;
	   io_data = (xf6000_data*)malloc(sizeof(xf6000_data));
	   io_data->offset = intreval_ms;

	   int fd;
	   int err,ret=0;
	   fd = open(XF6000_DEV_PATH,O_RDONLY);
	   if(fd>0)
	   	{
	   		  err = ioctl(fd,XF6000_IOC_DELAY,(char*)io_data);
	   		  if(err <0)
	   			{
	   				 LOGE("func:VCI2CRead  line:%d  ,ioctl err = %d",__LINE__,err);
	   				  ret = 0; 
	   			}else
	   			{
	   				 LOGI("func:VCI2CRead  line:%d  ,ioctl success",__LINE__);	
	   				 ret = 1;
	   			}
	   			close(fd);
	   	}else
	   	{
	   			LOGE("func:VCI2CRead  line:%d  ,open dev failed",__LINE__);
	   			ret = 0;
	   	}
	   	
	   	free(io_data);
	   	*/
	   	usleep(intreval_ms*1000);
	   // return ret;	
	//HW_WaitTimeoutMS(intreval_ms);
}
