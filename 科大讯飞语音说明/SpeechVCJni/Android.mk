LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
                  com_vc_carboy_SpeechVC.c \
                  vc_ctrl_api.c \
                  vc_i2c_command.c \
                  vc_i2c_io_interface.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
                  
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)
LOCAL_MODULE := libspeechvcjni
LOCAL_SHARED_LIBRARIES := libutils liblog
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS :=optional
include $(BUILD_SHARED_LIBRARY)