
#include "map_drawer.h"

#include <algorithm>
#include <cmath>

#include "cell.h"


void MapDrawer::set_map(Map const& map) {
    AbstractDrawer::set_map(map);
    reset_active_cells();
}

void MapDrawer::draw_map(CellCoords focus_coords, int phase, int nb_cells_to_grow) {
    mark_active_cells();

    std::string banner_text;
    if (phase == 0) {  // Attack phase
        if (no_active_cell())
            banner_text = "Touch here to end attack";
        else if (focus_coords.x == -1 || !active_cells[focus_coords.x][focus_coords.y])
            banner_text = "Touch a cell of your color\n(or touch here to end attack)";
        else
            banner_text = "Touch a nearby cell to attack\n(or touch here to end attack)";
    } else if (phase == 1) {  // Growth phase
        std::string nb_cells = std::to_string(nb_cells_to_grow);
        banner_text = "Touch a cell to grow (" + nb_cells + ")\n(or touch here to end turn)";
    } else {  // Enemy phase
        banner_text = "Wait for your turn...";
    }

    AbstractDrawer::draw_map(focus_coords, banner_text);
}

bool MapDrawer::no_active_cell() const {
    for (int i=0; i<map->height; i++) {
        for (int j=0; j<map->width; j++) {
            if (!map->grid[i][j].exists || map->grid[i][j].team != 1 || map->grid[i][j].value == 1)
                continue;

            for (int dir=1; dir<7; dir++) {
                CellCoords neigh_coords = map->get_neighbour_coords({i, j}, dir);
                if (neigh_coords.x == -1)
                    continue;

                if (map->grid[neigh_coords.x][neigh_coords.y].exists && map->grid[neigh_coords.x][neigh_coords.y].team != 1)
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
                CellCoords neigh_coords = map->get_neighbour_coords({i, j}, dir);
                if (neigh_coords.x == -1)
                    continue;

                if (map->grid[neigh_coords.x][neigh_coords.y].exists && map->grid[neigh_coords.x][neigh_coords.y].team != 1) {
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

void MapDrawer::draw_cell(CellCoords cell_coords, bool focus) const {
    Cell const& cell = map->grid[cell_coords.x][cell_coords.y];

    uint32_t color = TEAM_COLORS[cell.team];
    if (cell.team == 1) {
        // Inactive cells
        if (!active_cells[cell_coords.x][cell_coords.y])
            color = INACTIVE_COLOR;

        // Focused cell color
        else if (focus)
            color = FOCUS_COLOR[1];
    }

    AbstractDrawer::draw_cell(cell_coords, color);
}
