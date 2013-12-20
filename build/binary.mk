###########################################################
## Standard rules for building binary object files from
## asm/c/cpp source files.
##
## The list of object files is exported in $(all_objects).
###########################################################


#######################################
include $(BUILD_SYSTEM)/base_rules.mk
#######################################


my_compiler_dependencies :=

##################################################
# the linked_module is the linker raw output
##################################################
LOCAL_LINKED_MODULE := $(intermediates)/LINKED/$(LOCAL_BUILT_MODULE_STEM)
ifeq ($(LOCAL_MODULE_CLASS),STATIC_LIBRARIES)
  LOCAL_INTERMEDIATE_TARGETS += $(LOCAL_BUILT_MODULE)
else
  LOCAL_INTERMEDIATE_TARGETS += $(LOCAL_LINKED_MODULE)
endif

###########################################################
## Explicitly declare assembly-only __ASSEMBLY__ macro for
## assembly source
###########################################################
LOCAL_ASFLAGS += -D__ASSEMBLY__

###########################################################
## Define PRIVATE_ variables from global vars
###########################################################
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_PROJECT_INCLUDES := $(GLOBAL_INCLUDES)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_C_INCLUDES := $(GLOBAL_C_INCLUDES)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_GLOBAL_CPPFLAGS := $(GLOBAL_CPPFLAGS)

###########################################################
## Define PRIVATE_ variables used by multiple module types
###########################################################
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_NO_DEFAULT_COMPILER_FLAGS := \
    $(strip $(LOCAL_NO_DEFAULT_COMPILER_FLAGS))

LOCAL_AR := $(AR)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_AR := $(LOCAL_AR)

LOCAL_CC := $(CC)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CC := $(LOCAL_CC)

LOCAL_CXX := $(CXX)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CXX := $(LOCAL_CXX)

# TODO: support a mix of standard extensions so that this isn't necessary
LOCAL_CPP_EXTENSION := $(strip $(LOCAL_CPP_EXTENSION))
ifeq ($(LOCAL_CPP_EXTENSION),)
  LOCAL_CPP_EXTENSION := .cpp
endif
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CPP_EXTENSION := $(LOCAL_CPP_EXTENSION)

ifeq (true,$(LOCAL_GROUP_STATIC_LIBRARIES))
$(LOCAL_BUILT_MODULE): PRIVATE_GROUP_STATIC_LIBRARIES := true
else
$(LOCAL_BUILT_MODULE): PRIVATE_GROUP_STATIC_LIBRARIES :=
endif

###########################################################
## Define per-module debugging flags.  Users can turn on
## debugging for a particular module by setting DEBUG_MODULE_ModuleName
## to a non-empty value in their environment,
## and setting CUSTOM_DEBUG_CFLAGS to the
## debug flags that they want to use.
###########################################################
ifdef DEBUG_MODULE_$(strip $(LOCAL_MODULE))
  debug_cflags := $(CUSTOM_DEBUG_CFLAGS)
else
  debug_cflags :=
endif

###########################################################
## C++: Compile .cpp files to .o.
###########################################################

cpp_normal_sources := $(filter %$(LOCAL_CPP_EXTENSION),$(LOCAL_SRC_FILES))
cpp_normal_objects := $(addprefix $(intermediates)/,$(cpp_normal_sources:$(LOCAL_CPP_EXTENSION)=.o))

cpp_objects        := $(cpp_normal_objects)

ifneq ($(strip $(cpp_objects)),)
$(cpp_objects): $(intermediates)/%.o: \
    $(TOPDIR)$(LOCAL_PATH)/%$(LOCAL_CPP_EXTENSION) \
    $(LOCAL_ADDITIONAL_DEPENDENCIES) \
    | $(my_compiler_dependencies)
	$(transform-cpp-to-o)
-include $(cpp_objects:%.o=%.P)
endif

###########################################################
## C: Compile .c files to .o.
###########################################################

c_normal_sources := $(filter %.c,$(LOCAL_SRC_FILES))
c_normal_objects := $(addprefix $(intermediates)/,$(c_normal_sources:.c=.o))

c_objects        := $(c_normal_objects)

ifneq ($(strip $(c_objects)),)
$(c_objects): $(intermediates)/%.o: $(TOPDIR)$(LOCAL_PATH)/%.c \
    $(LOCAL_ADDITIONAL_DEPENDENCIES) \
    | $(my_compiler_dependencies)
	$(transform-c-to-o)
-include $(c_objects:%.o=%.P)
endif

###########################################################
## AS: Compile .S files to .o.
###########################################################

asm_sources_S := $(filter %.S,$(LOCAL_SRC_FILES))
asm_objects_S := $(addprefix $(intermediates)/,$(asm_sources_S:.S=.o))

ifneq ($(strip $(asm_objects_S)),)
$(asm_objects_S): $(intermediates)/%.o: $(TOPDIR)$(LOCAL_PATH)/%.S \
    $(LOCAL_ADDITIONAL_DEPENDENCIES) \
    | $(my_compiler_dependencies)
	$(transform-s-to-o)
-include $(asm_objects_S:%.o=%.P)
endif

asm_sources_s := $(filter %.s,$(LOCAL_SRC_FILES))
asm_objects_s := $(addprefix $(intermediates)/,$(asm_sources_s:.s=.o))

ifneq ($(strip $(asm_objects_s)),)
$(asm_objects_s): $(intermediates)/%.o: $(TOPDIR)$(LOCAL_PATH)/%.s \
    $(LOCAL_ADDITIONAL_DEPENDENCIES) \
    | $(my_compiler_dependencies)
	$(transform-s-to-o-no-deps)
-include $(asm_objects_s:%.o=%.P)
endif

