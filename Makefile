LLVM_BIN := ~/workspace/llvm-project/build-rel/bin
LLVM_CONFIG := $(LLVM_BIN)/llvm-config
CXX := $(LLVM_BIN)/clang++
CXXFLAG := -O2 $(shell $(LLVM_CONFIG) --cxxflags)
LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags)
LIBS := $(shell $(LLVM_CONFIG) --libs --system-libs)
DBGFLAG := -O0 -g3
CXXOBJFLAG := $(CXXFLAG) -c

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug
MKDIR_P := mkdir -p

# compile macros
TARGET_NAME := pee
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
MAIN_SRC := main.cpp

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
$(TARGET): $(OBJ)
	${MKDIR_P} ${BIN_PATH}
	$(CXX) $(CXXFLAG) $? $(LDFLAGS) $(LIBS) -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	${MKDIR_P} ${OBJ_PATH}
	$(CXX) $(CXXOBJFLAG) $< -o $@

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	${MKDIR_P} ${DBG_PATH}
	$(CXX) $(CXXOBJFLAG) $(DBGFLAG) $< -o $@

$(TARGET_DEBUG): $(OBJ_DEBUG)
	${MKDIR_P} ${DBG_PATH}
	$(CXX) $(CXXFLAG) $(DBGFLAG) $? $(LDFLAGS) $(LIBS) -o $@

# phony rules
.PHONY: all debug clean distclean

all: $(TARGET)

debug: $(TARGET_DEBUG)

clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
