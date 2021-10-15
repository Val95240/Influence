
#ifndef ARENA_H
#define ARENA_H

#include "map.h"


class Arena : public Map {
    public:
        using Map::Map;

        bool attack(CellCoords src, CellCoords dst);
        int get_growth_limit(int team) const;
        void grow_random_cells(int team, int nb_cells);
};


#endif // ARENA_H
