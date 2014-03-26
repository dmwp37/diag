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
DG_FLAGS += -D_GNU_SOURCE

#===================================================
# DIAG DAEMON
#===================================================
include $(CLEAR_VARS)

LOCAL_MODULE := diagd

LOCAL_CFLAGS := $(DG_FLAGS)

DG_DIAG_INC := \
  ../out \
  common/engine/hdr \
  common/engine/pal/hdr \
  common/handlers/hdr \
  common/drivers/hdr \
  platform/handlers/hdr \
  platform/drivers/hdr \
  platform/engine/hdr

LOCAL_C_INCLUDES :=  $(addprefix $(LOCAL_PATH)/, $(DG_DIAG_INC))

LOCAL_STATIC_LIBRARIES := libdiagapi
LOCAL_LDLIBS +=

#Common Engine
DG_ENGINE_SRC := \
  common/engine/src/dg_aux_engine.c \
  common/engine/src/dg_aux_util.c \
  common/engine/src/dg_client_comm.c \
  common/engine/src/dg_dbg.c \
  common/engine/src/dg_engine_util.c \
  common/engine/src/dg_main.c \
  common/engine/src/dg_socket.c

#Engine for specific platform
#Make sure you checked out the correct branch for the platform
DG_ENGINE_PLAT_SRC := \
  platform/engine/src/dg_main_task.c \
  platform/engine/pal/src/dg_pal_aux_engine.c \
  platform/engine/pal/src/dg_pal_dbg.c \
  platform/engine/pal/src/dg_pal_util.c \
  platform/engine/pal/src/dg_pal_security.c

#Common Handlers
DG_HANDLERS_CMN_SRC := \
  common/handlers/src/dg_bios.c \
  common/handlers/src/dg_button.c \
  common/handlers/src/dg_clock.c \
  common/handlers/src/dg_cpld.c \
  common/handlers/src/dg_debug_level.c \
  common/handlers/src/dg_dimm.c \
  common/handlers/src/dg_eeprom.c \
  common/handlers/src/dg_ext_loop.c \
  common/handlers/src/dg_fan.c \
  common/handlers/src/dg_fpga.c \
  common/handlers/src/dg_fpga_flash.c \
  common/handlers/src/dg_gpio.c \
  common/handlers/src/dg_i2c.c \
  common/handlers/src/dg_int_loop.c \
  common/handlers/src/dg_led.c \
  common/handlers/src/dg_loop.c \
  common/handlers/src/dg_pci.c \
  common/handlers/src/dg_phy.c \
  common/handlers/src/dg_ping.c \
  common/handlers/src/dg_port_led.c \
  common/handlers/src/dg_prbs.c \
  common/handlers/src/dg_psu.c \
  common/handlers/src/dg_pttm.c \
  common/handlers/src/dg_reset.c \
  common/handlers/src/dg_rtc.c \
  common/handlers/src/dg_sfp.c \
  common/handlers/src/dg_ssd.c \
  common/handlers/src/dg_suspend.c \
  common/handlers/src/dg_temp.c \
  common/handlers/src/dg_tpm.c \
  common/handlers/src/dg_usb.c \
  common/handlers/src/dg_version.c \
  common/handlers/src/dg_voltage.c \
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
  platform/drivers/src/dg_cmn_drv_clock.c \
  platform/drivers/src/dg_cmn_drv_cpld.c \
  platform/drivers/src/dg_cmn_drv_debug_level.c \
  platform/drivers/src/dg_cmn_drv_dimm.c \
  platform/drivers/src/dg_cmn_drv_eeprom.c \
  platform/drivers/src/dg_cmn_drv_ext_loop.c \
  platform/drivers/src/dg_cmn_drv_fan.c \
  platform/drivers/src/dg_cmn_drv_fpga.c \
  platform/drivers/src/dg_cmn_drv_fpga_flash.c \
  platform/drivers/src/dg_cmn_drv_gpio.c \
  platform/drivers/src/dg_cmn_drv_i2c.c \
  platform/drivers/src/dg_cmn_drv_int_loop.c \
  platform/drivers/src/dg_cmn_drv_led.c \
  platform/drivers/src/dg_cmn_drv_loop.c \
  platform/drivers/src/dg_cmn_drv_pci.c \
  platform/drivers/src/dg_cmn_drv_phy.c \
  platform/drivers/src/dg_cmn_drv_port_led.c \
  platform/drivers/src/dg_cmn_drv_prbs.c \
  platform/drivers/src/dg_cmn_drv_psu.c \
  platform/drivers/src/dg_cmn_drv_pttm.c \
  platform/drivers/src/dg_cmn_drv_reset.c \
  platform/drivers/src/dg_cmn_drv_rtc.c \
  platform/drivers/src/dg_cmn_drv_sfp.c \
  platform/drivers/src/dg_cmn_drv_ssd.c \
  platform/drivers/src/dg_cmn_drv_suspend.c \
  platform/drivers/src/dg_cmn_drv_temp.c \
  platform/drivers/src/dg_cmn_drv_tpm.c \
  platform/drivers/src/dg_cmn_drv_usb.c \
  platform/drivers/src/dg_cmn_drv_version.c \
  platform/drivers/src/dg_cmn_drv_voltage.c

LOCAL_SRC_FILES := \
  $(DG_ENGINE_SRC) \
  $(DG_ENGINE_PLAT_SRC) \
  $(DG_HANDLERS_CMN_SRC) \
  $(DG_HANDLERS_PLAT_SRC) \
  $(DG_DRIVERS_CMN_SRC) \
  $(DG_DRIVERS_PLAT_SRC)

include $(BUILD_EXECUTABLE)

