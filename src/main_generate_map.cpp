
#include "map_generator.h"


int main() {
    MapGenerator::from_str_file("maps/example.csv", "maps/example.map");

    return 0;
}
