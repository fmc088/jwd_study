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
#include <cutils/log.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#define LOGV(fmt,args...) ALOGV(fmt,##args)
#define LOGD(fmt,args...) ALOGD(fmt,##args)
#define LOGI(fmt,args...) ALOGI(fmt,##args)
#define LOGW(fmt,args...) ALOGW(fmt,##args)
#define LOGE(fmt,args...) ALOGE(fmt,##args)

#define SP_SWITCHSPK_DEV_PATH "/dev/philips_spk" 

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
 
static int write_int(char const *path, int value)
{
	int fd;
	static int already_warned;

	already_warned = 0;

	//LOGI("write_int: path %s, value %d", path, value);
	fd = open(path, O_RDWR);

	if (fd >= 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", value);
		int amt = write(fd, buffer, bytes);
		close(fd);
		return amt == -1 ? -errno : 0;
	} else {
		if (already_warned == 0) {
			LOGE("write_int failed to open dev\n");
			already_warned = 1;
		}
		return -errno;
	}
}
 
 JNIEXPORT void JNICALL Java_com_sps_switchspk_SwitchSpk_setSpk_1native
  (JNIEnv * env, jobject thiz , jint spSpkMode)
  {
  	//LOGV("lsc-shenzhen SwitchSpk spSpkMode = %d",spSpkMode);
    int err = 0;
	  pthread_mutex_lock(&g_lock);
	  err = write_int(SP_SWITCHSPK_DEV_PATH, spSpkMode);
	  pthread_mutex_unlock(&g_lock);
  	}
 