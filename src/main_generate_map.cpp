
#include <iostream>
#include <filesystem>

#include "map_generator.h"


int main(int argc, char* argv[]) {
    std::string map_file;
    if (argc < 2)
        map_file = "maps/example.csv";
    else
        map_file = argv[1];

    if (!std::filesystem::exists(map_file)) {
        std::cout << "Map file not found: " << map_file << std::endl;
        return 1;
    }

    size_t index = map_file.find('.');
    if (index == std::string::npos || map_file.substr(index+1) != "csv") {
        std::cout << "Invalid map file: the map file extension needs to be `.map`" << std::endl;
        return 1;
    }

    std::string dst_file = map_file;
    dst_file.replace(index+1, 3, "map");
    std::cout << "Generating map " << dst_file << "..." << std::endl;
    if (!MapGenerator::from_str_file(map_file, dst_file))
        return 1;

    return 0;
}
