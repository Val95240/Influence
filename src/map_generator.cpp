
#include "map_generator.h"

#include "map.h"


void MapGenerator::from_str_file(std::string const& src, std::string const& dst) {
    std::ifstream file(src);
    if (!file) {
        std::cout << "Couldn't open file " << src << std::endl;
        return;
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

    map.save(dst);
}
