

#include "main_window.h"

#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>


int const BANNER_HEIGHT = 100;


MainWindow::~MainWindow() {
    delete map_drawer;
}

bool MainWindow::init() {
    if (!AbstractWindow::init())
        return false;

    map_drawer = new MapDrawer(window, renderer, font, font_small, 100);
    return true;
}

void MainWindow::run(Map& map) {
    SDL_ShowWindow(window);
    map_drawer->set_map(map);

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

            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                click_callback(map, x, y);
                modified = true;
            }
        }

        if (modified) {
            modified = false;
            map_drawer->draw_map(focus_x, focus_y, attack_phase, nb_cells_to_grow);

            SDL_RenderPresent(renderer);
        }

        SDL_Delay(100);
    }
}


void MainWindow::click_callback(Map& map, int x, int y) {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    focus_x = -1;
    focus_y = -1;

    // Click on banner
    if (y > win_height - BANNER_HEIGHT) {
        if (attack_phase) {
            attack_phase = false;
            nb_cells_to_grow = map.count_cells(1);
            return;
        } else {
            attack_phase = true;
            std::cout << "End of turn" << std::endl;
            return;
        }
    }

    auto [cell_x, cell_y] = map_drawer->get_cell_at_coords(x, y);
    if (attack_phase) {
        // Click on a cell of player's color
        if (cell_x != -1 && map.grid[cell_x][cell_y].team == 1) {
            focus_x = cell_x;
            focus_y = cell_y;
        }

        // Attack if last click was on one of player's cells
        if (cell_x > -1 && last_click_x > -1 && map.grid[cell_x][cell_y].team != 1) {
            if (map.attack(last_click_x, last_click_y, cell_x, cell_y)) {
                focus_x = cell_x;
                focus_y = cell_y;
            }
        }

        last_click_x = focus_x;
        last_click_y = focus_y;

    } else {
        if (cell_x > 0) {
            if (map.grow_cell(cell_x, cell_y))
                nb_cells_to_grow--;

            if (nb_cells_to_grow == 0) {
                std::cout << "End of player turn" << std::endl;
                attack_phase = true;
            }
        }
    }
}
