
#include <filesystem>
#include <iostream>

#include "abstract_window.hpp"
#include "arena.hpp"
#include "editor_window.hpp"
#include "main_window.hpp"


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

    if (edit) {
        EditorWindow window(map_file);
        if (!window.init()) {
            std::cout << "Error in SDL initialization: " << window.errmsg() << std::endl;
            return -1;
        }

        Map map(map_file);
        window.run(map);

    } else {
        MainWindow window;
        if (!window.init()) {
            std::cout << "Error in SDL initialization: " << window.errmsg() << std::endl;
            return -1;
        }

        Arena arena(map_file);
        window.run(arena);
    }

    return 0;
}
