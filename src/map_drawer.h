
#ifndef MAP_DRAWER_H
#define MAP_DRAWER_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <string>

#include "cell.h"
#include "map.h"


class MapDrawer {
    public:
        ~MapDrawer();

        bool init();
        void draw(Map const& map) const;

        char* errmsg() const;

    private:
        const int screen_width = 640;
        const int screen_height = 480;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        char* error_msg;

        void draw_cell(Map const& map, int x, int y) const;
};


#endif // MAP_DRAWER_H

// vim: path+=/usr/include/SDL2,/usr/local/include/SDL2
