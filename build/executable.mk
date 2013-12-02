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

$(linked_module): PRIVATE_GLOBAL_LD_DIRS := $(my_global_ld_dirs)
$(linked_module): PRIVATE_GLOBAL_LDFLAGS := $(my_global_ldflags)
$(linked_module): PRIVATE_LIBGCC := $(my_libgcc)
$(linked_module): PRIVATE_CRTBEGIN_DYNAMIC_O := $(my_crtbegin_dynamic_o)
$(linked_module): PRIVATE_CRTBEGIN_STATIC_O := $(my_crtbegin_static_o)
$(linked_module): PRIVATE_CRTEND_O := $(my_crtend_o)

$(linked_module): $(my_crtbegin_dynamic_o) $(all_objects) $(all_libraries) $(my_crtend_o)
	$(transform-o-to-executable)

