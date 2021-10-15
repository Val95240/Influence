
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "abstract_window.h"
#include "cell.h"
#include "map.h"
#include "map_drawer.h"


class MainWindow : public AbstractWindow {
    public:
        ~MainWindow();

        bool init() override;
        void run(Map& map) override;

    private:
        MapDrawer* map_drawer = NULL;

        int last_click_x = -1;
        int last_click_y = -1;
        CellCoords last_clicked = {-1, -1};
        CellCoords focus_coords;
        bool attack_phase = true;
        int nb_cells_to_grow = -1;

        void banner_action(Map& map);
        void click_callback(Map& map, int x, int y);
};


#endif // MAIN_WINDOW_H
