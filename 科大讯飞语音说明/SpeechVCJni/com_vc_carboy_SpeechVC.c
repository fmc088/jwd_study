/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>

#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "vc_ctrl_api.h"
#include "vc_i2c_io_interface.h"
#include "vc_i2c_command.h"


static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

 JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1native_1init(JNIEnv* env, jobject thiz)
  {
  	  LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_native_1init");
      int ret = -1;
      
      return ret;
  }
 
 JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1initVC(JNIEnv* env, jobject thiz)
  {
  	  LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_initVC");
      vc_int32_t ret = -1;
	    Command_t cmd ={0};
	    ret = VCWaitReady(&cmd,500);//设置超时为400ms，如果模块已经就绪，会立即返回1，否则会持续查询直到超时。
	    LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_initVC, ret = %d",ret);
      return ret;
  }
  	
JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1getVersion(JNIEnv * env, jobject thiz)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getVersion");
		 vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCGetVersion(&cmd);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getVersion, ret = %d",ret);
	   return ret;
} 

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1setWorkMode(JNIEnv *env, jobject thiz, int workMode)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setWorkMode");
		 vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCChangeWorkMode(&cmd,workMode);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setWorkMode,ret = %d",ret);
	   return ret;
}

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1getWorkMode(JNIEnv * env, jobject thiz)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getWorkMode");
		 vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCGetWorkMode(&cmd);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getWorkMode, ret = %d",ret);
	   return ret;
}

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1setFuncMode(JNIEnv * env, jobject thiz, int funcMode)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setFuncMode");
		 vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCChangeFunc(&cmd,funcMode);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setFuncMode, ret = %d",ret);
	   return ret;
}	

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1getFuncMode(JNIEnv * env, jobject thiz)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getFuncMode");
		 vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCGetFunc(&cmd);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_getFuncMode, ret = %d",ret);
	   return ret;
}

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1setDACGain(JNIEnv * env, jobject thiz, int dacGain)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setDACGain");
	   vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCSetDACVolume(&cmd,dacGain);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setDACGain, ret = %d",ret);
	   return ret;
}

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1setWakeupID(JNIEnv * env, jobject thiz, int wkID)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setWakeupID");
	   vc_int32_t ret = -1;
	   Command_t cmd ={0};
	   ret = VCConfigWakeupID(&cmd,wkID);
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_setWakeupID ret = %d",ret);
	   return ret;
}

JNIEXPORT int JNICALL Java_com_vc_carboy_SpeechVC__1startWakeup(JNIEnv * env, jobject thiz)
{
	   LOGI("LSC_SHENZHEN_VC Java_com_iflytek_jwd_SpeechVCJniNative_startWakeup");
     int ret = -1;



	   return ret;
}

 

 
