# Makefile for the 3D Penguin Game

# Compiler
CXX = g++

# Compiler flags
# Adds optimization and warnings
CXXFLAGS = -O2

# Name of the output executable
TARGET = pinguim3d.out

# Source files
SOURCES = trab2.cpp

# Libraries and Linker Flags
# The libraries needed are OpenGL, GLU, and GLUT.
# The linker flag order can matter.
# For Linux:
ifeq ($(shell uname), Linux)
	LDLIBS = -lglut -lGLU -lGL -lm
endif
# For macOS (Darwin):
ifeq ($(shell uname), Darwin)
	LDLIBS = -framework OpenGL -framework GLUT
endif

# Default target
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDLIBS)
	@echo "Build complete. Executable is '$(TARGET)'"

# Rule to clean up the build directory
clean:
	rm -f $(TARGET)
	@echo "Cleanup complete."

# Phony targets
.PHONY: all clean
