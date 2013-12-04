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
BUILD_STATIC_LIBRARY:= $(BUILD_SYSTEM)/static_library.mk

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
COMMON_GLOBAL_CPPFLAGS:= $(COMMON_GLOBAL_CFLAGS) -Wsign-promo

COMMON_RELEASE_CFLAGS:= -DNDEBUG -UDEBUG
COMMON_RELEASE_CPPFLAGS:= $(COMMON_RELEASE_CFLAGS)

# list of flags to turn specific warnings in to errors
COMMON_ERROR_FLAGS := -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point

# ---------------------------------------------------------------
# Set up configuration for host machine.  We don't do cross-
# compiles except for arm/mips, so the HOST is whatever we are
# running on
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

STATIC_LIB_SUFFIX := .a

ifeq ($(HOST_OS),)
$(error Unable to determine HOST_OS from uname -sm: $(UNAME)!)
endif

# ###############################################################
# Set up final options.
# ###############################################################

GLOBAL_INCLUDES :=
GLOBAL_C_INCLUDES :=

GLOBAL_ARFLAGS := cqs

GLOBAL_CFLAGS += $(COMMON_GLOBAL_CFLAGS)
GLOBAL_CPPFLAGS += $(COMMON_GLOBAL_CPPFLAGS)

GLOBAL_CFLAGS += $(COMMON_ERROR_FLAGS)
GLOBAL_CPPFLAGS += $(COMMON_ERROR_FLAGS)

GLOBAL_CFLAGS += $(COMMON_RELEASE_CFLAGS)
GLOBAL_CPPFLAGS += $(COMMON_RELEASE_CPPFLAGS)

# ---------------------------------------------------------------
# figure out the output directories

ifeq (,$(strip $(OUT_DIR)))
OUT_DIR := $(TOPDIR)out
endif

# config the global output for where to install
OUT_INSTALL := $(OUT_DIR)/install
OUT_EXECUTABLES := $(OUT_INSTALL)/bin
OUT_SHARE_LIBRARIES :=  $(OUT_INSTALL)/lib
OUT_STATIC_LIBRARIES := $(OUT_INSTALL)/lib

#  config the global intermediate output
OUT_INTERMEDIATES := $(OUT_DIR)/obj
OUT_INTERMEDIATE_LIBRARIES := $(OUT_INTERMEDIATES)/lib

