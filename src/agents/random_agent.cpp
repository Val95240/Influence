
#include "agents/random_agent.hpp"

#include <algorithm>

#include "utils.hpp"


RandomAgent::RandomAgent(Arena const& arena, int team, double attack_proba)
    : AbstractAgent(arena, team), attack_proba(attack_proba)
{
}

void RandomAgent::init_turn() {
    passive_cells.clear();
    for (CellCoords coords : find_border()) {
        if (utils::random_chance(1.0 - attack_proba))
            passive_cells.push_back(coords);
    }
}

std::pair<CellCoords, CellCoords> RandomAgent::attack() {
    std::vector<CellCoords> border_cells = find_border();
    border_cells.erase(std::remove_if(border_cells.begin(),
                                      border_cells.end(),
                                      [this](CellCoords const& coord) { return this->is_passive(coord); }),
                       border_cells.end());
    CellCoords attacker = utils::choice(border_cells);

    std::vector<CellCoords> open_neighs = find_open_neighs(attacker);
    CellCoords enemy = utils::choice(open_neighs);
    return {attacker, enemy};
}

bool RandomAgent::end_attack() const {
    std::vector<CellCoords> border_cells = find_border();
    border_cells.erase(std::remove_if(border_cells.begin(),
                                      border_cells.end(),
                                      [this](CellCoords const& coord) { return this->is_passive(coord); }),
                       border_cells.end());
    return border_cells.empty();
}

CellCoords RandomAgent::grow(int nb_cells_to_grow) {
    return utils::choice(find_cells());
}

bool RandomAgent::end_growth() const {
    return false;
}

bool RandomAgent::is_passive(CellCoords coords) const {
    return std::find(passive_cells.begin(), passive_cells.end(), coords) != passive_cells.end();
}
