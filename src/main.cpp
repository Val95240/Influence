
#include <iostream>

#include "cell.h"
#include "map.h"
#include "map_drawer.h"


int main(int argc, char* argv[]) {
    Map map("maps/example.map");
    map.debug();

    MapDrawer drawer;
    if (!drawer.init()) {
        std::cout << "Error in SDL initialization: " << drawer.errmsg() << std::endl;
        return -1;
    }

    drawer.draw(map);

    return 0;
}
