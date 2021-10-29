
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "abstract_window.hpp"
#include "arena.hpp"
#include "cell.hpp"
#include "map_drawer.hpp"


enum Phase { ATTACK, GROWTH, ENEMY };

class MainWindow : public AbstractWindow {
    public:
        ~MainWindow();

        bool init() override;
        void run(Arena& arena);

    private:
        MapDrawer* map_drawer = NULL;

        CellCoords last_clicked = {-1, -1};
        CellCoords focus_coords = {-1, -1};

        Phase phase;
        int active_player;
        int nb_cells_to_grow = -1;

        void end_turn(Arena const& arena);
        void banner_action(Arena& arena);
        void click_callback(Arena& arena, int x, int y);
};


#endif // MAIN_WINDOW_H
