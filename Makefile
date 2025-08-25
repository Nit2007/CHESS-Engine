# Makefile for modular chess engine

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# List all source files
SRC = main.cpp board.cpp bitboard.cpp init.cpp attack.cpp data.cpp io.cpp movegen.cpp validate.cpp makemove.cpp

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
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
