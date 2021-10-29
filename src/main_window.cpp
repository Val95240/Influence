

#include "main_window.hpp"

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
    phase = Phase::ATTACK;
    active_player = 1;
    return true;
}

void MainWindow::run(Arena& arena) {
    SDL_ShowWindow(window);
    map_drawer->set_map(arena);

    bool modified = true;
    bool quit = false;
    SDL_Event event;
    while (!quit) {

        // Enemy's turn
        if (active_player > 1) {
            if (arena.play_agent_turn(active_player))
                end_turn(arena);
            modified = true;
        }

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                quit = true;

            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q)
                    quit = true;

                if (active_player == 1 && event.key.keysym.sym == SDLK_SPACE) {
                    banner_action(arena);
                    modified = true;
                }
            }

            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                modified = true;

            else if (active_player == 1 && event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                click_callback(arena, x, y);
                modified = true;
            }
        }

        if (modified) {
            modified = false;
            map_drawer->draw_map(focus_coords, phase, nb_cells_to_grow);

            SDL_RenderPresent(renderer);
        }

        if (active_player == 1)
            SDL_Delay(100);
    }
}

void MainWindow::end_turn(Arena const& arena) {
    if (active_player == 1)
        std::cout << "End of turn" << std::endl;
    active_player += 1;
    phase = Phase::ENEMY;

    if (active_player > arena.nb_teams) {
        phase = Phase::ATTACK;
        active_player = 1;
    }
}

void MainWindow::banner_action(Arena& arena) {
    if (phase == Phase::ATTACK) {
        phase = Phase::GROWTH;
        nb_cells_to_grow = std::min(arena.count_cells(1), arena.get_growth_limit(1));

        if (nb_cells_to_grow == 0)
            end_turn(arena);

    } else {
        arena.grow_random_cells(1, nb_cells_to_grow);
        end_turn(arena);
    }
}

void MainWindow::click_callback(Arena& arena, int x, int y) {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    focus_coords = {-1, -1};

    // Click on banner
    if (y > win_height - BANNER_HEIGHT) {
        banner_action(arena);
        return;
    }

    CellCoords cell_coords = map_drawer->get_cell_at_coords(x, y);
    if (phase == Phase::ATTACK) {
        // Click on a cell of player's color
        if (cell_coords.x != -1 && arena.grid[cell_coords.x][cell_coords.y].team == 1)
            focus_coords = cell_coords;

        // Attack if last click was on one of player's cells
        if (cell_coords.x > -1 && last_clicked.x > -1 && arena.grid[cell_coords.x][cell_coords.y].team != 1) {
            if (arena.attack(last_clicked, cell_coords))
                focus_coords = cell_coords;
        }

        last_clicked = focus_coords;

    } else {  // Player's growth phase
        if (cell_coords.x == -1)
            return;

        if (arena.grow_cell(1, cell_coords))
            nb_cells_to_grow--;

        if (nb_cells_to_grow == 0)
            end_turn(arena);
    }
}
