
#include "map_drawer.h"

int const RADIUS = 20;
double const INNER_MIN_RADIUS = 0.55;
double const SQRT_3_2 = 0.8660254038;

double const HEX_DX[6] = {1/2.0, 1, 1/2.0, -1/2.0, -1, -1/2.0};
double const HEX_DY[6] = {-SQRT_3_2, 0, SQRT_3_2, SQRT_3_2, 0, -SQRT_3_2};

uint32_t const TEAM_COLORS[5] = {0x5C5C5C, 0xF0B03A, 0x58920B, 0x5CD7FF, 0x5752DD};


static void compute_hex_coords(int radius, int x, int y, short int hex_x[6], short int hex_y[6]) {
    for (int i=0; i<6; i++) {
        hex_x[i] = x + HEX_DX[i] * radius;
        hex_y[i] = y + HEX_DY[i] * radius;
    }
}

MapDrawer::~MapDrawer() {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    TTF_CloseFont(font);
    font = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    TTF_Quit();
    SDL_Quit();
}

bool MapDrawer::init() {
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
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    return true;
}

void MapDrawer::run(Map const& map) const {
    SDL_ShowWindow(window);

    bool modified = true;
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

            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                modified = true;
        }

        if (modified) {
            modified = false;
            draw_map(map);

            SDL_RenderPresent(renderer);
            SDL_Delay(200);
        }
    }
}

char* MapDrawer::errmsg() const {
    return error_msg;
}


std::pair<int, int> MapDrawer::get_cell_pos(Map const& map, int x, int y) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    int x_pos = 40 + y * ((double)(win_width - 80) / (map.width - 1));
    int y_pos = 40 + x * ((double)(win_height - 80) / (map.height - 1));

    if (x % 2 == 1)
        x_pos += ((double)(win_width - 80)) / (map.width - 1) / 2.0;

    return {x_pos, y_pos};
}

std::pair<int, int> MapDrawer::get_cell_pos(Map const& map, int x, int y, int dir) const {
    auto [x_pos, y_pos] = get_cell_pos(map, x, y);

    x_pos += RADIUS * HEX_DX[dir-1];
    y_pos += RADIUS * HEX_DY[dir-1];

    return {x_pos, y_pos};
}

void MapDrawer::draw_map(Map const& map) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    for (int i=0; i<map.height; i++) {
        for (int j=0; j<map.width; j++)
            draw_cell(map, i, j);
    }

    draw_links(map);
}

void MapDrawer::draw_cell(Map const& map, int x, int y) const {
    Cell cell = map.grid[x][y];
    if (!cell.exists)
        return;

    auto [x_pos, y_pos] = get_cell_pos(map, x, y);
    uint32_t color = TEAM_COLORS[cell.team];

    // Outer hex (cell outline)
    short int hex_x[6], hex_y[6];
    compute_hex_coords(RADIUS, x_pos, y_pos, hex_x, hex_y);
    polygonColor(renderer, hex_x, hex_y, 6, color | 0xFF000000);

    // Inner hex
    double percent_value = cell.value / (cell.limit_12 ? 12.0 : 8.0);
    int inner_radius = (INNER_MIN_RADIUS + (1 - INNER_MIN_RADIUS) * percent_value) * RADIUS;
    compute_hex_coords(inner_radius, x_pos, y_pos, hex_x, hex_y);
    filledPolygonColor(renderer, hex_x, hex_y, 6, color | 0xFF000000);

    // Text value
    char const *text_value = std::to_string(cell.value).c_str();
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text_value, text_color);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == NULL) {
        printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        return;
    }
    SDL_Rect rect = {x_pos-5, y_pos-10, 10, 20};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);
}

void MapDrawer::draw_links(Map const& map) const {
    for (int i=0; i<map.height; i++) {
        for (int j=0; j<map.width; j++) {
            if (!map.grid[i][j].exists)
                continue;

            for (int k=1; k<4; k++) {
                if (map.grid[i][j].links[k-1])
                    draw_link(map, i, j, k);
            }
        }
    }
}

void MapDrawer::draw_link(Map const& map, int x, int y, int dir) const {
    auto [x_target, y_target] = map.get_neighbor_pos(x, y, dir);

    auto [x_pos, y_pos] = get_cell_pos(map, x, y, dir);
    auto [x_target_pos, y_target_pos] = get_cell_pos(map, x_target, y_target, dir + 3);

    uint32_t color;
    if (map.grid[x][y].team == map.grid[x_target][y_target].team)
        color = TEAM_COLORS[map.grid[x][y].team];
    else
        color = TEAM_COLORS[0];

    SDL_SetRenderDrawColor(renderer, color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, 0xFF);
    SDL_RenderDrawLine(renderer, x_pos, y_pos, x_target_pos, y_target_pos);
}
