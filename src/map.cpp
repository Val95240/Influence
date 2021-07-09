
#include "map.h"


Map::Map(int height, int width) {
    initialize(height, width);
}

Map::Map(std::string const& path) {
    load(path);
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
    std::cout << "Map: " << std::endl;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            std::cout << grid[i][j];
        }
        std::cout << std::endl;
    }
}


void Map::initialize(int height, int width) {
    this->height = height;
    this->width = width;
    grid.clear();

    for (int i=0; i<height; i++) {
        std::vector<Cell> row;
        for (int j=0; j<width; j++) {
            row.push_back(Cell());
        }
        grid.push_back(row);
    }
}

std::vector<uint16_t> Map::serialize() const {
    std::vector<uint16_t> data;
    data.push_back((uint16_t) height);
    data.push_back((uint16_t) width);

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

    initialize(width, height);

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            grid[i][j].unserialize(data[index++]);
        }
    }
}

