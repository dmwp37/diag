SHELL := /bin/bash

# this turns off the suffix rules built into make
.SUFFIXES:

# this turns off the RCS / SCCS implicit rules of GNU Make
% : RCS/%,v
% : RCS/%
% : %,v
% : s.%
% : SCCS/s.%

# If a rule fails, delete $@.
.DELETE_ON_ERROR:

# Absolute path of the present working direcotry.
# This overrides the shell variable $PWD, which does not necessarily points to
# the top of the source tree, for example when "make -C" is used in m/mm/mmm.
PWD := $(shell pwd)

TOP := .
TOPDIR :=

BUILD_SYSTEM := $(TOPDIR)build

# This is the default target.  It must be the first declared target.
.PHONY: world version
DEFAULT_GOAL := world
$(DEFAULT_GOAL): version

version:
	@$(BUILD_SYSTEM)/version.sh

# Used to force goals to build.  Only use for conditionally defined goals.
.PHONY: FORCE
FORCE:

# Set up various standard variables based on configuration
# and host information.
include $(BUILD_SYSTEM)/config.mk

# These are the modifier targets that don't do anything themselves, but
# change the behavior of the build.
# (must be defined before including definitions.make)
INTERNAL_MODIFIER_TARGETS := showcommands all 

# Bring in standard build system definitions.
include $(BUILD_SYSTEM)/definitions.mk

# If they only used the modifier goals (showcommands, etc), we'll actually
# build the default target.
ifeq ($(filter-out $(INTERNAL_MODIFIER_TARGETS),$(MAKECMDGOALS)),)
.PHONY: $(INTERNAL_MODIFIER_TARGETS)
$(INTERNAL_MODIFIER_TARGETS): $(DEFAULT_GOAL)
endif

#
# Typical build; include any local module makefiles we can find.
#
subdirs := $(TOP)
skip_serch_dir := .git .repo out

find_cmd := find $(subdirs) -type d \( $(foreach d, $(skip_serch_dir),-name $(d) -o ) -name build \) -prune -o -type f -name $(LOCAL_MODULE_MAKEFILE_NAME) -printf "%p "

subdir_makefiles := $(shell $(find_cmd)) 
include $(subdir_makefiles)


# -------------------------------------------------------------------
# All module makefiles have been included at this point.
# -------------------------------------------------------------------
# -------------------------------------------------------------------
# Define dependencies for modules that require other modules.
# This can only happen now, after we've read in all module makefiles.
#
define add-required-deps
$(1): | $(2)
endef
$(foreach m,$(ALL_MODULES), \
  $(eval r := $(ALL_MODULES.$(m).REQUIRED)) \
  $(if $(r), \
    $(eval r := $(call module-installed-files,$(r))) \
    $(eval $(call add-required-deps, $(m),$(r))) \
   ) \
 )

m :=
r :=
add-required-deps :=

# -------------------------------------------------------------------
# Figure out our module sets.
#
# by default we need to compile all registered modules
modules_to_install := $(call module-installed-files, $(ALL_MODULES))

$(DEFAULT_GOAL): $(modules_to_install)


.PHONY: clean
clean:
	@rm -rf $(OUT_DIR)
	@echo "Entire build directory removed."

.PHONY: clobber
clobber: clean

# The rules for dataclean and installclean are defined in cleanbuild.mk.

#xxx scrape this from ALL_MODULE_NAME_TAGS
.PHONY: modules
modules:
	@echo "Available sub-modules:"
	@echo "$(ALL_MODULES) " | tr -s ' ' '\n' | sort -u | cat

.PHONY: showcommands
showcommands:
	@echo >/dev/null

.PHONY: nothing
nothing:
	@echo Successfully read the makefiles.
