
#ifndef ABSTRACT_DRAWER_H
#define ABSTRACT_DRAWER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "cell.hpp"
#include "map.hpp"


int const RADIUS_8 = 22;
int const RADIUS_12 = 28;
double const INNER_MIN_RADIUS = 0.55;
double const SQRT_3_2 = 0.8660254038;

double const HEX_DX[6] = {1/2.0, 1, 1/2.0, -1/2.0, -1, -1/2.0};
double const HEX_DY[6] = {-SQRT_3_2, 0, SQRT_3_2, SQRT_3_2, 0, -SQRT_3_2};

uint32_t const TEAM_COLORS[6] = {0x5C5C5C, 0xF0B040, 0x509010, 0x60D0FF, 0x5050E0, 0xF06090};
uint32_t const INACTIVE_COLOR = 0x856D0B;
uint32_t const FOCUS_COLOR[6] = {0xA0A0A0, 0xFFE070, 0x20E020, 0x50FFFF, 0x9090FF, 0xF090B0};


class AbstractDrawer {
    public:
        AbstractDrawer(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* font_small, int banner_height);
        virtual ~AbstractDrawer();

        void set_map(Map const& map);

        CellCoords get_cell_at_coords(int x, int y) const;

    protected:
        Map const* map = NULL;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        TTF_Font* font = NULL;
        TTF_Font* font_small = NULL;
        SDL_Color value_color = {0, 0, 0, 0};

        int const banner_height;
        SDL_Color banner_color = {0xFF, 0xFF, 0xFF, 0xFF};
        SDL_Color banner_color_2 = {0x70, 0x70, 0x70, 0xFF};

        SDL_Point get_cell_pos(CellCoords cell_coords) const;
        SDL_Point get_cell_pos(CellCoords cell_coords, int dir) const;

        virtual void draw_map(CellCoords focus_coords, std::string banner_text);
        virtual void draw_banner(std::string banner_text) const = 0;
        virtual void draw_cell(CellCoords cell_coords, bool focus) const = 0;

        void draw_banner_lines(int win_width, int win_height) const;
        bool draw_text(std::string text, SDL_Rect pos, SDL_Color color, bool small=false) const;
        bool draw_text(std::string text, int y, SDL_Color color, bool small=false) const;
        bool draw_text_center(std::string text, SDL_Rect pos, SDL_Color color) const;
        void draw_cell(CellCoords cell_coords, uint32_t color, bool display_empty=false) const;
        void draw_links() const;
        void draw_link(CellCoords cell_coords, int dir) const;
};


#endif // ABSTRACT_DRAWER_H
