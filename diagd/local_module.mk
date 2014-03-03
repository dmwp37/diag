#===================================================================================================
#
#   Module Name:  local_module.mk
#
#   General Description: main makefile for diagd and client API
#
#===================================================================================================
LOCAL_PATH := $(call my-dir)

#===================================================
# DIAG Config
#===================================================
include $(LOCAL_PATH)/dg_cfg.mk

DG_FLAGS := $(DG_CFG_FLAGS)


#===================================================
# DIAG CLIENT API
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libdiagapi

LOCAL_CFLAGS := $(DG_FLAGS)

ifeq ($(DG_DEBUG), 1)
    LOCAL_CFLAGS += -DDG_DEBUG=1
endif

DG_CLIENT_INC := \
  common/client_api/hdr \
  common/client_api/pal/hdr \
  common/engine/hdr \
  platform/pal/client_api/hdr

LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/, $(DG_CLIENT_INC))

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    common/client_api/src/dg_client_api.c \
    platform/pal/client_api/src/dg_pal_client_api.c

include $(BUILD_STATIC_LIBRARY)


#===================================================
# DIAG DAEMON
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := diagd

LOCAL_CFLAGS := $(DG_FLAGS)
ifeq ($(DG_DEBUG), 1)
    LOCAL_CFLAGS += -DDG_DEBUG=1
endif

DG_DIAG_INC := \
  ../out \
  common/engine/hdr \
  common/engine/pal/hdr \
  common/handlers/hdr \
  common/drivers/hdr \
  platform/handlers/hdr \
  platform/drivers/hdr \
  platform/pal/client_api/hdr

LOCAL_C_INCLUDES :=  $(addprefix $(LOCAL_PATH)/, $(DG_DIAG_INC))

LOCAL_STATIC_LIBRARIES := libdiagapi
LOCAL_SHARED_LIBRARIES :=

#Common Engine
DG_ENGINE_SRC := \
  common/engine/src/dg_aux_engine.c \
  common/engine/src/dg_aux_util.c \
  common/engine/src/dg_client_comm.c \
  common/engine/src/dg_dbg.c \
  common/engine/src/dg_engine_util.c \
  common/engine/src/dg_main.c \
  common/engine/src/dg_socket.c

#Engine PAL for the specific platform
#Make sure you checked out the correct branch for the platform
DG_ENGINE_PAL_SRC := \
  platform/pal/engine/src/dg_main_task.c \
  platform/pal/engine/src/dg_pal_aux_engine.c \
  platform/pal/engine/src/dg_pal_dbg.c \
  platform/pal/engine/src/dg_pal_util.c \
  platform/pal/engine/src/dg_pal_security.c

#Common Handlers
DG_HANDLERS_CMN_SRC := \
  common/handlers/src/dg_bios.c \
  common/handlers/src/dg_button.c \
  common/handlers/src/dg_debug_level.c \
  common/handlers/src/dg_dimm.c \
  common/handlers/src/dg_eeprom.c \
  common/handlers/src/dg_ext_loop.c \
  common/handlers/src/dg_fpga.c \
  common/handlers/src/dg_gpio.c \
  common/handlers/src/dg_i2c.c \
  common/handlers/src/dg_int_loop.c \
  common/handlers/src/dg_led.c \
  common/handlers/src/dg_pci.c \
  common/handlers/src/dg_phy.c \
  common/handlers/src/dg_ping.c \
  common/handlers/src/dg_pttm.c \
  common/handlers/src/dg_reset.c \
  common/handlers/src/dg_rtc.c \
  common/handlers/src/dg_suspend.c \
  common/handlers/src/dg_usb.c \
  common/handlers/src/dg_version.c \
  common/handlers/src/dg_test_engine.c

#Platform Specific Handlers
DG_HANDLERS_PLAT_SRC := \
  platform/handlers/src/dg_handler_table.c \
  platform/handlers/src/dg_aux_cmd.c

DG_DRIVERS_CMN_SRC := \
  common/drivers/src/dg_drv_util.c

#Platform Specific Drivers
DG_DRIVERS_PLAT_SRC := \
  platform/drivers/src/dg_cmn_drv_bios.c \
  platform/drivers/src/dg_cmn_drv_button.c \
  platform/drivers/src/dg_cmn_drv_debug_level.c \
  platform/drivers/src/dg_cmn_drv_dimm.c \
  platform/drivers/src/dg_cmn_drv_eeprom.c \
  platform/drivers/src/dg_cmn_drv_ext_loop.c \
  platform/drivers/src/dg_cmn_drv_fpga.c \
  platform/drivers/src/dg_cmn_drv_gpio.c \
  platform/drivers/src/dg_cmn_drv_i2c.c \
  platform/drivers/src/dg_cmn_drv_int_loop.c \
  platform/drivers/src/dg_cmn_drv_led.c \
  platform/drivers/src/dg_cmn_drv_pci.c \
  platform/drivers/src/dg_cmn_drv_phy.c \
  platform/drivers/src/dg_cmn_drv_pttm.c \
  platform/drivers/src/dg_cmn_drv_reset.c \
  platform/drivers/src/dg_cmn_drv_rtc.c \
  platform/drivers/src/dg_cmn_drv_suspend.c \
  platform/drivers/src/dg_cmn_drv_usb.c \
  platform/drivers/src/dg_cmn_drv_version.c

LOCAL_SRC_FILES := \
  $(DG_ENGINE_SRC) \
  $(DG_ENGINE_PAL_SRC) \
  $(DG_HANDLERS_CMN_SRC) \
  $(DG_HANDLERS_PLAT_SRC) \
  $(DG_DRIVERS_CMN_SRC) \
  $(DG_DRIVERS_PLAT_SRC)

include $(BUILD_EXECUTABLE)

