ALL_MODULES:=

###########################################################
## Debugging; prints a variable list to stdout
###########################################################

# $(1): variable name list, not variable values
define print-vars
$(foreach var,$(1), \
  $(info $(var):) \
  $(foreach word,$($(var)), \
    $(info $(space)$(space)$(word)) \
   ) \
 )
endef

###########################################################
## Retrieve the directory of the current makefile
###########################################################

# Figure out where we are.
define my-dir
$(strip \
  $(eval LOCAL_MODULE_MAKEFILE := $$(lastword $$(MAKEFILE_LIST))) \
  $(if $(filter $(CLEAR_VARS),$(LOCAL_MODULE_MAKEFILE)), \
    $(error LOCAL_PATH must be set before including $$(CLEAR_VARS)) \
   , \
    $(patsubst %/,%,$(dir $(LOCAL_MODULE_MAKEFILE))) \
   ) \
 )
endef

###########################################################
## Convert a list of short modules names
## into the list of files that are installed for those modules.
## NOTE: this won't return reliable results until after all
## sub-makefiles have been included.
## $(1): target list
###########################################################

define module-installed-files
$(foreach module,$(1),$(ALL_MODULES.$(module).INSTALLED))
endef

###########################################################
## Copy a single file from one place to another
###########################################################

define copy-file-to-target-with-cp
@mkdir -p $(dir $@)
$(hide) cp -fp $< $@
endef

###########################################################
## The intermediates directory.  Where object files go
###########################################################

# $(1): target class, like "APPS"
# $(2): target name, like "NotePad"
define intermediates-dir-for
$(strip \
    $(eval _idfClass := $(strip $(1))) \
    $(if $(_idfClass),, \
        $(error $(LOCAL_PATH): Class not defined in call to intermediates-dir-for)) \
    $(eval _idfName := $(strip $(2))) \
    $(if $(_idfName),, \
        $(error $(LOCAL_PATH): Name not defined in call to intermediates-dir-for)) \
    $(eval _idfIntBase := $(OUT_INTERMEDIATES)) \
    $(_idfIntBase)/$(_idfClass)/$(_idfName)_intermediates \
)
endef

# Uses LOCAL_MODULE_CLASS and LOCAL_MODULE
# to determine the intermediates directory.
#
define local-intermediates-dir
$(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE))
endef


###########################################################
## Output the command lines, or not
###########################################################

ifeq ($(strip $(SHOW_COMMANDS)),)
define pretty
@echo $1
endef
hide := @
else
define pretty
endef
hide :=
endif


###########################################################
## Commands for munging the dependency files GCC generates
###########################################################
# $(1): the input .d file
# $(2): the output .P file
define transform-d-to-p-args
$(hide) cp $(1) $(2); \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(1) >> $(2); \
	rm -f $(1)
endef

define transform-d-to-p
$(call transform-d-to-p-args,$(@:%.o=%.d),$(@:%.o=%.P))
endef


###########################################################
## Commands for running gcc to compile a C++ file
###########################################################

define transform-cpp-to-o
@mkdir -p $(dir $@)
@echo "compile C++: $(PRIVATE_MODULE) <= $<"
$(hide) $(PRIVATE_CXX) \
	$(addprefix -I , $(PRIVATE_C_INCLUDES)) \
	$(shell cat $(PRIVATE_IMPORT_INCLUDES)) \
	$(addprefix -isystem ,\
	    $(if $(PRIVATE_NO_DEFAULT_COMPILER_FLAGS),, \
	        $(filter-out $(PRIVATE_C_INCLUDES), \
	            $(PRIVATE_PROJECT_INCLUDES) \
	            $(PRIVATE_C_INCLUDES)))) \
	-c \
	$(if $(PRIVATE_NO_DEFAULT_COMPILER_FLAGS),, \
	    $(PRIVATE_GLOBAL_CFLAGS) \
	    $(PRIVATE_GLOBAL_CPPFLAGS) \
	 ) \
	$(PRIVATE_RTTI_FLAG) \
	$(PRIVATE_CFLAGS) \
	$(PRIVATE_CPPFLAGS) \
	$(PRIVATE_DEBUG_CFLAGS) \
	-MD -MF $(patsubst %.o,%.d,$@) -o $@ $<
$(transform-d-to-p)
endef


###########################################################
## Commands for running gcc to compile a C file
###########################################################

# $(1): extra flags
define transform-c-or-s-to-o-no-deps
@mkdir -p $(dir $@)
$(hide) $(PRIVATE_CC) \
	$(addprefix -I , $(PRIVATE_C_INCLUDES)) \
	$(shell cat $(PRIVATE_IMPORT_INCLUDES)) \
	$(addprefix -isystem ,\
	    $(if $(PRIVATE_NO_DEFAULT_COMPILER_FLAGS),, \
	        $(filter-out $(PRIVATE_C_INCLUDES), \
	            $(PRIVATE_PROJECT_INCLUDES) \
	            $(PRIVATE_C_INCLUDES)))) \
	-c \
	$(if $(PRIVATE_NO_DEFAULT_COMPILER_FLAGS),, \
	    $(PRIVATE_GLOBAL_CFLAGS) \
	 ) \
	 $(1) \
	-MD -MF $(patsubst %.o,%.d,$@) -o $@ $<
