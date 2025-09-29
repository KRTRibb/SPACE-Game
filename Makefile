# Compiler
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Raylib
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib)
RAYLIB_LDFLAGS := $(shell pkg-config --libs raylib)

# Project files
SRC = $(shell find src -name '*.cpp')

# Object output directory
OBJDIR = build
OBJ = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))

BIN = main

# Include directories
INCLUDES = -Iinclude -Iinclude/core -Iinclude/controllers -Iinclude/ui

# Build rule
all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(BIN) $(RAYLIB_LDFLAGS)

# Pattern rule: compile into $(OBJDIR)
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RAYLIB_CFLAGS) -c $< -o $@

# Run
run: $(BIN)
	./$(BIN)

# Clean
clean:
	rm -rf $(OBJDIR) $(BIN)

count:
	find . -name '*.cpp' -o -name '*.hpp' | xargs wc -l
