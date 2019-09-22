#LLVM_BIN := ~/workspace/llvm-project/build-rel/bin
LLVM_BIN := ~/workspace/llvm-project/build-debug/bin
LLVM_CONFIG := $(LLVM_BIN)/llvm-config
CXX := $(LLVM_BIN)/clang++
LLVM_SRC_PATH := $(shell $(LLVM_CONFIG) --src-root)/../
LLVM_BUILD_PATH := $(shell $(LLVM_CONFIG) --obj-root)
CLANG_INCLUDES := \
    -I$(LLVM_SRC_PATH)/tools/clang/include \
    -I$(LLVM_SRC_PATH)/clang/include \
    -I$(LLVM_BUILD_PATH)/tools/clang/include

# List of Clang libraries to link. The proper -L will be provided by the
# call to llvm-config
# Note that I'm using -Wl,--{start|end}-group around the Clang libs; this is
# because there are circular dependencies that make the correct order difficult
# to specify and maintain. The linker group options make the linking somewhat
# slower, but IMHO they're still perfectly fine for tools that link with Clang.
CLANG_LIBS := \
    -Wl,--start-group \
    -lclangAST \
    -lclangASTMatchers \
    -lclangAnalysis \
    -lclangBasic \
    -lclangDriver \
    -lclangEdit \
    -lclangFrontend \
    -lclangFrontendTool \
    -lclangLex \
    -lclangParse \
    -lclangSema \
    -lclangEdit \
    -lclangRewrite \
    -lclangRewriteFrontend \
    -lclangStaticAnalyzerFrontend \
    -lclangStaticAnalyzerCheckers \
    -lclangStaticAnalyzerCore \
    -lclangSerialization \
    -lclangToolingCore \
    -lclangTooling \
    -lclangFormat \
    -Wl,--end-group

CXXFLAG := -O2 $(CLANG_INCLUDES) $(shell $(LLVM_CONFIG) --cxxflags)
LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags)
LIBS := $(CLANG_LIBS) $(shell $(LLVM_CONFIG) --libs --system-libs)
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
.PHONY: all debug clean distclean cscope run

all: $(TARGET)

run: all
	@$(BIN_PATH)/$(TARGET_NAME)

debug: $(TARGET_DEBUG)

cscope:
	@cscope -Rbq

clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
