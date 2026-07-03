CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g -fsanitize=address
LDFLAGS = -lglfw -lGLEW -lGL -lm -fsanitize=address

TARGET = mazmorra
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run valgrind