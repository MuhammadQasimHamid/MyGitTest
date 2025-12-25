# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Directories
SRC_DIR = scr
OBJ_DIR = build/obj
BIN_DIR = build/bin

# Subfolders in scr
COMMANDS = $(SRC_DIR)/commands
DATASTRUCTURE = $(SRC_DIR)/dataStructure
CORE = $(SRC_DIR)/core
UTILS = $(SRC_DIR)/utils

# Source files
SRC_COMMANDS = $(wildcard $(COMMANDS)/*.cpp)
SRC_DATASTRUCTURE = $(wildcard $(DATASTRUCTURE)/*.cpp)
SRC_CORE = $(wildcard $(CORE)/*.cpp)
SRC_UTILS = $(wildcard $(UTILS)/*.cpp)
SRC_MAIN = pit.cpp

# Object files
OBJ_COMMANDS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_COMMANDS:.cpp=.o))
OBJ_DATASTRUCTURE = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_DATASTRUCTURE:.cpp=.o))
OBJ_CORE = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_CORE:.cpp=.o))
OBJ_UTILS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_UTILS:.cpp=.o))
OBJ_MAIN = $(OBJ_DIR)/pit.o

# All objects
OBJS = $(OBJ_COMMANDS) $(OBJ_CORE) $(OBJ_UTILS) $(OBJ_MAIN) $(OBJ_DATASTRUCTURE)

# Output executable
TARGET = $(BIN_DIR)/pit

# Default target
all: $(TARGET)

# Link all objects
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lz -lcrypto

# Compile main source (pit.cpp)
$(OBJ_DIR)/pit.o: pit.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile other sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/pit
