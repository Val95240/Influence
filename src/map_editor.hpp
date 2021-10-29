
#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "abstract_drawer.hpp"
#include "cell.hpp"


enum BannerAction {
    MODIFY_WIDTH,
    MODIFY_HEIGHT,
    MODIFY_NB_TEAMS,
    SAVE,
    SAVE_AS,
    NOOP
};


class MapEditor : public AbstractDrawer {
    public:
        using AbstractDrawer::AbstractDrawer;

        BannerAction curr_action = BannerAction::NOOP;

        BannerAction click_banner(int x, int y);
        void modify(BannerAction action);
        void draw_map(CellCoords focus_coords);

    private:
        SDL_Color setting_color_1 = {0x70, 0xF0, 0xF0, 0xFF};
        SDL_Color setting_color_2 = {0x10, 0x30, 0xF0, 0xFF};

        void get_button_pos(SDL_Rect pos[4]) const;

        void draw_banner(std::string banner_text) const override;
        void draw_cell(CellCoords cell_coords, bool focus) const override;
};


#endif // MAP_EDITOR_H
