#===================================================================================================
#
#   Module Name:  local_module.mk
#
#   General Description: main makefile for diag libs
#
#===================================================================================================
LOCAL_PATH := $(call my-dir)

#===================================================
# DIAG Config
#===================================================
include $(LOCAL_PATH)/../diagd/dg_cfg.mk

DG_FLAGS := $(DG_CFG_FLAGS)
DG_FLAGS += -D_GNU_SOURCE

#===================================================
# DIAG CLIENT API
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libdiagapi

LOCAL_CFLAGS := $(DG_FLAGS)

ifeq ($(DG_DEBUG), 1)
    LOCAL_CFLAGS += -DDG_DEBUG=1
endif

DG_CLIENT_EXPORT_INC := \
  ../diagd/platform/engine/hdr \
  client_api/common/hdr \
  
DG_CLIENT_INC := $(DG_CLIENT_EXPORT_INC)
DG_CLIENT_INC += \
  ../diagd/common/engine/hdr \
  client_api/common/pal/hdr \
  client_api/platform/hdr

LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/, $(DG_CLIENT_INC))

LOCAL_EXPORT_C_INCLUDE_DIRS := $(addprefix $(LOCAL_PATH)/, $(DG_CLIENT_EXPORT_INC))

LOCAL_SRC_FILES := \
  client_api/common/src/dg_client_api.c \
  client_api/platform/src/dg_pal_client_api.c

include $(BUILD_STATIC_LIBRARY)

