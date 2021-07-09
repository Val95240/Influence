
#ifndef GENERATE_MAP_H
#define GENERATE_MAP_H

#include <fstream>
#include <sstream>
#include <string>

#include "cell.h"
#include "map.h"


class MapGenerator {
    public:
        static void from_str_file(std::string const& src, std::string const& dst);
};

#endif // GENERATE_MAP_H
