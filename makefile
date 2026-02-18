# LeadRose 2D Game Engine Makefile (C++ with Box2D, Tilemap, and Procedural Generation)
# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -fPIC -I/usr/include/SDL2
DEBUG_FLAGS = -g -O0
LDFLAGS = -lm -lSDL2 -lSDL2_image -lbox2d

# Source files and output
SOURCES = main.cpp engine.cpp graphics.cpp physics.cpp tilemap.cpp cave_generator.cpp joystick_manager.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = game

# Default target
all: $(EXECUTABLE)

# Build executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Build complete: $(EXECUTABLE)"

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean $(EXECUTABLE)

# Run the game
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Run with debug info
debug-run: debug
	gdb ./$(EXECUTABLE)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	@echo "✓ Clean complete"

# Install dependencies on Ubuntu/Debian
install-deps:
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev libbox2d-dev

# Show help
help:
	@echo "LeadRose 2D Game Engine - Build Targets"
	@echo "Features: C++, Box2D Physics, Tilemap Rendering, Procedural Cave Generation"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build the game"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make run          - Build and run the game"
	@echo "  make debug-run    - Build with debug info and run in gdb"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make install-deps - Install required dependencies"
	@echo "  make help         - Show this help message"

.PHONY: all debug run debug-run clean install-deps help
