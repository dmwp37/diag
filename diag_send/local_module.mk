LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := diag_send

LOCAL_CFLAGS := -DDG_DEBUG=0

LOCAL_C_INCLUDES :=

LOCAL_STATIC_LIBRARIES := libdiagapi

LOCAL_SHARED_LIBRARIES :=

LOCAL_REQUIRED_MODULES := diagd

LOCAL_SRC_FILES := diag_send_main.c

include $(BUILD_EXECUTABLE)
