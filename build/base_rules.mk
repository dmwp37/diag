###########################################################
## Common instructions for a generic module.
###########################################################

LOCAL_MODULE := $(strip $(LOCAL_MODULE))
ifeq ($(LOCAL_MODULE),)
  $(error $(LOCAL_PATH): LOCAL_MODULE is not defined)
endif

LOCAL_MODULE_CLASS := $(strip $(LOCAL_MODULE_CLASS))
ifneq ($(words $(LOCAL_MODULE_CLASS)),1)
  $(error $(LOCAL_PATH): LOCAL_MODULE_CLASS must contain exactly one word, not "$(LOCAL_MODULE_CLASS)")
endif

LOCAL_MODULE_PATH := $(strip $(LOCAL_MODULE_PATH))
ifeq ($(LOCAL_MODULE_PATH),)
  LOCAL_MODULE_PATH := $(OUT_$(LOCAL_MODULE_CLASS))
  ifeq ($(strip $(LOCAL_MODULE_PATH)),)
    $(error $(LOCAL_PATH): unhandled LOCAL_MODULE_CLASS "$(LOCAL_MODULE_CLASS)")
  endif
endif


intermediates := $(call local-intermediates-dir)

###########################################################
# Pick a name for the intermediate and final targets
###########################################################
LOCAL_MODULE_STEM := $(strip $(LOCAL_MODULE_STEM))
ifeq ($(LOCAL_MODULE_STEM),)
  LOCAL_MODULE_STEM := $(LOCAL_MODULE)
endif
LOCAL_INSTALLED_MODULE_STEM := $(LOCAL_MODULE_STEM)$(LOCAL_MODULE_SUFFIX)

LOCAL_BUILT_MODULE_STEM := $(strip $(LOCAL_BUILT_MODULE_STEM))
ifeq ($(LOCAL_BUILT_MODULE_STEM),)
  LOCAL_BUILT_MODULE_STEM := $(LOCAL_INSTALLED_MODULE_STEM)
endif

LOCAL_BUILT_MODULE := $(intermediates)/$(LOCAL_BUILT_MODULE_STEM)

###########################################################
## make clean- targets
###########################################################
cleantarget := clean-$(LOCAL_MODULE)
$(cleantarget) : PRIVATE_MODULE := $(LOCAL_MODULE)
$(cleantarget) : PRIVATE_CLEAN_FILES := \
    $(LOCAL_BUILT_MODULE) \
    $(LOCAL_INSTALLED_MODULE) \
    $(intermediates)
$(cleantarget)::
	@echo "Clean: $(PRIVATE_MODULE)"
	$(hide) rm -rf $(PRIVATE_CLEAN_FILES)

###########################################################
## Common definitions for module.
###########################################################

# Propagate local configuration options to this target.
$(LOCAL_INTERMEDIATE_TARGETS) : PRIVATE_PATH:=$(LOCAL_PATH)
$(LOCAL_INTERMEDIATE_TARGETS) : PRIVATE_INTERMEDIATES_DIR:= $(intermediates)

# Tell the module and all of its sub-modules who it is.
$(LOCAL_INTERMEDIATE_TARGETS) : PRIVATE_MODULE:= $(LOCAL_MODULE)

###########################################################
## Module installation rule
###########################################################

ifndef LOCAL_UNINSTALLABLE_MODULE
LOCAL_INSTALLED_MODULE := $(LOCAL_MODULE_PATH)/$(LOCAL_INSTALLED_MODULE_STEM)

$(LOCAL_INSTALLED_MODULE): $(LOCAL_BUILT_MODULE)
	@echo "Install: $@"
	$(copy-file-to-target-with-cp)

endif # !LOCAL_UNINSTALLABLE_MODUL


# Provide a short-hand for building this module.
# We name both BUILT and INSTALLED in case
# LOCAL_UNINSTALLABLE_MODULE is set.
.PHONY: $(LOCAL_MODULE)
$(LOCAL_MODULE): $(LOCAL_BUILT_MODULE) $(LOCAL_INSTALLED_MODULE)

###########################################################
## Register with ALL_MODULES
###########################################################

ALL_MODULES += $(LOCAL_MODULE)

# Don't use += on subvars, or else they'll end up being
# recursively expanded.
ALL_MODULES.$(LOCAL_MODULE).CLASS := \
    $(ALL_MODULES.$(LOCAL_MODULE).CLASS) $(LOCAL_MODULE_CLASS)
ALL_MODULES.$(LOCAL_MODULE).PATH := \
    $(ALL_MODULES.$(LOCAL_MODULE).PATH) $(LOCAL_PATH)
ALL_MODULES.$(LOCAL_MODULE).BUILT := \
    $(ALL_MODULES.$(LOCAL_MODULE).BUILT) $(LOCAL_BUILT_MODULE)
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
    $(strip $(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(LOCAL_INSTALLED_MODULE))
ALL_MODULES.$(LOCAL_MODULE).REQUIRED := \
    $(ALL_MODULES.$(LOCAL_MODULE).REQUIRED) $(LOCAL_REQUIRED_MODULES)

