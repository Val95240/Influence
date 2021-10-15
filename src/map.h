
#ifndef MAP_H
#define MAP_H

#include <random>
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
        void switch_team(CellCoords cell_coords, bool backward=false);
        void change_value(CellCoords cell_coords, bool backward=false);
        void toggle_links(CellCoords cell_coords);
        void toggle_link(CellCoords cell_coords, int dir);

        CellCoords get_neighbour_coords(CellCoords cell_coords, int dir) const;

        bool attack(CellCoords src, CellCoords dst);
        int count_cells(int team) const;
        int get_growth_limit(int team) const;
        std::vector<double> get_team_percent() const;
        bool grow_cell(CellCoords cell_coords);
        void grow_random_cells(int team, int nb_cells);

        void clear_links();
        bool is_valid() const;

        void save(std::string const& path) const;
        void load(std::string const& path);

        void debug() const;

    private:
        std::default_random_engine rng;
        void initialize(int height, int width, int nb_teams);
        int count_total_value(int team) const;

        std::vector<uint16_t> serialize() const;
        void unserialize(std::vector<uint16_t> const& data);
};


#endif // MAP_H
