
#include "editor_window.h"

#include <stdio.h>

#include "cell.h"


int const BANNER_HEIGHT = 150;


EditorWindow::EditorWindow(std::string map_path) : map_path(map_path) { }

EditorWindow::~EditorWindow() {
    delete map_editor;
}

bool EditorWindow::init() {
    if (!AbstractWindow::init())
        return false;

    map_editor = new MapEditor(window, renderer, font, font_small, BANNER_HEIGHT);
    return true;
}

void EditorWindow::run(Map& map) {
    SDL_ShowWindow(window);
    map_editor->set_map(map);
    SDL_StopTextInput();

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

                else if (event.key.keysym.sym == SDLK_s && SDL_GetModState() & KMOD_CTRL)
                    save_map(map);

                else if (event.key.keysym.sym == SDLK_RETURN && map_editor->curr_action != BannerAction::NOOP) {
                    modify_value(map);
                    stop_input();
                    modified = true;
                }

                if (focus_x != -1) {
                    edit_key_callback(map, event.key.keysym.sym);
                    modified = true;
                }
            }

            else if (event.type == SDL_TEXTINPUT) {
                char c = event.text.text[0];
                if ('0' <= c && c <= '9')
                    input_value = (10 * input_value) + (c - '0');
                modified = true;
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
            map_editor->draw_map(focus_x, focus_y);

            SDL_RenderPresent(renderer);
        }

        SDL_Delay(100);
    }
}


void EditorWindow::start_input(BannerAction action) {
    map_editor->curr_action = action;
    SDL_StartTextInput();
}

void EditorWindow::stop_input() {
    map_editor->curr_action = BannerAction::NOOP;
    input_value = 0;
    SDL_StopTextInput();
}

void EditorWindow::modify_value(Map& map) {
    if (input_value == 0)
        return;

    if (map_editor->curr_action == BannerAction::MODIFY_WIDTH)
        map.resize(map.height, input_value);

    else if (map_editor->curr_action == BannerAction::MODIFY_HEIGHT)
        map.resize(input_value, map.width);

    else if (map_editor->curr_action == BannerAction::MODIFY_NB_TEAMS)
        map.modify_nb_teams(input_value);
}

void EditorWindow::edit_key_callback(Map& map, SDL_Keycode code) {
    bool shift = (SDL_GetModState() & KMOD_SHIFT);
    switch (code) {
        case SDLK_w:
            start_input(BannerAction::MODIFY_WIDTH);
            break;

        case SDLK_e:
            start_input(BannerAction::MODIFY_HEIGHT);
            break;

        case SDLK_n:
            start_input(BannerAction::MODIFY_NB_TEAMS);
            break;

        case SDLK_h:
            if (focus_y > 0)
                focus_y--;
            break;

        case SDLK_j:
            if (focus_x < map.height-1)
                focus_x++;
            break;

        case SDLK_k:
            if (focus_x > 0)
                focus_x--;
            break;

        case SDLK_l:
            if (focus_y < map.width-1)
                focus_y++;
            break;

        case SDLK_t:
            map.switch_team(focus_x, focus_y, shift);
            break;

        case SDLK_u:
            map.change_value(focus_x, focus_y, shift);
            break;

        case SDLK_0:
            map.toggle_links(focus_x, focus_y);
            break;

        case SDLK_1:
            map.toggle_link(focus_x, focus_y, 1);
            break;

        case SDLK_2:
            map.toggle_link(focus_x, focus_y, 2);
            break;

        case SDLK_3:
            map.toggle_link(focus_x, focus_y, 3);
            break;

        case SDLK_4:
            map.toggle_link(focus_x, focus_y, 4);
            break;

        case SDLK_5:
            map.toggle_link(focus_x, focus_y, 5);
            break;

        case SDLK_6:
            map.toggle_link(focus_x, focus_y, 6);
            break;

        default:
            break;
    }

    if (focus_x % 2 == 1 && focus_y == map.width-1)
        focus_y--;
}

void EditorWindow::click_callback(Map& map, int x, int y) {
    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    focus_x = -1;
    focus_y = -1;

    if (map_editor->curr_action != BannerAction::NOOP) {
        stop_input();
        return;
    }

    // Click on banner
    if (y > win_height - BANNER_HEIGHT) {
        BannerAction action = map_editor->click_banner(x, y);
        switch (action) {
            case BannerAction::MODIFY_WIDTH:
            case BannerAction::MODIFY_HEIGHT:
            case BannerAction::MODIFY_NB_TEAMS:
                start_input(action);
                break;

            case BannerAction::SAVE:
                save_map(map);
                break;

            case BannerAction::SAVE_AS:
                save_map_as(map);
                break;

            case BannerAction::NOOP:
                break;
        }
    }

    auto [cell_x, cell_y] = map_editor->get_cell_at_coords(x, y);
    if (cell_x != -1) {
        focus_x = cell_x;
        focus_y = cell_y;
    }
}

void EditorWindow::save_map(Map const& map) {
    if (map_path.empty()) {
        save_map_as(map);
        return;
    }

    map.save(map_path);
    std::cout << "Map saved in " << map_path << std::endl;
}

void EditorWindow::save_map_as(Map const& map) {
    char filename[128] = {0};
    FILE* file = popen("zenity --file-selection --save --confirm-overwrite --file-filter *.map | tr -d '\n'", "r");
    fgets(filename, 128, file);

    std::string filename_s = filename;
    if (filename_s.empty())
        return;

    map_path = filename_s;
    save_map(map);
}
