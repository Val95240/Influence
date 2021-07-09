
#ifndef MAP_DRAWER_H
#define MAP_DRAWER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <string>

#include "cell.h"
#include "map.h"


class MapDrawer {
    public:
        ~MapDrawer();

        bool init();
        void run(Map const& map) const;

        char* errmsg() const;

    private:
        const int screen_width = 720;
        const int screen_height = 930;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        TTF_Font* font = NULL;
        SDL_Color text_color = {0, 0, 0, 0};

        char* error_msg;

        std::pair<int, int> get_cell_pos(Map const& map, int x, int y) const;
        std::pair<int, int> get_cell_pos(Map const& map, int x, int y, int dir) const;
        void draw_map(Map const& map) const;
        void draw_cell(Map const& map, int x, int y) const;
        void draw_links(Map const& map) const;
        void draw_link(Map const& map, int x, int y, int dir) const;
};


#endif // MAP_DRAWER_H

// vim: path+=/usr/include/SDL2,/usr/local/include/SDL2
