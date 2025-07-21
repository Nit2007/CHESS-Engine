# Makefile for modular chess engine

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# List all source files
SRC = main.cpp board.cpp bitboard.cpp 

# Output executable name
TARGET = chess

# Object files (replace .cpp with .o)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