endef

define transform-c-to-o-no-deps
@echo "compile C: $(PRIVATE_MODULE) <= $<"
$(call transform-c-or-s-to-o-no-deps, $(PRIVATE_CFLAGS) $(PRIVATE_CONLYFLAGS) $(PRIVATE_DEBUG_CFLAGS))
endef

define transform-s-to-o-no-deps
@echo "compile asm: $(PRIVATE_MODULE) <= $<"
$(call transform-c-or-s-to-o-no-deps, $(PRIVATE_ASFLAGS))
endef

define transform-c-to-o
$(transform-c-to-o-no-deps)
$(transform-d-to-p)
endef

define transform-s-to-o
$(transform-s-to-o-no-deps)
$(transform-d-to-p)
endef


###########################################################
## Commands for running ar
###########################################################

define _concat-if-arg2-not-empty
$(if $(2),$(hide) $(1) $(2))
endef

# Split long argument list into smaller groups and call the command repeatedly
# Call the command at least once even if there are no arguments, as otherwise
# the output file won't be created.
#
# $(1): the command without arguments
# $(2): the arguments
define split-long-arguments
$(hide) $(1) $(wordlist 1,500,$(2))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 501,1000,$(2)))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 1001,1500,$(2)))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 1501,2000,$(2)))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 2001,2500,$(2)))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 2501,3000,$(2)))
$(call _concat-if-arg2-not-empty,$(1),$(wordlist 3001,99999,$(2)))
endef

# $(1): the full path of the source static library.
define _extract-and-include-single-target-whole-static-lib
@echo "preparing StaticLib: $(PRIVATE_MODULE) [including $(1)]"
$(hide) ldir=$(PRIVATE_INTERMEDIATES_DIR)/WHOLE/$(basename $(notdir $(1)))_objs;\
    rm -rf $$ldir; \
    mkdir -p $$ldir; \
    filelist=; \
    for f in `$(PRIVATE_AR) t $(1)`; do \
        $(PRIVATE_AR) p $(1) $$f > $$ldir/$$f; \
        filelist="$$filelist $$ldir/$$f"; \
    done ; \
    $(PRIVATE_AR) $(GLOBAL_ARFLAGS) $(PRIVATE_ARFLAGS) $@ $$filelist

endef

define extract-and-include-target-whole-static-libs
$(foreach lib,$(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES), \
    $(call _extract-and-include-single-target-whole-static-lib, $(lib)))
endef

# Explicitly delete the archive first so that ar doesn't
# try to add to an existing archive.
define transform-o-to-static-lib
@mkdir -p $(dir $@)
@rm -f $@
$(extract-and-include-target-whole-static-libs)
@echo "target StaticLib: $(PRIVATE_MODULE) ($@)"
$(call split-long-arguments,$(PRIVATE_AR) $(GLOBAL_ARFLAGS) $(PRIVATE_ARFLAGS) $@,$(filter %.o, $^))
endef


###########################################################
## Commands for filtering a target executable or library
###########################################################

define transform-to-stripped
@mkdir -p $(dir $@)
@echo "Strip: $(PRIVATE_MODULE) ($@)"
$(hide) strip --strip-debug $< -o $@
endef


###########################################################
## Commands for running gcc to link an executable
###########################################################

define transform-o-to-executable-inner
$(hide) $(PRIVATE_CXX) \
	$(PRIVATE_GLOBAL_LDFLAGS) \
	$(PRIVATE_GLOBAL_LD_DIRS) \
	-Wl,-rpath-link=$(OUT_INTERMEDIATE_LIBRARIES) \
	-Wl,-rpath,\$$ORIGIN/../lib \
	$(PRIVATE_LDFLAGS) \
	$(PRIVATE_ALL_OBJECTS) \
	-Wl,--whole-archive \
	$(call normalize-target-libraries,$(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES)) \
	-Wl,--no-whole-archive \
	$(if $(PRIVATE_GROUP_STATIC_LIBRARIES),-Wl$(comma)--start-group) \
	$(call normalize-target-libraries,$(PRIVATE_ALL_STATIC_LIBRARIES)) \
	$(if $(PRIVATE_GROUP_STATIC_LIBRARIES),-Wl$(comma)--end-group) \
	$(call normalize-target-libraries,$(PRIVATE_ALL_SHARED_LIBRARIES)) \
	-o $@ \
	$(PRIVATE_LDLIBS)
endef

define transform-o-to-executable
@mkdir -p $(dir $@)
@echo "Executable: $(PRIVATE_MODULE) ($@)"
$(transform-o-to-executable-inner)
endef

