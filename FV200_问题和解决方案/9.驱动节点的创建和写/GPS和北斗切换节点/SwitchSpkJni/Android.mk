LOCAL_PATH:= $(call my-dir)
# 一个完整模块编译
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=com_philips_switchspk_SwitchSpk.c
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)
LOCAL_MODULE := libswitchspk-jni
LOCAL_SHARED_LIBRARIES := libutils liblog
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS :=optional
include $(BUILD_SHARED_LIBRARY)