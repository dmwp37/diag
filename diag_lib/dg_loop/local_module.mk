#===================================================================================================
#
#   Module Name:  local_module.mk
#
#   General Description: main makefile for libdg_loop
#
#===================================================================================================
LOCAL_PATH := $(call my-dir)

DG_FLAGS := -D_GNU_SOURCE

#===================================================
# dg_loop library
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libdg_loop

LOCAL_CFLAGS := $(DG_FLAGS)

DG_LOOP_EXPORT_INC := \
  ../../diagd/platform/engine/hdr \
  hdr

DG_LOOP_INC := $(DG_LOOP_EXPORT_INC)

LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/, $(DG_LOOP_INC))

LOCAL_EXPORT_C_INCLUDE_DIRS :=  $(addprefix $(LOCAL_PATH)/, $(DG_LOOP_EXPORT_INC))

LOCAL_STATIC_LIBRARIES := libdg_dbg

LOCAL_SRC_FILES := \
  src/dg_loop_api.c \
  src/dg_loop_cfg.c \
  src/dg_loop_detect.c\
  src/dg_loop_test.c

include $(BUILD_STATIC_LIBRARY)

