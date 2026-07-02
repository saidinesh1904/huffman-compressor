CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude
SRC_DIR  := src
OBJ_DIR  := build
TARGET   := huffman

SOURCES  := $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJECTS  := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))

.PHONY: all clean dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR) output

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) output/*.huff output/*.txt
