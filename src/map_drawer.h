
#ifndef MAP_DRAWER_H
#define MAP_DRAWER_H

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "abstract_drawer.h"
#include "cell.h"
#include "map.h"


class MapDrawer : public AbstractDrawer {
    public:
        using AbstractDrawer::AbstractDrawer;

        void set_map(Map const& map);
        void draw_map(CellCoords focus_coords, int phase, int nb_cells_to_grow);

        bool no_active_cell() const;

    private:
        std::vector<std::vector<bool>> active_cells;

        void reset_active_cells();
        void mark_active_cells();
        void draw_banner(std::string banner_text) const override;
        void draw_cell(CellCoords cell_coords, bool focus) const override;
};


#endif // MAP_DRAWER_H
