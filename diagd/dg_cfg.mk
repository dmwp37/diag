#===================================================================================================
#
#   Module Name:  dg_cfg.mk
#
#   General Description: Enables/disables DIAG daemon features
#
#===================================================================================================

### Aux Configuration
DG_CFG_DEFINES += DG_CFG_AUX_NUM=0


### Diag daemon pid file
DG_CFG_DEFINES += DG_CFG_PID_FILE=\"/tmp/diagd.pid\"

### Diag daemon internel socket file
DG_CFG_DEFINES += DG_CFG_INT_SOCKET=\"/tmp/diagd_local_socket\"


DG_CFG_FLAGS := $(addprefix -D, $(DG_CFG_DEFINES))

#$(call print-vars, DG_CFG_FLAGS)


