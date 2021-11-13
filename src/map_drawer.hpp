
#ifndef MAP_DRAWER_H
#define MAP_DRAWER_H

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "abstract_drawer.hpp"
#include "cell.hpp"
#include "map.hpp"


class MapDrawer : public AbstractDrawer {
    public:
        using AbstractDrawer::AbstractDrawer;

        void set_map(Map const& map);
        void draw_map(CellCoords focus_coords, int phase, int nb_cells_to_grow);
        void draw_gameover(bool won);

    private:
        std::vector<std::vector<bool>> active_cells;

        bool no_active_cell() const;
        void reset_active_cells();
        void mark_active_cells();
        void draw_banner(std::string banner_text) const override;
        void draw_cell(CellCoords cell_coords, bool focus) const override;
};


#endif // MAP_DRAWER_H
