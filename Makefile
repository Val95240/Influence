
.PHONY: all clean

CC := g++
CFLAGS := -std=c++17 -Wall -Wextra -Werror -Wno-unused-parameter
CFLAGS += -I/usr/include/SDL2 -I/usr/local/include/SDL2 -I./src
LFLAGS := -lSDL2 -lSDL2_gfx -lSDL2_ttf

HEADERS := $(shell find src/ -name "*.h")
SOURCES := $(filter-out src/main_generate_map.cpp, $(shell find src/ -name "*.cpp"))
OBJECTS := $(SOURCES:src/%.cpp=build/%.o)

SOURCES_MAP := $(filter-out src/main.cpp, $(shell find src/ -name "*.cpp"))
OBJECTS_MAP := $(SOURCES_MAP:src/%.cpp=build/%.o)


all: main generate_map

main: $(OBJECTS)
	@ mkdir -p build/
	@ mkdir -p build/agents/
	@ $(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

generate_map: $(OBJECTS_MAP)
	@ mkdir -p build/
	@ mkdir -p build/agents/
	@ $(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

build/%.o: src/%.cpp $(HEADERS)
	@ mkdir -p build/
	@ mkdir -p build/agents/
	@ $(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf build/
