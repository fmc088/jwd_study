LOCAL_PATH:= $(call my-dir)
# һ������ģ�����
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=com_android_internal_policy_impl_PhoneWindowManager.c
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)
LOCAL_MODULE := libkillfill_jni
LOCAL_SHARED_LIBRARIES := libutils liblog libcutils
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS :=optional
include $(BUILD_SHARED_LIBRARY)