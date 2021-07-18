
#include <filesystem>
#include <iostream>

#include "map.h"
#include "abstract_window.h"
#include "main_window.h"
#include "editor_window.h"


int main(int argc, char* argv[]) {
    bool edit = false;
    std::string map_file = "maps/default.map";

    for (int i=1; i<argc; i++) {
        std::string arg = argv[i];

        if (arg == "-e" || arg == "--edit")
            edit = true;
        else
            map_file = arg;
    }

    if (!edit && !std::filesystem::exists(map_file)) {
        std::cout << "Map file not found: " << map_file << std::endl;
        return 1;
    }

    Map map(map_file);
    /* map.debug(); */


    AbstractWindow* window;
    if (edit)
        window = new EditorWindow(map_file);
    else
        window = new MainWindow();

    if (!window->init()) {
        std::cout << "Error in SDL initialization: " << window->errmsg() << std::endl;
        return -1;
    }

    window->run(map);

    delete window;

    return 0;
}
