
#include "map_generator.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "cell.hpp"
#include "map.hpp"


bool MapGenerator::from_str_file(std::string const& src, std::string const& dst) {
    std::ifstream file(src);
    if (!file) {
        std::cout << "Couldn't open file " << src << std::endl;
        return false;
    }

    try {
        std::string line;
        std::getline(file, line);
        size_t index1 = line.find(',');
        size_t index2 = line.find(',', index1+1);
        int height = std::stoi(line.substr(0, index1));
        int width = std::stoi(line.substr(index1+1, index2-index1-1));
        int nb_teams = std::stoi(line.substr(index2+1));

        Map map(height, width, nb_teams);

        for (int i=0; i<height; i++) {
            std::getline(file, line);
            std::stringstream line_stream(line);
            std::string token;

            for (int j=0; j<width; j++) {
                std::getline(line_stream, token, ',');
                map.grid[i][j].read_from_token(token);
            }

        }
        file.close();

        map.clear_links();
        if (!map.is_valid()) {
            std::cout << "Invalid map: links are not consistent" << std::endl;
            return false;
        }

        map.save(dst);
        return true;

    } catch (...) {
        file.close();
        std::cout << "Invalid map format. Are you sure " << src << " is a valid csv map file ?" << std::endl;
        return false;
    }
}
