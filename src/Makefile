# Makefile for modular chess engine

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-variable -Wno-unused-but-set-variable

# List all source files
# SRC = $(wildcard *.cpp)
SRC = main.cpp \
      board.cpp \
      bitboard.cpp \
      init.cpp \
      attack.cpp \
      data.cpp \
      io.cpp \
      movegen.cpp \
      validate.cpp \
      makemove.cpp \
      perft.cpp \
      test.cpp \
	  search.cpp \
	  misc.cpp \
	  pvtable.cpp \
	  evaluate.cpp \
      moveorder.cpp \
      uci.cpp \
      xboard.cpp \
      openingBook/polykey.cpp \
	  openingBook/polybook.cpp \
	  incrementalEval.cpp

# Output executable name
TARGET = chess

# Object files (replace .cpp with .o)
OBJ = $(SRC:.cpp=.o)

# Header files (so Make recompiles if these change)
HEADERS = defs.h struct.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
	@echo "🧹 Cleaned up object files and executable"

# Show compiler and source info
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(SRC)"
	@echo "Objects: $(OBJ)"

# Build with O2 optimization (fast)
fast: CXXFLAGS += -O2
fast: clean $(TARGET)
	@echo "⚡ Built with O2 optimization"

# Build with O3 optimization (faster)
faster: CXXFLAGS += -O3
faster: clean $(TARGET)
	@echo "🚀 Built with O3 optimization"

# Show git diff stats (files changed, lines added/deleted) + untracked files
diff:
	@cd .. && echo "=== Tracked file changes ===" && git diff --stat
	@cd .. && echo "" && echo "=== Untracked files ===" && git status --short

# ---------------------------------------------------------------------------------
# Shared JNI library build (adds `shared` target)
# Produces `libchessengine.so` on Unix or `chessengine.dll` on Windows
# it compiles a separate set of object files with PIC and JNI include 
# flags so the engine can be packaged as a native library consumable by Java.
# ---------------------------------------------------------------------------------

SHARED_NAME = native_engine

ifeq ($(OS),Windows_NT)
	SHARED_LIB = $(SHARED_NAME).dll
	JNI_PLATFORM = win32
	SHARED_LINK_FLAGS = -shared
else
	SHARED_LIB = lib$(SHARED_NAME).so
	JNI_PLATFORM = linux
	SHARED_LINK_FLAGS = -shared -fPIC 
#fPIC - Position Independent code , assign address based on where the code is loaded
endif

# Allow overriding JAVA_HOME externally. If not set, the build will try
# to locate `javac` to infer an include path on Unix-like systems.
ifndef JAVA_HOME
JAVAC_PATH := $(shell which javac 2>/dev/null)
ifneq ($(JAVAC_PATH),)
JAVA_HOME := $(dir $(dir $(JAVAC_PATH)))
endif
endif

## Robust JNI include detection
## Prefer explicit JAVA_HOME when it contains jni.h, otherwise search common JDK locations
ifneq ($(strip $(JAVA_HOME)),)
ifneq ($(wildcard $(JAVA_HOME)/include/jni.h),)
JNI_ROOT := $(JAVA_HOME)
else
$(warning JAVA_HOME does not contain jni.h. Will search common JDK locations.)
endif
else
$(warning JAVA_HOME is not set. Will search common JDK locations for jni.h.)
endif

ifneq ($(strip $(JNI_ROOT)),)
# JNI_ROOT already set from JAVA_HOME
else
JNI_SEARCH := /c/Program\ Files/Java/*/include/jni.h /c/Program\ Files\ \(x86\)/Java/*/include/jni.h C:/Program\ Files/Java/*/include/jni.h C:/Program\ Files\ \(x86\)/Java/*/include/jni.h /usr/lib/jvm/*/include/jni.h /c/Users/*/scoop/apps/openjdk/*/current/include/jni.h /c/Users/*/jdk*/include/jni.h C:/Users/*/scoop/apps/openjdk/*/current/include/jni.h C:/Users/*/jdk*/include/jni.h
JNI_HEADER := $(firstword $(wildcard $(JNI_SEARCH)))
ifneq ($(JNI_HEADER),)
JNI_ROOT := $(patsubst %/include/jni.h,%,$(JNI_HEADER))
endif
endif

ifneq ($(strip $(JNI_ROOT)),)
JNI_INCLUDES = -I"$(JNI_ROOT)/include" -I"$(JNI_ROOT)/include/$(JNI_PLATFORM)"
else
$(warning Could not locate jni.h. Set JAVA_HOME to your JDK root (e.g. C:/Program Files/Java/jdk-17.0.8).)
JNI_INCLUDES =
endif

# Sources to include in the shared library: exclude only `main.cpp` (the real driver)
# If JNI headers were not located, exclude `jni_wrapper.cpp` so the shared build still succeeds.
ifneq ($(strip $(JNI_INCLUDES)),)
SHARED_SRC = $(filter-out main.cpp,$(SRC)) jni_wrapper.cpp
else
SHARED_SRC = $(filter-out main.cpp,$(SRC))
endif

# Object names for the shared build (use distinct suffix to avoid collisions)
SHARED_OBJ = $(patsubst %.cpp,%.so.o,$(SHARED_SRC))

# Compiler flags for shared objects
# SHARED_CXXFLAGS = $(CXXFLAGS) -I. $(JNI_INCLUDES) -fPIC
SHARED_CXXFLAGS = $(CXXFLAGS) -O2 -DNDEBUG -march=native -funroll-loops -fomit-frame-pointer -I. $(JNI_INCLUDES) -fPIC

# Pattern rule to compile shared objects (keeps them separate from normal .o files)
%.so.o: %.cpp $(HEADERS)
	$(CXX) $(SHARED_CXXFLAGS) -c $< -o $@

# Link the shared library
$(SHARED_LIB): $(SHARED_OBJ)
	$(CXX) $(SHARED_LINK_FLAGS) -o $@ $^

# Convenience target: build and place the native lib into `native/` directory
shared: $(SHARED_LIB)
	@mkdir -p native
	@cp -f $(SHARED_LIB) native/
	@echo "✅ Built shared library: native/$(SHARED_LIB)"

clean-shared:
	rm -f $(SHARED_OBJ) $(SHARED_LIB)
	@echo "🧹 Cleaned shared-library artifacts"

# Build optimised shared JNI library  [O3 + march=native + NDEBUG]
fast-shared: SHARED_CXXFLAGS += -O3 -DNDEBUG -march=native -funroll-loops -fomit-frame-pointer
fast-shared: clean-shared shared
	@echo "🚀 Built FAST shared library: native/$(SHARED_LIB)  [O3 + native + NDEBUG]"

# Debug helper: show resolved JNI-related variables and whether jni.h exists
check-jni:
	@echo "JAVA_HOME='$(JAVA_HOME)'"
	@echo "JNI_ROOT='$(JNI_ROOT)'"
	@echo "JNI_HEADER='$(JNI_HEADER)'"
	@echo "JNI_INCLUDES='$(JNI_INCLUDES)'"
	@if [ -n "$(JNI_ROOT)" ] && [ -f "$(JNI_ROOT)/include/jni.h" ]; then \
		echo "Found jni.h at $(JNI_ROOT)/include/jni.h"; \
	else \
		echo "jni.h NOT found"; \
	fi