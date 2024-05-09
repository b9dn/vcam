CC := g++

CFLAGS := -Wall -std=c++17

LDFLAGS := -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm

INCLUDES := -I./include

SOURCES := bsp.cpp cube.cpp util.cpp vcam.cpp

OBJECTS := $(SOURCES:.cpp=.o)

EXECUTABLE := vcam

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean

