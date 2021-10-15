
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "abstract_window.h"
#include "arena.h"
#include "cell.h"
#include "map_drawer.h"


class MainWindow : public AbstractWindow {
    public:
        ~MainWindow();

        bool init() override;
        void run(Arena& arena);

    private:
        MapDrawer* map_drawer = NULL;

        CellCoords last_clicked = {-1, -1};
        CellCoords focus_coords = {-1, -1};
        bool attack_phase = true;
        int nb_cells_to_grow = -1;

        void banner_action(Arena& arena);
        void click_callback(Arena& arena, int x, int y);
};


#endif // MAIN_WINDOW_H
