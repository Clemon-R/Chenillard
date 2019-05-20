ifndef COMPONENT
COMPONENT := ver
endif

COMPONENT_LIB := lib$(COMPONENT).a

include Makefile.files

#all: git_hash

INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS) $(INCLUDE_PRIVATE_DIRS))
CPPFLAGS += $(INCLUDE_FLAGS) -g
CXXFLAGS += $(INCLUDE_FLAGS) -std=c++11 -g

# Create target for building this component as a library
CFILES := $(filter %.c, $(SOURCE_FILES))
CPPFILES := $(filter %.cpp, $(SOURCE_FILES))

CTARGET = ${2}/$(patsubst %.c,%.o,$(notdir ${1}))
CPPTARGET = ${2}/$(patsubst %.cpp,%.o,$(notdir ${1}))

ifndef BUILD_DIR
BUILD_DIR := build
endif

OBJ_FILES := $(addprefix $(BUILD_DIR)/, $(filter %.o, $(notdir $(SOURCE_FILES:.cpp=.o) $(SOURCE_FILES:.c=.o))))

define COMPILE_C
$(call CTARGET, ${1}, $(BUILD_DIR)) : ${1}
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(call CTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

define COMPILE_CPP
$(call CPPTARGET, ${1}, $(BUILD_DIR)) : ${1}
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(call CPPTARGET, ${1}, $(BUILD_DIR)) ${1}
endef

$(BUILD_DIR)/$(COMPONENT_LIB): $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

clean:
	rm -f $(OBJ_FILES) $(COMPONENT_LIB)

lib: $(BUILD_DIR)/$(COMPONENT_LIB)

$(foreach cfile, $(CFILES), $(eval $(call COMPILE_C, $(cfile))))
$(foreach cxxfile, $(CPPFILES), $(eval $(call COMPILE_CPP, $(cxxfile))))

.PHONY: all lib clean