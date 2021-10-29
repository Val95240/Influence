
#ifndef MAP_H
#define MAP_H

#include <random>
#include <string>
#include <vector>

#include "cell.hpp"
#include "map_generator.hpp"


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
        void switch_team(CellCoords cell_coords, bool backward=false);
        void change_value(CellCoords cell_coords, bool backward=false);
        void toggle_links(CellCoords cell_coords);
        void toggle_link(CellCoords cell_coords, int dir);

        CellCoords get_neighbour_coords(CellCoords cell_coords, int dir) const;

        int count_cells(int team) const;
        std::vector<double> get_team_percent() const;
        bool grow_cell(int team, CellCoords cell_coords);

        void clear_links();
        bool is_valid() const;

        void save(std::string const& path) const;
        void load(std::string const& path);

        void debug() const;

    protected:
        void initialize(int height, int width, int nb_teams);
        int count_total_value(int team) const;

        std::vector<uint16_t> serialize() const;
        void unserialize(std::vector<uint16_t> const& data);
};


#endif // MAP_H
