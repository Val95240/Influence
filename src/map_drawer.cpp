
#include "map_drawer.h"

#include <math.h>

#include <algorithm>

#include "cell.h"


void MapDrawer::set_map(Map const& map) {
    this->map = &map;
    reset_active_cells();
}

void MapDrawer::draw_map(int focus_x, int focus_y, bool attack_phase, int nb_cells_to_grow) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);

    mark_active_cells();

    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            bool focus = (i == focus_x && j == focus_y);
            draw_cell(i, j, focus);
        }
    }

    draw_links();

    std::string banner_text;
    if (attack_phase) {
        if (no_active_cell())
            banner_text = "Touch here to end attack";
        else if (focus_x == -1 || !active_cells[focus_x][focus_y])
            banner_text = "Touch a cell of your color\n(or touch here to end attack)";
        else
            banner_text = "Touch a nearby cell to attack\n(or touch here to end attack)";
    } else {
        std::string nb_cells = std::to_string(nb_cells_to_grow);
        banner_text = "Touch a cell to grow (" + nb_cells + ")\n(or touch here to end turn)";
    }

    draw_banner(banner_text);
}

bool MapDrawer::no_active_cell() const {
    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            if (!map->grid[i][j].exists || map->grid[i][j].team != 1 || map->grid[i][j].value == 1)
                continue;

            for (int dir=1; dir<7; dir++) {
                auto [neigh_x, neigh_y] = map->get_neighbor_pos(i, j, dir);
                if (neigh_x == -1)
                    continue;

                if (map->grid[neigh_x][neigh_y].exists && map->grid[neigh_x][neigh_y].team != 1)
                    return false;
            }
        }
    }
    return true;
}


void MapDrawer::reset_active_cells() {
    active_cells.clear();
    for (int i=0; i<map->height; i++) {
        std::vector<bool> row(map->width, false);
        active_cells.push_back(row);
    }
}

void MapDrawer::mark_active_cells() {
    reset_active_cells();

    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            if (!map->grid[i][j].exists || map->grid[i][j].team != 1 || map->grid[i][j].value == 1)
                continue;

            for (int dir=1; dir<7; dir++) {
                auto [neigh_x, neigh_y] = map->get_neighbor_pos(i, j, dir);
                if (neigh_x == -1)
                    continue;

                if (map->grid[neigh_x][neigh_y].exists && map->grid[neigh_x][neigh_y].team != 1) {
                    active_cells[i][j] = true;
                    break;
                }
            }
        }
    }
}

void MapDrawer::draw_banner(std::string banner_text) const {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);
    draw_banner_lines(win_width, win_height);

    std::string main_line, sub_line;

    size_t index = banner_text.find('\n');
    if (index == std::string::npos) {
        main_line = banner_text;
    } else {
        main_line = banner_text.substr(0, index);
        sub_line = banner_text.substr(index+1);
    }

    draw_text(main_line, win_height - 0.65*banner_height, banner_color);

    if (!sub_line.empty())
        draw_text(sub_line, win_height - 0.35*banner_height, banner_color_2, true);
}

void MapDrawer::draw_cell(int x, int y, bool focus) const {
    Cell const& cell = map->grid[x][y];
    if (!cell.exists)
        return;

    int radius = (cell.limit_12 ? RADIUS_12 : RADIUS);

    uint32_t color = TEAM_COLORS[cell.team];
    if (cell.team == 1) {
        // Inactive cells
        if (!active_cells[x][y])
            color = INACTIVE_COLOR;

        // Focused cell color
        else if (focus)
            color = FOCUS_COLOR[1];
    }

    AbstractDrawer::draw_cell(x, y, radius, cell.value, color, cell.limit_12);
}
