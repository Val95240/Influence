
#include "arena.hpp"

#include "agents/random_agent.hpp"
#include "utils.hpp"


Arena::Arena(std::string const& path, std::vector<int> agent_levels)
    : Map(path), dead_agents(nb_teams, false)
{
    if (!agent_levels.empty() && agent_levels.size() != (size_t)nb_teams) {
        std::cerr << "Agent difficulties does not match number of teams" << std::endl;
        agent_levels.clear();
    }

    if (agent_levels.empty()) {
        for (int i=0; i<nb_teams; i++)
            agents.push_back(new RandomAgent(*this, i+2));
        return;
    }

    for (size_t i=0; i<agent_levels.size(); i++) {
        switch (agent_levels[i]) {
            case 1:  // Random agent
                agents.push_back(new RandomAgent(*this, i+2));
                break;

            default:  // Random agent
                std::cerr << "Agent (difficulty " << agent_levels[i] << ") not implemented !" << std::endl;
                agents.push_back(new RandomAgent(*this, i+2));
                break;
        }
    }
}

bool Arena::attack(CellCoords src_coords, CellCoords dst_coords) {
    Cell& src = grid[src_coords.x][src_coords.y];
    Cell& dst = grid[dst_coords.x][dst_coords.y];

    if (!src.exists || !dst.exists || src.value < 2 || src.team == dst.team)
        return false;

    if (utils::distance(src_coords, dst_coords) > 1)
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
        if (utils::random_chance(proba))
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
    while (nb_cells > 0) {
        while (cell_pos[value].empty())
            value++;

        if ((int)cell_pos[value].size() <= nb_cells) {
            for (CellCoords cell_coords : cell_pos[value]) {
                if (grid[cell_coords.x][cell_coords.y].value < (grid[cell_coords.x][cell_coords.y].limit_12 ? 12 : 8))
                    cell_pos[value+1].push_back(cell_coords);
                grow_cell(team, cell_coords);
                nb_cells--;
            }
        } else {
            utils::shuffle(cell_pos[value]);
            for (int i=0; i<nb_cells; i++)
                grow_cell(team, cell_pos[value][i]);
            return;
        }

        value++;
    }
}

bool Arena::play_agent_turn(int player_id) {
    if (dead_agents[player_id-2]) {
        return true;
    } else if (count_cells(player_id) == 0) {
        dead_agents[player_id-2] = true;
        return true;
    }

    if (player_id != last_player_id) {
        agents[player_id-2]->init_turn();
        attack_phase = true;
    }
    last_player_id = player_id;

    if (attack_phase) {
        auto [src, dst] = agents[player_id-2]->attack();
        attack(src, dst);
        attack_phase = agents[player_id-2]->end_attack();
        if (!attack_phase)
            nb_cells_to_grow = std::min(count_cells(player_id), get_growth_limit(player_id));

    } else {
        CellCoords cell_coords = agents[player_id-2]->grow(nb_cells_to_grow--);
        grow_cell(player_id, cell_coords);
        if (nb_cells_to_grow == 0 || agents[player_id-2]->end_growth())
            return true;
    }

    return false;
}
