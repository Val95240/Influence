
#ifndef ABSTRACT_WINDOW_H
#define ABSTRACT_WINDOW_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "map.h"


class AbstractWindow {
    public:
        virtual ~AbstractWindow();

        virtual bool init();
        virtual void run(Map& map) = 0;

        char* errmsg() const;

    protected:
        const int screen_width = 680;
        const int screen_height = 1020;

        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        TTF_Font* font = NULL;
        TTF_Font* font_small = NULL;

        char* error_msg;
};


#endif // ABSTRACT_WINDOW_H
