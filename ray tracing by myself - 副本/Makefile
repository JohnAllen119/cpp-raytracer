CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall
TARGET = raytracer
SRC_DIR = src
INCLUDE_DIR = include
OUTPUT_DIR = output

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Logic.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(OUTPUT_DIR)/*.ppm

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
