# Utility variables.
empty :=
space := $(empty) $(empty)
comma := ,

LOCAL_MODULE_MAKEFILE_NAME := local_module.mk

# ###############################################################
# Build system internal files
# ###############################################################
CLEAR_VARS:= $(BUILD_SYSTEM)/clear_vars.mk
BUILD_EXECUTABLE:= $(BUILD_SYSTEM)/executable.mk


# ###############################################################
# Parse out any modifier targets.
# ###############################################################

# The 'showcommands' goal says to show the full command
# lines being executed, instead of a short message about
# the kind of operation being done.
SHOW_COMMANDS:= $(filter showcommands,$(MAKECMDGOALS))

# ###############################################################
# Set common values
# ###############################################################

# These can be changed to modify both host and device modules.
COMMON_GLOBAL_CFLAGS:= -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith
COMMON_RELEASE_CFLAGS:= -DNDEBUG -UDEBUG

COMMON_GLOBAL_CPPFLAGS:= $(COMMON_GLOBAL_CFLAGS) -Wsign-promo
COMMON_RELEASE_CPPFLAGS:= $(COMMON_RELEASE_CFLAGS)

# list of flags to turn specific warnings in to errors
TARGET_ERROR_FLAGS := -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point

# ---------------------------------------------------------------
# Set up configuration for host machine.  We don't do cross-
# compiles except for arm/mips, so the HOST is whatever we are
# running on

ifneq ($(findstring CYGWIN,$(UNAME)),)
ifeq ($(strip $(USE_CYGWIN)),)

endif
endif


# $(1): The file to check
# TODO: find out what format cygwin's stat(1) uses
define get-file-size
999999999
endef

UNAME := $(shell uname -sm)

# HOST_OS
ifneq (,$(findstring Linux,$(UNAME)))
    HOST_OS := linux
endif

ifneq (,$(findstring CYGWIN,$(UNAME)))
    HOST_OS := windows
    GLOBAL_LDFLAGS += -mconsole
    SHLIB_SUFFIX := .dll
    EXECUTABLE_SUFFIX := .exe
endif

# BUILD_OS is the real host doing the build.
BUILD_OS := $(HOST_OS)

ifeq ($(HOST_OS),)
$(error Unable to determine HOST_OS from uname -sm: $(UNAME)!)
endif

# ---------------------------------------------------------------
# figure out the output directories

ifeq (,$(strip $(OUT_DIR)))
OUT_DIR := $(TOPDIR)out
endif

# config the global output for where to install
OUT_INSTALL := $(OUT_DIR)/install
OUT_EXECUTABLES := $(OUT_INSTALL)/bin
OUT_SHARE_LIB :=  $(OUT_INSTALL)/lib

#  config the global intermediate output
OUT_INTERMEDIATES := $(OUT_DIR)/obj
OUT_INTERMEDIATE_LIBRARIES := $(OUT_INTERMEDIATES)/lib
