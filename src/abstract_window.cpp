
#include "abstract_window.hpp"

#include <cstdio>


AbstractWindow::~AbstractWindow() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    TTF_CloseFont(font);
    font = NULL;

    TTF_CloseFont(font_small);
    font_small = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    TTF_Quit();
    SDL_Quit();
}

bool AbstractWindow::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::sprintf(error_msg, "SDL could not initialize: %s\n", SDL_GetError());
        return false;
	}

    if (TTF_Init() == -1) {
        std::printf(error_msg, "SDL TTF could not initialize: %s\n", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow("Influence", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screen_width, screen_height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        std::sprintf(error_msg, "Window could not be created: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::sprintf(error_msg, "Renderer could not be created: %s\n", SDL_GetError());
        return false;
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/arial.ttf", 100);
    if (font == NULL) {
        std::sprintf(error_msg, "Font could not be loaded: %s\n", TTF_GetError());
        return false;
    }

    font_small = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/arial.ttf", 60);
    if (font_small == NULL) {
        std::sprintf(error_msg, "Font could not be loaded: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

char* AbstractWindow::errmsg() const {
    return error_msg;
}

