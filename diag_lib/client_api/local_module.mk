#===================================================================================================
#
#   Module Name:  local_module.mk
#
#   General Description: main makefile for libdg_client_api
#
#===================================================================================================
LOCAL_PATH := $(call my-dir)

#===================================================
# Config
#===================================================
include $(LOCAL_PATH)/../../diagd/dg_cfg.mk

DG_FLAGS := $(DG_CFG_FLAGS)
DG_FLAGS += -D_GNU_SOURCE

ifeq ($(DG_DEBUG), 1)
    DG_FLAGS += -DDG_DEBUG=1
endif

#===================================================
# dg_client_api library
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libdg_client_api

LOCAL_CFLAGS := $(DG_FLAGS)

DG_CLIENT_EXPORT_INC := \
  ../../diagd/platform/engine/hdr \
  common/hdr \
  
DG_CLIENT_INC := $(DG_CLIENT_EXPORT_INC)
DG_CLIENT_INC += \
  ../../diagd/common/engine/hdr \
  common/pal/hdr \
  platform/hdr

LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/, $(DG_CLIENT_INC))

LOCAL_EXPORT_C_INCLUDE_DIRS := $(addprefix $(LOCAL_PATH)/, $(DG_CLIENT_EXPORT_INC))

LOCAL_SRC_FILES := \
  common/src/dg_client_api.c \
  platform/src/dg_pal_client_api.c

include $(BUILD_STATIC_LIBRARY)

