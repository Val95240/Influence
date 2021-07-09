
.PHONY: default clean

CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Werror -Wno-unused-parameter
CFLAGS += -I/usr/include/SDL2 -I/usr/local/include/SDL2
LFLAGS = -lSDL2 -lSDL2_gfx -lSDL2_ttf

HEADERS := $(wildcard src/*.h)
SOURCES := $(filter-out src/main_generate_map.cpp, $(wildcard src/*.cpp))
OBJECTS := $(SOURCES:src/%.cpp=build/%.o)

SOURCES_MAP := $(filter-out src/main.cpp, $(wildcard src/*.cpp))
OBJECTS_MAP := $(SOURCES_MAP:src/%.cpp=build/%.o)


main: $(OBJECTS)
	@ mkdir -p build/
	@ $(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

generate_map: $(OBJECTS_MAP)
	@ mkdir -p build/
	@ $(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

build/%.o: src/%.cpp $(HEADERS)
	@ mkdir -p build/
	@ $(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf build/
