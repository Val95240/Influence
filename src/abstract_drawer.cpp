
#include "abstract_drawer.h"


AbstractDrawer::AbstractDrawer(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font, TTF_Font* font_small, int banner_height) :
    window(window), renderer(renderer), font(font), font_small(font_small), banner_height(banner_height) {
}

void AbstractDrawer::set_map(Map const& map) {
    this->map = &map;
}

std::pair<int, int> AbstractDrawer::get_cell_at_coords(int x, int y) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);
    win_height -= banner_height;

    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            SDL_Point cell_pos = get_cell_pos(i, j);
            int radius = (map->grid[i][j].limit_12 ? RADIUS_12 : RADIUS);
            double dist = (cell_pos.x - x) * (cell_pos.x - x) + (cell_pos.y - y) * (cell_pos.y - y);
            if (dist < radius*radius)
                return {i, j};
        }
    }
    return {-1, -1};
}


static void compute_hex_coords(int radius, SDL_Point pos, short int hex_x[6], short int hex_y[6]) {
    for (int i=0; i<6; i++) {
        hex_x[i] = pos.x + HEX_DX[i] * radius;
        hex_y[i] = pos.y + HEX_DY[i] * radius;
    }
}

SDL_Point AbstractDrawer::get_cell_pos(int x, int y) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);
    win_height -= banner_height;

    int x_pos = 40 + y * ((double)(win_width - 80) / (map->width - 1));
    int y_pos = 40 + x * ((double)(win_height - 80) / (map->height - 1));

    if (x % 2 == 1)
        x_pos += ((double)(win_width - 80)) / (map->width - 1) / 2.0;

    return {x_pos, y_pos};
}

SDL_Point AbstractDrawer::get_cell_pos(int x, int y, int dir) const {
    SDL_Point cell_pos = get_cell_pos(x, y);
    int radius = (map->grid[x][y].limit_12 ? RADIUS_12 : RADIUS);

    cell_pos.x += radius * HEX_DX[dir-1];
    cell_pos.y += radius * HEX_DY[dir-1];

    return cell_pos;
}

void AbstractDrawer::draw_banner_lines(int win_width, int win_height) const {
    std::vector<double> percents = map->get_team_percent();
    double cumulative_percentage = 0;
    for (size_t team=0; team<percents.size(); team++) {
        if (percents[team] == 0)
            continue;

        uint32_t color = TEAM_COLORS[team+1];
        int x_init = cumulative_percentage * win_width;
        int x_end = x_init + percents[team] * win_width;
        thickLineColor(renderer, x_init, win_height - banner_height, x_end, win_height - banner_height, 10, color | 0xFF000000);
        thickLineColor(renderer, x_init, win_height - 5, x_end, win_height - 5, 10, color | 0xFF000000);

        cumulative_percentage += percents[team];
    }

    if (cumulative_percentage == 0) {
        thickLineColor(renderer, 0, win_height - banner_height, win_width, win_height - banner_height, 5, TEAM_COLORS[0] | 0xFF000000);
        thickLineColor(renderer, 0, win_height - 5, win_width, win_height - 5, 5, TEAM_COLORS[0] | 0xFF000000);
    }
}

bool AbstractDrawer::draw_text(std::string text, SDL_Rect pos, SDL_Color color, bool small) const {
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    char const *ctext = text.c_str();
    SDL_Surface* textSurface = TTF_RenderText_Solid((small ? font_small : font), ctext, color);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == NULL) {
        printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    int text_width = textSurface->w / 2.5;
    int text_height = textSurface->h / 3.0;

    if (pos.x < 0)
        pos.x = (win_width - text_width) / 2;
    if (pos.h < 0) {
        pos.h = text_height;
        pos.y -= text_height / 2;
    }
    if (pos.w < 0)
        pos.w = text_width;

    SDL_RenderCopy(renderer, texture, NULL, &pos);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);

    return true;
}

bool AbstractDrawer::draw_text(std::string text, int y, SDL_Color color, bool small) const {
    return draw_text(text, {-1, y, -1, -1}, color, small);
}

bool AbstractDrawer::draw_text_center(std::string text, SDL_Rect pos, SDL_Color color) const {
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    char const *ctext = text.c_str();
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, ctext, color);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == NULL) {
        printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    int text_width = textSurface->w / 2.5;

    pos.x = pos.x + pos.w / 2.0 - text_width / 2.0;
    pos.w = text_width;

    SDL_RenderCopy(renderer, texture, NULL, &pos);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);

    return true;
}

void AbstractDrawer::draw_cell(int x, int y, int radius, int value, uint32_t color, bool limit_12) const {
    SDL_Point cell_pos = get_cell_pos(x, y);

    // Outer hex (cell outline)
    short int hex_x[6], hex_y[6];
    compute_hex_coords(radius, cell_pos, hex_x, hex_y);
    polygonColor(renderer, hex_x, hex_y, 6, color | 0xFF000000);

    if (value < 0)
        return;

    // Inner hex
    double percent_value = value / (limit_12 ? 12.0 : 8.0);
    int inner_radius = (INNER_MIN_RADIUS + (1 - INNER_MIN_RADIUS) * percent_value) * radius;
    compute_hex_coords(inner_radius, cell_pos, hex_x, hex_y);
    filledPolygonColor(renderer, hex_x, hex_y, 6, color | 0xFF000000);

    // Text value
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    std::string text_value = std::to_string(value);
    SDL_Rect text_pos;
    text_pos.x = cell_pos.x - 5 * text_value.length();
    text_pos.y = cell_pos.y - 10;
    text_pos.w = 10 * text_value.length();
    text_pos.h = 20;

    draw_text(text_value, text_pos, value_color);
}

void AbstractDrawer::draw_links() const {
    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            if (!map->grid[i][j].exists)
                continue;

            for (int dir=1; dir<4; dir++) {
                if (map->grid[i][j].links[dir-1])
                    draw_link(i, j, dir);
            }
        }
    }
}

void AbstractDrawer::draw_link(int x, int y, int dir) const {
    auto [x_target, y_target] = map->get_neighbor_pos(x, y, dir);
    if (x_target == -1 || !map->grid[x_target][y_target].exists)
        return;

    SDL_Point cell_pos = get_cell_pos(x, y, dir);
    SDL_Point target_pos = get_cell_pos(x_target, y_target, dir + 3);

    uint32_t color;
    if (map->grid[x][y].team == map->grid[x_target][y_target].team)
        color = TEAM_COLORS[map->grid[x][y].team];
    else
        color = TEAM_COLORS[0];

    SDL_SetRenderDrawColor(renderer, color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, 0xFF);
    SDL_RenderDrawLine(renderer, cell_pos.x, cell_pos.y, target_pos.x, target_pos.y);
}
