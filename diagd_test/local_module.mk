LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := diagd_test

LOCAL_CFLAGS := -DDG_DEBUG=1

LOCAL_C_INCLUDES :=

LOCAL_STATIC_LIBRARIES := libdiagapi

LOCAL_SHARED_LIBRARIES :=

LOCAL_REQUIRED_MODULES := diagd

LOCAL_SRC_FILES := dg_test_client.c

include $(BUILD_EXECUTABLE)
