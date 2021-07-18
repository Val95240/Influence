
#include "map.h"

#include <math.h>
#include <stdlib.h>

#include <fstream>


static bool random_chance(double limit) {
    double x = rand() / (RAND_MAX + 1.0);
    return x < limit;
}

static int distance(int src_x, int src_y, int dst_x, int dst_y) {
    int dx = std::abs(dst_x - src_x);
    int dy = std::abs(dst_y - src_y);
    bool penalty = (src_x % 2 == 0 && dst_x % 2 == 1 && (src_y < dst_y)) || (dst_x % 2 == 0 && src_x % 2 == 1 && (dst_y < src_y));

    return std::max(dx, dy + dx / 2 + penalty);
}


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

void Map::switch_team(int x, int y, bool backward) {
    Cell& cell = grid[x][y];

    if (!cell.exists) {
        cell.exists = true;
        cell.team = (backward ? nb_teams : 0);
        cell.value = 1;
        for (int dir=1; dir<7; dir++) {
            auto [neigh_x, neigh_y] = get_neighbor_pos(x, y, dir);
            if (neigh_x == -1)
                continue;

            cell.links[dir-1] = true;
            grid[neigh_x][neigh_y].links[(dir+2)%6] = true;
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

void Map::change_value(int x, int y, bool backward) {
    Cell& cell = grid[x][y];

    if (!cell.exists)
        return;

    int limit = (cell.limit_12 ? 12 : 8);
    if ((backward && cell.value > 0) || (!backward && cell.value < limit))
        cell.value += (backward ? -1 : 1);
}

void Map::toggle_links(int x, int y) {
    Cell& cell = grid[x][y];

    bool all_true = true;
    for (int dir=1; dir<7; dir++) {
        auto [neigh_x, neigh_y] = get_neighbor_pos(x, y, dir);
        if (neigh_x != -1 && !cell.links[dir-1]) {
            all_true = false;
            break;
        }
    }

    for (int dir=1; dir<7; dir++) {
        auto [neigh_x, neigh_y] = get_neighbor_pos(x, y, dir);
        if (neigh_x == -1)
            continue;

        cell.links[dir-1] = !all_true;
        grid[neigh_x][neigh_y].links[(dir+2)%6] = cell.links[dir-1];
    }
}

void Map::toggle_link(int x, int y, int dir) {
    auto [neigh_x, neigh_y] = get_neighbor_pos(x, y, dir);
    if (neigh_x == -1)
        return;

    grid[x][y].links[dir-1] = !grid[x][y].links[dir-1];
    grid[neigh_x][neigh_y].links[(dir+2)%6] = grid[x][y].links[dir-1];
}

std::pair<int, int> Map::get_neighbor_pos(int x, int y, int dir) const {
    int neigh_x;
    int neigh_y;

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

bool Map::attack(int src_x, int src_y, int dst_x, int dst_y) {
    Cell& src = grid[src_x][src_y];
    Cell& dst = grid[dst_x][dst_y];

    if (!src.exists || !dst.exists || src.value < 2 || src.team == dst.team)
        return false;

    if (distance(src_x, src_y, dst_x, dst_y) > 1)
        return false;

    if (src.value < dst.value - 1) {
        dst.value -= src.value;
        src.value = 1;

    } else if (src.value > dst.value + 1) {
        dst.value = src.value - std::max(dst.value, 1);
        src.value = 1;
        dst.team = src.team;

        if (!dst.limit_12 && dst.value > 8) {
            src.value += dst.value - 9;
            dst.value = 8;
        }

    } else {
        double proba = 0.50 + 0.25 * (src.value - dst.value);
        src.value = 1;
        dst.value = 1;
        if (random_chance(proba))
            dst.team = src.team;
    }

    return true;
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

bool Map::grow_cell(int x, int y) {
    Cell& cell = grid[x][y];

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
                auto [x_neigh, y_neigh] = get_neighbor_pos(i, j, dir);
                if (x_neigh == -1 || !grid[x_neigh][y_neigh].exists)
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

                auto [neigh_x, neigh_y] = get_neighbor_pos(i, j, dir);
                if (neigh_x == -1)
                    continue;

                Cell neigh_cell = grid[neigh_x][neigh_y];
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

