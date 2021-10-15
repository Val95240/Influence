
#include "map_editor.h"


BannerAction MapEditor::click_banner(int x, int y) {
    SDL_Point click_pos = {x, y};

    SDL_Rect pos[5];
    get_button_pos(pos);
    for (int i=0; i<5; i++) {
        if (SDL_PointInRect(&click_pos, &pos[i])) {
            curr_action = (BannerAction)(i);
            return (BannerAction)(i);
        }
    }

    return BannerAction::NOOP;
}

void MapEditor::draw_map(CellCoords focus_coords) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            bool focus = (i == focus_coords.x && j == focus_coords.y);
            draw_cell(focus_coords, focus);
        }
    }

    draw_links();

    std::string banner_text;
    if (focus_coords.x == -1)
        banner_text = "Select a cell to edit it";
    else
        banner_text = "Modify the cell team with `t` and its value with `[k]` and `[j]`";

    draw_banner(banner_text);
}


void MapEditor::get_button_pos(SDL_Rect pos[5]) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    int small_button_width = (win_width - 40) / 3.0;
    int small_left_x = 10;
    int small_middle_x = small_left_x + small_button_width + 10;
    int small_right_x = small_middle_x + small_button_width + 10;

    int button_width = win_width / 2.0 - 15;
    int left_x = 10;
    int right_x = left_x + button_width + 10;

    int top_y = win_height - 0.85 * banner_height;
    int bottom_y = win_height - 0.45 * banner_height;
    int button_height = 0.3 * banner_height;

    pos[0] = {small_left_x, top_y, small_button_width, button_height};
    pos[1] = {small_middle_x, top_y, small_button_width, button_height};
    pos[2] = {small_right_x, top_y, small_button_width, button_height};

    pos[3] = {left_x, bottom_y, button_width, button_height};
    pos[4] = {right_x, bottom_y, button_width, button_height};
}

void MapEditor::draw_banner(std::string banner_text) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    draw_banner_lines(win_width, win_height);

    SDL_Rect pos[5];
    get_button_pos(pos);

    std::string width = std::to_string(map->width);
    std::string height = std::to_string(map->height);
    std::string nb_teams = std::to_string(map->nb_teams);

    if (curr_action == BannerAction::MODIFY_WIDTH)
        width = "<...>";
    else if (curr_action == BannerAction::MODIFY_HEIGHT)
        height = "<...>";
    else if (curr_action == BannerAction::MODIFY_NB_TEAMS)
        nb_teams = "<...>";

    draw_text_center("Width: " + width, pos[0], setting_color_1);
    draw_text_center("Height: " + height, pos[1], setting_color_1);
    draw_text_center("Nb teams: " + nb_teams, pos[2], setting_color_1);
    draw_text_center("Save", pos[3], setting_color_2);
    draw_text_center("Save as...", pos[4], setting_color_2);
}

void MapEditor::draw_cell(CellCoords cell_coords, bool focus) const {
    Cell const& cell = map->grid[cell_coords.x][cell_coords.y];
    uint32_t color = (focus ? FOCUS_COLOR[cell.team] : TEAM_COLORS[cell.team]);

    if (!cell.exists) {
        AbstractDrawer::draw_cell(cell_coords, RADIUS, -1, color, false);
        return;
    }

    int radius = (cell.limit_12 ? RADIUS_12 : RADIUS);
    AbstractDrawer::draw_cell(cell_coords, radius, cell.value, color, cell.limit_12);
}
