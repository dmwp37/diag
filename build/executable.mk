###########################################################
## Standard rules for building an executable file.
##
## Additional inputs from base_rules.make:
## None.
###########################################################
ifeq ($(strip $(LOCAL_MODULE_CLASS)),)
  LOCAL_MODULE_CLASS := EXECUTABLES
endif

ifeq ($(strip $(LOCAL_MODULE_SUFFIX)),)
  LOCAL_MODULE_SUFFIX := $(EXECUTABLE_SUFFIX)
endif

include $(BUILD_SYSTEM)/binary.mk

# Define PRIVATE_ variables from global vars
my_global_ld_dirs := $(GLOBAL_LD_DIRS)
my_global_ldflags := $(GLOBAL_LDFLAGS)
my_libgcc := $(LIBGCC)
my_crtbegin_dynamic_o := $(CRTBEGIN_DYNAMIC_O)
my_crtbegin_static_o := $(CRTBEGIN_STATIC_O)
my_crtend_o := $(CRTEND_O)

$(LOCAL_LINKED_MODULE): PRIVATE_GLOBAL_LD_DIRS := $(my_global_ld_dirs)
$(LOCAL_LINKED_MODULE): PRIVATE_GLOBAL_LDFLAGS := $(my_global_ldflags)
$(LOCAL_LINKED_MODULE): PRIVATE_LIBGCC := $(my_libgcc)
$(LOCAL_LINKED_MODULE): PRIVATE_CRTBEGIN_DYNAMIC_O := $(my_crtbegin_dynamic_o)
$(LOCAL_LINKED_MODULE): PRIVATE_CRTBEGIN_STATIC_O := $(my_crtbegin_static_o)
$(LOCAL_LINKED_MODULE): PRIVATE_CRTEND_O := $(my_crtend_o)

$(LOCAL_LINKED_MODULE): $(my_crtbegin_dynamic_o) $(all_objects) $(all_libraries) $(my_crtend_o)
	$(transform-o-to-executable)

###########################################################
## Strip
###########################################################
$(LOCAL_BUILT_MODULE): $(LOCAL_LINKED_MODULE)
	$(transform-to-stripped)

