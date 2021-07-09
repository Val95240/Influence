
#include "map_generator.h"

#include "map.h"


bool MapGenerator::from_str_file(std::string const& src, std::string const& dst) {
    std::ifstream file(src);
    if (!file) {
        std::cout << "Couldn't open file " << src << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);
    size_t index = line.find(',');
    int height = std::stoi(line.substr(0, index));
    int width = std::stoi(line.substr(index+1));

    Map map(height, width);

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

    clear_links(map);
    if (!is_valid(map)) {
        std::cout << "Invalid map: links are not consistent" << std::endl;
        return false;
    }

    map.save(dst);
    return true;
}


void MapGenerator::clear_links(Map& map) {
    for (int i=0; i<map.height; i++) {
        for (int j=0; j<map.width; j++) {
            if (!map.grid[i][j].exists)
                continue;

            for (int dir=1; dir<7; dir++) {
                auto [x_neigh, y_neigh] = map.get_neighbor_pos(i, j, dir);
                if (x_neigh == -1 || !map.grid[x_neigh][y_neigh].exists)
                    map.grid[i][j].links[dir-1] = false;
            }
        }
    }
}

bool MapGenerator::is_valid(Map const& map) {
    for (int i=0; i<map.height; i++) {
        for (int j=0; j<map.width; j++) {
            if (!map.grid[i][j].exists)
                continue;

            for (int dir=1; dir<7; dir++) {
                Cell curr_cell = map.grid[i][j];
                if (!curr_cell.links[dir-1])
                    continue;

                auto [x_neigh, y_neigh] = map.get_neighbor_pos(i, j, dir);
                if (x_neigh == -1)
                    continue;

                Cell neigh_cell = map.grid[x_neigh][y_neigh];
                if (!neigh_cell.exists || !neigh_cell.links[(dir+2)%6])
                    return false;
            }
        }
    }
    return true;
}
