
#ifndef MAP_H
#define MAP_H

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "cell.h"
#include "map_generator.h"


using Grid = std::vector<std::vector<Cell>>;

class Map {
    public:
        Map(int height, int width);
        Map(std::string const& path);

        void save(std::string const& path) const;
        void load(std::string const& path);

        void debug() const;

    private:
        int height;
        int width;
        Grid grid;

        void initialize(int height, int width);

        std::vector<uint16_t> serialize() const;
        void unserialize(std::vector<uint16_t> const& data);

    friend class MapGenerator;
    friend class MapDrawer;
};

#endif // MAP_H
