
#include "map.h"

#include <fstream>


Map::Map(int height, int width, int nb_teams) {
    initialize(height, width, nb_teams);
}

Map::Map(std::string const& path) {
    load(path);
}

void Map::resize(int new_height, int new_width) {
    Grid new_grid;

    for (int i=0; i<new_height; i++) {
        std::vector<Cell> row;
        for (int j=0; j<new_width; j++) {
            if (i % 2 == 1 && j == width-1)
                row.push_back(Cell());
            else if (i < height && j < width)
                row.push_back(grid[i][j]);
            else
                row.push_back(Cell());
        }
        new_grid.push_back(row);
    }

    this->height = new_height;
    this->width = new_width;
    grid = new_grid;
    clear_links();
}

void Map::modify_nb_teams(int new_nb_teams) {
    if (new_nb_teams > nb_teams) {
        nb_teams = new_nb_teams;
        return;
    }

    nb_teams = new_nb_teams;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (grid[i][j].team > nb_teams)
                grid[i][j].team = 0;
        }
    }
}

void Map::switch_team(CellCoords cell_coords, bool backward) {
    Cell& cell = grid[cell_coords.x][cell_coords.y];

    if (!cell.exists) {
        cell.exists = true;
        cell.team = (backward ? nb_teams : 0);
        cell.value = 1;
        for (int dir=1; dir<7; dir++) {
            CellCoords neigh_coords = get_neighbour_coords(cell_coords, dir);
            if (neigh_coords.x == -1)
                continue;

            cell.links[dir-1] = true;
            grid[neigh_coords.x][neigh_coords.y].links[(dir+2)%6] = true;
        }
    }

    else if ((backward && cell.team == 0) || (!backward && cell.team == nb_teams)) {
        cell.exists = false;
        cell.team = 0;
        cell.value = 0;
    }

    else
        cell.team = (cell.team + nb_teams + 1 + (backward ? -1 : 1)) % (nb_teams + 1);
}

void Map::change_value(CellCoords cell_coords, bool backward) {
    Cell& cell = grid[cell_coords.x][cell_coords.y];

    if (!cell.exists)
        return;

    int limit = (cell.limit_12 ? 12 : 8);
    if ((backward && cell.value > 0) || (!backward && cell.value < limit))
        cell.value += (backward ? -1 : 1);
}

void Map::toggle_links(CellCoords cell_coords) {
    Cell& cell = grid[cell_coords.x][cell_coords.y];

    bool all_true = true;
    for (int dir=1; dir<7; dir++) {
        CellCoords neigh_coords = get_neighbour_coords(cell_coords, dir);
        if (neigh_coords.x != -1 && !cell.links[dir-1]) {
            all_true = false;
            break;
        }
    }

    for (int dir=1; dir<7; dir++) {
        CellCoords neigh_coords = get_neighbour_coords(cell_coords, dir);
        if (neigh_coords.x == -1)
            continue;

        cell.links[dir-1] = !all_true;
        grid[neigh_coords.x][neigh_coords.y].links[(dir+2)%6] = cell.links[dir-1];
    }
}

void Map::toggle_link(CellCoords cell_coords, int dir) {
    CellCoords neigh_coords = get_neighbour_coords(cell_coords, dir);
    if (neigh_coords.x == -1)
        return;

    grid[cell_coords.x][cell_coords.y].links[dir-1] = !grid[cell_coords.x][cell_coords.y].links[dir-1];
    grid[neigh_coords.x][neigh_coords.y].links[(dir+2)%6] = grid[cell_coords.x][cell_coords.y].links[dir-1];
}

CellCoords Map::get_neighbour_coords(CellCoords cell_coords, int dir) const {
    auto [x, y] = cell_coords;
    int neigh_x, neigh_y;

    if (dir == 1) {
        neigh_x = x - 1;
        neigh_y = y + (x % 2);
    } else if (dir == 2) {
        neigh_x = x;
        neigh_y = y + 1;
    } else if (dir == 3) {
        neigh_x = x + 1;
        neigh_y = y + (x % 2);
    } else if (dir == 4) {
        neigh_x = x + 1;
        neigh_y = y - !(x % 2);
    } else if (dir == 5) {
        neigh_x = x;
        neigh_y = y - 1;
    } else if (dir == 6) {
        neigh_x = x - 1;
        neigh_y = y - !(x % 2);
    }

    if (!(0 <= neigh_x && neigh_x < height && 0 <= neigh_y && neigh_y < width))
        return {-1, -1};

    return {neigh_x, neigh_y};
}

