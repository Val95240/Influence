
#include "arena.h"

#include <math.h>
#include <stdlib.h>

#include <algorithm>


static bool random_chance(double limit) {
    double x = rand() / (RAND_MAX + 1.0);
    return x < limit;
}

static int distance(CellCoords src, CellCoords dst) {
    int dx = std::abs(dst.x - src.x);
    int dy = std::abs(dst.y - src.y);
    bool penalty = (src.x % 2 == 0 && dst.x % 2 == 1 && (src.y < dst.y)) || (dst.x % 2 == 0 && src.x % 2 == 1 && (dst.y < src.y));

    return std::max(dx, dy + dx / 2 + penalty);
}


bool Arena::attack(CellCoords src_coords, CellCoords dst_coords) {
    Cell& src = grid[src_coords.x][src_coords.y];
    Cell& dst = grid[dst_coords.x][dst_coords.y];

    if (!src.exists || !dst.exists || src.value < 2 || src.team == dst.team)
        return false;

    if (distance(src_coords, dst_coords) > 1)
        return false;

    if (src.value < dst.value - 1) {
        dst.value -= src.value;
        src.value = 1;

    } else if (src.value > dst.value + 1) {
        dst.value = src.value - std::max(dst.value, 1);
        src.value = 1;
        dst.team = src.team;

        if (!dst.limit_12 && dst.value > 8) {
            src.value += dst.value - 9;
            dst.value = 8;
        }

    } else {
        double proba = 0.50 + 0.25 * (src.value - dst.value);
        src.value = 1;
        dst.value = 1;
        if (random_chance(proba))
            dst.team = src.team;
    }

    return true;
}

int Arena::get_growth_limit(int team) const {
    int limit = 0;
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (grid[i][j].exists && grid[i][j].team == team)
                limit += (grid[i][j].limit_12 ? 12 : 8) - grid[i][j].value;
        }
    }
    return limit;
}

void Arena::grow_random_cells(int team, int nb_cells) {
    std::vector<CellCoords> cell_pos[12];
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            if (grid[i][j].exists && grid[i][j].team == team && grid[i][j].value < (grid[i][j].limit_12 ? 12 : 8))
                cell_pos[grid[i][j].value-1].push_back({i, j});
        }
    }

    int value = 0;
    while (nb_cells> 0) {
        while (cell_pos[value].empty())
            value++;

        if ((int)cell_pos[value].size() <= nb_cells) {
            for (CellCoords cell_coords : cell_pos[value]) {
                if (grid[cell_coords.x][cell_coords.y].value < (grid[cell_coords.x][cell_coords.y].limit_12 ? 12 : 8))
                    cell_pos[value+1].push_back(cell_coords);
                grow_cell(cell_coords);
                nb_cells--;
            }
        } else {
            std::shuffle(cell_pos[value].begin(), cell_pos[value].end(), rng);
            for (CellCoords cell_coords : cell_pos[value]) {
                grow_cell(cell_coords);
                nb_cells--;
                if (nb_cells == 0)
                    return;
            }
        }
        value++;
    }
}

