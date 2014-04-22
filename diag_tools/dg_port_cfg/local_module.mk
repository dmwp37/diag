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

LOCAL_MODULE := dg_port_cfg

LOCAL_CFLAGS :=

DG_INT_LOOP_INC := ../../out

LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/, $(DG_INT_LOOP_INC))

LOCAL_STATIC_LIBRARIES := libdg_loop libdg_dbg
LOCAL_LDLIBS +=

LOCAL_REQUIRED_MODULES :=

LOCAL_SRC_FILES := dg_port_cfg.c

include $(BUILD_EXECUTABLE)

