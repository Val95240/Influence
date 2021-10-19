
#include "agents/abstract_agent.h"


AbstractAgent::AbstractAgent(Arena const& arena, int team) : arena(arena), team(team) { }

std::vector<CellCoords> AbstractAgent::find_cells() const {
    std::vector<CellCoords> agent_cells;
    for (int i=0; i<arena.height; i++) {
        for (int j=0; j<arena.width; j++) {
            if (arena.grid[i][j].exists && arena.grid[i][j].team == team)
                agent_cells.push_back({i, j});
        }
    }

    return agent_cells;
}

std::vector<CellCoords> AbstractAgent::find_border() const {
    std::vector<CellCoords> cells_coords = find_cells();
    std::vector<CellCoords> border_cells;
    for (CellCoords coords : cells_coords) {
        std::vector<CellCoords> open_neighs = find_open_neighs(coords);
        if (!open_neighs.empty())
            border_cells.push_back(coords);
    }

    return border_cells;
}

std::vector<CellCoords> AbstractAgent::find_open_neighs(CellCoords coords) const {
    std::vector<CellCoords> open_neighs;
    for (int dir=1; dir<7; dir++) {
        auto [neigh_x, neigh_y] = arena.get_neighbour_coords(coords, dir);
        if (neigh_x > -1 && arena.grid[neigh_x][neigh_y].team != team)
            open_neighs.push_back({neigh_x, neigh_y});
    }
    return open_neighs;
}

