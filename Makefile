CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g -fsanitize=address
LDFLAGS = -lglfw -lGLEW -lGL -lm -fsanitize=address

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/Room.cpp \
          $(SRC_DIR)/BSPNode.cpp \
          $(SRC_DIR)/MazeGenerator.cpp \
          $(SRC_DIR)/Player.cpp \
          $(SRC_DIR)/Renderer.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/mazmorra

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run valgrind directories