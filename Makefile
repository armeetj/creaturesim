# Compiler and flags
CC = clang++
CFLAGS = -Wall -std=c++11 -Iinclude -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lraylib

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Target executable
TARGET = game

# Source and header files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(INC_DIR)/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Build rules
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Utility rules
.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ_DIR)
