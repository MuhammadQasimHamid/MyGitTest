# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Source files (all .cpp in src/ recursively + main file)
SRC = mygit.cpp $(wildcard src/**/*.cpp)

# Object files
OBJ = $(SRC:.cpp=.o)

# Output executable
TARGET = mygit

# Default rule
all: $(TARGET)

# Link object files
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)
