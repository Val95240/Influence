
#ifndef GENERATE_MAP_H
#define GENERATE_MAP_H

#include <fstream>
#include <sstream>
#include <string>

#include "cell.h"
#include "map.h"


class Map;

class MapGenerator {
    public:
        static bool from_str_file(std::string const& src, std::string const& dst);

    private:
        static void clear_links(Map& map);
        static bool is_valid(Map const& map);
};

#endif // GENERATE_MAP_H
