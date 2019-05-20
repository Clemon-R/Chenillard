export BUILD_DIR=$(COMPONENT_BUILD_DIR)
include $(COMPONENT_PATH)/Makefile.files
COMPONENT_OWNBUILDTARGET := 1
COMPONENT_OWNCLEANTARGET := 1

COMPONENT_ADD_INCLUDEDIRS := $(INCLUDE_DIRS)
COMPONENT_PRIV_INCLUDEDIRS := $(INCLUDE_PRIVATE_DIRS)

GENERATED_HASH := $(COMPONENT_BUILD_DIR)/version_hash.h

GIT_HASH:=$(shell git log -1 --pretty=format:"%H")

ifeq ($(GIT_HASH),)
	GIT_HASH:=not a git repository
else
	GIT_DIRTY:=$(shell git diff --stat)
	ifneq ($(GIT_DIRTY),)
		GIT_HASH:=$(GIT_HASH)-dirty
	endif
endif

.PHONY: build
build:
	$(summary) RM -f $(GENERATED_HASH)
	rm -f $(GENERATED_HASH)
	@echo 'GIT_HASH =  "$(GIT_HASH)"'
	@echo 'PROJECT_VER =  "$(PROJECT_VER)"'
	@echo '/*------------------------------------------------------------------------------' > $(GENERATED_HASH)
	@echo ' * auto-generated file; do not modify' >> $(GENERATED_HASH)
	@echo ' *----------------------------------------------------------------------------*/' >> $(GENERATED_HASH)
	@echo '#ifndef __GIT_HASH_FILE__' >> $(GENERATED_HASH)
	@echo '#define __GIT_HASH_FILE__' >> $(GENERATED_HASH)
	@echo '' >> $(GENERATED_HASH)
	@echo '#define VERSION_VERSION     "$(PROJECT_VER)"' >> $(GENERATED_HASH)
	@echo '' >> $(GENERATED_HASH)
	@echo '#define VERSION_HASH        "$(GIT_HASH)"' >> $(GENERATED_HASH)
	@echo '' >> $(GENERATED_HASH)
	@echo '#endif /* __GIT_HASH_FILE__ */' >> $(GENERATED_HASH)
	$(summary) MAKE -C $(COMPONENT_PATH) lib BUILD_DIR=$(COMPONENT_BUILD_DIR) COMPONENT=$(COMPONENT_NAME)
	$(MAKE) -C $(COMPONENT_PATH) lib BUILD_DIR=$(COMPONENT_BUILD_DIR) COMPONENT=$(COMPONENT_NAME)

.PHONY: clean
clean:
	$(summary) RM -f $(GENERATED_HASH)
	rm -f $(GENERATED_HASH)
	$(summary) MAKE -C $(COMPONENT_PATH) clean
	$(MAKE) -C $(COMPONENT_PATH) clean
