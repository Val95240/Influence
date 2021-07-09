
#include "map_drawer.h"

int const RADIUS = 20;
int const SQRT_3_2_R = 0.8660254038 * RADIUS;

short int const HEX_DX[6] = {RADIUS/2, RADIUS, RADIUS/2, -RADIUS/2, -RADIUS, -RADIUS/2};
short int const HEX_DY[6] = {-SQRT_3_2_R, 0, SQRT_3_2_R, SQRT_3_2_R, 0, -SQRT_3_2_R};

uint32_t const TEAM_COLORS[5] = {0xFF5C5C5C, 0xFFF0B03A, 0xFF58920B, 0xFF5CD7FF, 0xFF5752DD};


MapDrawer::~MapDrawer() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

bool MapDrawer::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::sprintf(error_msg, "SDL could not initialize: %s\n", SDL_GetError());
        return false;
	}

    window = SDL_CreateWindow("Influence", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screen_width, screen_height, SDL_WINDOW_HIDDEN);
    if (window == NULL) {
        std::sprintf(error_msg, "Window could not be created: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::sprintf(error_msg, "Renderer could not be created: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void MapDrawer::draw(Map const& map) const {
    SDL_ShowWindow(window);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                quit = true;

            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q)
                    quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        for (int i=0; i<map.height; i++) {
            for (int j=0; j<map.width; j++) {
                draw_cell(map, i, j);
            }
        }

        SDL_RenderPresent(renderer);
    }
}

char* MapDrawer::errmsg() const {
    return error_msg;
}


void MapDrawer::draw_cell(Map const& map, int x, int y) const {
    Cell cell = map.grid[x][y];
    int x_pos = 40 + y * (screen_width / map.width);
    int y_pos = 40 + x * (screen_height / map.height);

    short int hex_x[6], hex_y[6];
    for (int i=0; i<6; i++) {
        hex_x[i] = x_pos + HEX_DX[i];
        hex_y[i] = y_pos + HEX_DY[i];
    }
    uint32_t color = TEAM_COLORS[cell.team];

    filledPolygonColor(renderer, hex_x, hex_y, 6, color);
}
