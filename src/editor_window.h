
#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <string>

#include "abstract_window.h"
#include "cell.h"
#include "map.h"
#include "map_editor.h"


class EditorWindow : public AbstractWindow {
    public:
        EditorWindow(std::string map_path);
        ~EditorWindow();

        bool init() override;
        void run(Map& map);

    private:
        MapEditor* map_editor;
        std::string map_path;
        BannerAction curr_action = BannerAction::NOOP;
        int input_value = 0;

        CellCoords focus_coords = {0, 0};

        void start_input(BannerAction action);
        void stop_input();
        void modify_value(Map& map);

        void edit_key_callback(Map& map, SDL_Keycode code);
        void banner_action(Map const& map, BannerAction action);
        void click_callback(Map const& map, int x, int y);
        void save_map(Map const& map);
        void save_map_as(Map const& map);
};


#endif // EDITOR_WINDOW_H
