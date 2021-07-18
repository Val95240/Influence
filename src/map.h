
#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

#include "cell.h"
#include "map_generator.h"


using Grid = std::vector<std::vector<Cell>>;

class Map {
    public:
        Grid grid;
        int height;
        int width;
        int nb_teams;

        Map(int height, int width, int nb_teams);
        Map(std::string const& path);

        void resize(int height, int width);
        void modify_nb_teams(int new_nb_teams);
        void switch_team(int x, int y, bool backward=false);
        void change_value(int x, int y, bool backward=false);
        void toggle_links(int x, int y);
        void toggle_link(int x, int y, int dir);

        std::pair<int, int> get_neighbor_pos(int x, int y, int dir) const;

        bool attack(int src_x, int src_y, int dst_x, int dst_y);
        int count_cells(int team) const;
        std::vector<double> get_team_percent() const;
        bool grow_cell(int x, int y);

        void clear_links();
        bool is_valid() const;

        void save(std::string const& path) const;
        void load(std::string const& path);

        void debug() const;

    private:
        void initialize(int height, int width, int nb_teams);
        int count_total_value(int team) const;

        std::vector<uint16_t> serialize() const;
        void unserialize(std::vector<uint16_t> const& data);
};


#endif // MAP_H
