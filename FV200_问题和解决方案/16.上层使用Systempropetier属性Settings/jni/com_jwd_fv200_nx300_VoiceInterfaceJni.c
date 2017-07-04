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
#include <cutils/compiler.h>
#include <cutils/properties.h>

#define LOG_TAG "VoiceBroadCast_JNI"
#define LOGV(LOG_TAG,args...) ALOGV(LOG_TAG,##args)
#define LOGD(LOG_TAG,args...) ALOGD(LOG_TAG,##args)
#define LOGI(LOG_TAG,args...) ALOGI(LOG_TAG,##args)
#define LOGW(LOG_TAG,args...) ALOGW(LOG_TAG,##args)
#define LOGE(LOG_TAG,args...) ALOGE(LOG_TAG,##args)

 
JNIEXPORT void JNICALL Java_com_jwd_fv200_nx300_VoiceInterfaceJni_voicePropertiesSet
  (JNIEnv *env, jclass clazz,jstring keyJ, jstring valJ){
    int err;
    const char* key;
    const char* val;
    ALOGE("--jwd----VoiceInterfaceJni_voicePropertiesSet start");
    if (keyJ == NULL) {
    	ALOGE("--jwd----key must not be null.");
        //jniThrowNullPointerException(env, "key must not be null.");
        return ;
    }
    key = (*env)->GetStringUTFChars(env,keyJ, NULL);

    if (valJ == NULL) {
        val = "";       /* NULL pointer not allowed here */
    } else {
        val = (*env)->GetStringUTFChars(env,valJ, NULL);
    }

    err = property_set(key, val);

    (*env)->ReleaseStringUTFChars(env,keyJ, key);

    if (valJ != NULL) {
        (*env)->ReleaseStringUTFChars(env,valJ, val);
    }

    if (err < 0) {
    	ALOGE("--jwd----failed to set system property");
        //jniThrowException(env, "java/lang/RuntimeException",
       //                   "failed to set system property");
    }
 }

