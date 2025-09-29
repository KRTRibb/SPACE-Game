# Compiler
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Raylib
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib)
RAYLIB_LDFLAGS := $(shell pkg-config --libs raylib)

# Project files
SRC = $(shell find src -name '*.cpp')
OBJ = $(SRC:.cpp=.o)
BIN = main

# Include directory
INCLUDES = -Iinclude

# Build rule
all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(BIN) $(RAYLIB_LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RAYLIB_CFLAGS) -c $< -o $@

# Run
run: $(BIN)
	./$(BIN)

# Clean
clean:
	rm -f $(OBJ) $(BIN)
count:
	find . -name '*.cpp' -o -name '*.hpp' | xargs wc -l