asm_objects := $(asm_objects_S) $(asm_objects_s)


####################################################
## Import includes
####################################################
import_includes := $(intermediates)/import_includes
import_includes_deps := $(strip \
    $(foreach l, $(installed_shared_library_module_names), \
      $(call intermediates-dir-for,SHARED_LIBRARIES,$(l))/export_includes) \
    $(foreach l, $(LOCAL_STATIC_LIBRARIES) $(LOCAL_WHOLE_STATIC_LIBRARIES), \
      $(call intermediates-dir-for,STATIC_LIBRARIES,$(l))/export_includes))
$(import_includes) : $(import_includes_deps)
	@echo Import includes file: $@
	$(hide) mkdir -p $(dir $@) && rm -f $@
ifdef import_includes_deps
	$(hide) for f in $^; do \
	  cat $$f >> $@; \
	done
else
	$(hide) touch $@
endif


###########################################################
## Common object handling.
###########################################################

# some rules depend on asm_objects being first.  If your code depends on
# being first, it's reasonable to require it to be assembly
all_objects := \
    $(asm_objects) \
    $(cpp_objects) \
    $(c_objects)

LOCAL_C_INCLUDES += $(TOPDIR)$(LOCAL_PATH)

$(all_objects) : | $(import_includes)

###########################################################
# Standard library handling.
###########################################################

###########################################################
# The list of libraries that this module will link against are in
# these variables.  Each is a list of bare module names like "libc libm".
#
# LOCAL_SHARED_LIBRARIES
# LOCAL_STATIC_LIBRARIES
# LOCAL_WHOLE_STATIC_LIBRARIES
#
# We need to convert the bare names into the dependencies that
# we'll use for LOCAL_BUILT_MODULE and LOCAL_INSTALLED_MODULE.
# LOCAL_BUILT_MODULE should depend on the BUILT versions of the
# libraries, so that simply building this module doesn't force
# an install of a library.  Similarly, LOCAL_INSTALLED_MODULE
# should depend on the INSTALLED versions of the libraries so
# that they get installed when this module does.
###########################################################
# NOTE:
# WHOLE_STATIC_LIBRARIES are libraries that are pulled into the
# module without leaving anything out, which is useful for turning
# a collection of .a files into a .so file.  Linking against a
# normal STATIC_LIBRARY will only pull in code/symbols that are
# referenced by the module. (see gcc/ld's --whole-archive option)
###########################################################

# Get the list of BUILT libraries, which are under
# various intermediates directories.
so_suffix := $(SHLIB_SUFFIX)
a_suffix := $(STATIC_LIB_SUFFIX)

LOCAL_SHARED_LIBRARIES += $(LOCAL_SYSTEM_SHARED_LIBRARIES)
built_shared_libraries := \
    $(addprefix $(OUT_INTERMEDIATE_LIBRARIES)/, \
      $(addsuffix $(so_suffix), \
        $(LOCAL_SHARED_LIBRARIES)))

built_static_libraries := \
    $(foreach lib,$(LOCAL_STATIC_LIBRARIES), \
      $(call intermediates-dir-for, \
        STATIC_LIBRARIES,$(lib))/$(lib)$(a_suffix))

built_whole_libraries := \
    $(foreach lib,$(LOCAL_WHOLE_STATIC_LIBRARIES), \
      $(call intermediates-dir-for, \
        STATIC_LIBRARIES,$(lib))/$(lib)$(a_suffix))


# Default is -fno-rtti.
ifeq ($(strip $(LOCAL_RTTI_FLAG)),)
LOCAL_RTTI_FLAG := -fno-rtti
endif

###########################################################
# Rule-specific variable definitions
###########################################################
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_ASFLAGS := $(LOCAL_ASFLAGS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CFLAGS := $(LOCAL_CFLAGS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CPPFLAGS := $(LOCAL_CPPFLAGS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_DEBUG_CFLAGS := $(debug_cflags)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_C_INCLUDES := $(LOCAL_C_INCLUDES)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_IMPORT_INCLUDES := $(import_includes)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_LDFLAGS := $(LOCAL_LDFLAGS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_LDLIBS := $(LOCAL_LDLIBS)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_NO_CRT := $(LOCAL_NO_CRT)

# this is really the way to get the files onto the command line instead
# of using $^, because then LOCAL_ADDITIONAL_DEPENDENCIES doesn't work
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_ALL_SHARED_LIBRARIES := $(built_shared_libraries)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_ALL_STATIC_LIBRARIES := $(built_static_libraries)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_ALL_WHOLE_STATIC_LIBRARIES := $(built_whole_libraries)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_ALL_OBJECTS := $(all_objects)

###########################################################
# Define library dependencies.
###########################################################
# all_libraries is used for the dependencies on LOCAL_BUILT_MODULE.
all_libraries := \
    $(built_shared_libraries) \
    $(built_static_libraries) \
    $(built_whole_libraries)


###########################################################
# Export includes
###########################################################
export_includes := $(intermediates)/export_includes
$(export_includes): PRIVATE_EXPORT_C_INCLUDE_DIRS := $(LOCAL_EXPORT_C_INCLUDE_DIRS)
$(export_includes) : $(LOCAL_MODULE_MAKEFILE)
	@echo Export includes file: $< -- $@
	$(hide) mkdir -p $(dir $@) && rm -f $@
ifdef LOCAL_EXPORT_C_INCLUDE_DIRS
	$(hide) for d in $(PRIVATE_EXPORT_C_INCLUDE_DIRS); do \
	        echo "-I $$d" >> $@; \
	        done
else
	$(hide) touch $@
endif

# Make sure export_includes gets generated when you are running mm/mmm
$(LOCAL_BUILT_MODULE) : | $(export_includes)

