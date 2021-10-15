

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

                if (event.key.keysym.sym == SDLK_SPACE) {
                    banner_action(map);
                    modified = true;
                }
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
            map_drawer->draw_map(focus_coords, attack_phase, nb_cells_to_grow);

            SDL_RenderPresent(renderer);
        }

        SDL_Delay(100);
    }
}

void MainWindow::banner_action(Map& map) {
    if (attack_phase) {
        attack_phase = false;
        nb_cells_to_grow = map.count_cells(1);
        int growth_limit = map.get_growth_limit(1);
        if (nb_cells_to_grow > growth_limit)
            nb_cells_to_grow = growth_limit;

        if (nb_cells_to_grow == 0) {
            attack_phase = true;
            std::cout << "End of turn" << std::endl;
        }

    } else {
        attack_phase = true;
        map.grow_random_cells(1, nb_cells_to_grow);
        std::cout << "End of turn" << std::endl;
    }
}

void MainWindow::click_callback(Map& map, int x, int y) {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    focus_coords = {-1, -1};

    // Click on banner
    if (y > win_height - BANNER_HEIGHT) {
        banner_action(map);
        return;
    }

    CellCoords cell_coords = map_drawer->get_cell_at_coords(x, y);
    if (attack_phase) {
        // Click on a cell of player's color
        if (cell_coords.x != -1 && map.grid[cell_coords.x][cell_coords.y].team == 1)
            focus_coords = cell_coords;

        // Attack if last click was on one of player's cells
        if (cell_coords.x > -1 && last_clicked.x > -1 && map.grid[cell_coords.x][cell_coords.y].team != 1 && map.attack(last_clicked, cell_coords))
            focus_coords = cell_coords;

        last_clicked = focus_coords;

    } else {
        if (cell_coords.x > 0) {
            if (map.grow_cell(cell_coords))
                nb_cells_to_grow--;

            if (nb_cells_to_grow == 0) {
                std::cout << "End of player turn" << std::endl;
                attack_phase = true;
            }
        }
    }
}
