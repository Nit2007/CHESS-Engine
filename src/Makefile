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
	  jni_wrapper.cpp \
	  openingBook/polykey.cpp \
	  openingBook/polybook.cpp

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

ifeq ($(strip $(JAVA_HOME)),)
$(warning JAVA_HOME is not set. Set JAVA_HOME to your JDK root to include JNI headers.)
JNI_INCLUDES =
else
# Quote include paths to handle spaces in Windows JAVA_HOME (e.g. "Program Files")
JNI_INCLUDES = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/$(JNI_PLATFORM)"
endif

# Sources to include in the shared library: exclude only `main.cpp` (the real driver)
SHARED_SRC = $(filter-out main.cpp,$(SRC))

# Object names for the shared build (use distinct suffix to avoid collisions)
SHARED_OBJ = $(patsubst %.cpp,%.so.o,$(SHARED_SRC))

# Compiler flags for shared objects
SHARED_CXXFLAGS = $(CXXFLAGS) -I. $(JNI_INCLUDES) -fPIC

# Pattern rule to compile shared objects (keeps them separate from normal .o files)
%.so.o: %.cpp $(HEADERS)
	$(CXX) $(SHARED_CXXFLAGS) -c $< -o $@

# Link the shared library
$(SHARED_LIB): $(SHARED_OBJ)
	$(CXX) $(SHARED_LINK_FLAGS) -o $@ $^

# Convenience target: build and place the native lib into `native/` directory
shared: $(SHARED_LIB)
	if not exist native (mkdir native)
	copy /Y $(SHARED_LIB) native\
	@echo "✅ Built shared library: native/$(SHARED_LIB)"

clean-shared:
	rm -f $(SHARED_OBJ) $(SHARED_LIB)
	@echo "🧹 Cleaned shared-library artifacts"

