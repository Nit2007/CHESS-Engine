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
