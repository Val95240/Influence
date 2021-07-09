
#include <filesystem>
#include <iostream>

#include "cell.h"
#include "map.h"
#include "map_drawer.h"


int main(int argc, char* argv[]) {
    std::string map_file;
    if (argc < 2)
        map_file = "maps/example.map";
    else
        map_file = argv[1];

    if (!std::filesystem::exists(map_file)) {
        std::cout << "Map file not found: " << map_file << std::endl;
        return 1;
    }

    Map map(map_file);
    /* map.debug(); */

    MapDrawer drawer;
    if (!drawer.init()) {
        std::cout << "Error in SDL initialization: " << drawer.errmsg() << std::endl;
        return -1;
    }

    drawer.run(map);

    return 0;
}
