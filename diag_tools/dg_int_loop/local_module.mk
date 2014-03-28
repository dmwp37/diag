#===================================================================================================
#
##   Module Name:  local_module.mk
#
##   General Description: main makefile for dg_int_loop
#
##===================================================================================================
LOCAL_PATH := $(call my-dir)

#===================================================
# dg_int_loop
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := dg_int_loop

LOCAL_CFLAGS :=

LOCAL_C_INCLUDES :=

LOCAL_STATIC_LIBRARIES := libdg_loop
LOCAL_LDLIBS += -largp

LOCAL_REQUIRED_MODULES :=

LOCAL_SRC_FILES := dg_int_loop.c

include $(BUILD_EXECUTABLE)