int Map::count_cells(int team) const {
    int count = 0;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            count += (grid[i][j].exists && grid[i][j].team == team);
        }
    }
    return count;
}

std::vector<double> Map::get_team_percent() const {
    int total_value = count_total_value(0);

    std::vector<double> percents;
    for (int team=1; team<nb_teams+1; team++) {
        int team_value = count_total_value(team);
        if (team_value == 0) {
            percents.push_back(0);
        } else {
            percents.push_back((double)team_value / total_value);
        }
    }

    return percents;
}

bool Map::grow_cell(CellCoords cell_coords) {
    Cell& cell = grid[cell_coords.x][cell_coords.y];

    if (!cell.exists || cell.team != 1 || cell.value >= (cell.limit_12 ? 12 : 8))
        return false;

    cell.value++;
    return true;
}

void Map::clear_links() {
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (!grid[i][j].exists)
                continue;

            for (int dir=1; dir<7; dir++) {
                CellCoords neigh_coords = get_neighbour_coords({i, j}, dir);
                if (neigh_coords.x == -1 || !grid[neigh_coords.x][neigh_coords.y].exists)
                    grid[i][j].links[dir-1] = false;
            }
        }
    }
}

bool Map::is_valid() const {
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (!grid[i][j].exists)
                continue;

            for (int dir=1; dir<7; dir++) {
                Cell curr_cell = grid[i][j];
                if (!curr_cell.links[dir-1])
                    continue;

                CellCoords neigh_coords = get_neighbour_coords({i, j}, dir);
                if (neigh_coords.x == -1)
                    continue;

                Cell neigh_cell = grid[neigh_coords.x][neigh_coords.y];
                if (!neigh_cell.exists || !neigh_cell.links[(dir+2)%6])
                    return false;
            }
        }
    }
    return true;
}

void Map::save(std::string const& path) const {
    std::vector<uint16_t> data = serialize();
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cout << "Couldn't open file " << path << std::endl;
        return;
    }

    file.write((char*) &data[0], data.size() * sizeof(uint16_t));
    file.close();

    if (!file.good())
        std::cout << "Something went wrong when writing map to " << path << std::endl;
}

void Map::load(std::string const& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cout << "Couldn't open file " << path << std::endl;
        initialize(2, 2, 2);
        return;
    }

    std::random_device rd;
    rng = std::default_random_engine{rd()};

    file.seekg(0, std::ios::end);
    size_t filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint16_t> data;
    data.resize(filesize / sizeof(uint16_t));

    file.read((char *)data.data(), filesize);
    file.close();

    if (!file.good())
        std::cout << "Something went wrong when reading map from " << path << std::endl;

    unserialize(data);
}

void Map::debug() const {
    std::cout << "Map: (" << height << ", " << width << ", " << nb_teams << ")" << std::endl;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            std::cout << grid[i][j];
        }
        std::cout << std::endl;
    }
}


void Map::initialize(int height, int width, int nb_teams) {
    std::random_device rd;
    rng = std::default_random_engine{rd()};

    this->height = height;
    this->width = width;
    this->nb_teams = nb_teams;
    grid.clear();

    for (int i=0; i<height; i++) {
        std::vector<Cell> row;
        for (int j=0; j<width; j++) {
            row.push_back(Cell());
        }
        grid.push_back(row);
    }
}

int Map::count_total_value(int team) const {
    int total_value = 0;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (!grid[i][j].exists)
                continue;

            if ((team > 0 && grid[i][j].team == team) || (team == 0 && grid[i][j].team > 0))
                total_value += grid[i][j].value;
        }
    }

    return total_value;
}

std::vector<uint16_t> Map::serialize() const {
    std::vector<uint16_t> data;
    data.push_back((uint16_t) height);
    data.push_back((uint16_t) width);
    data.push_back((uint16_t) nb_teams);

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            data.push_back(grid[i][j].serialize());
        }
    }
    return data;
}

void Map::unserialize(std::vector<uint16_t> const& data) {
    size_t index = 0;
    width = (int) data[index++];
    height = (int) data[index++];
    nb_teams = (int) data[index++];

    initialize(width, height, nb_teams);

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            grid[i][j].unserialize(data[index++]);
        }
    }
}

