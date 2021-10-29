
#ifndef RANDOM_AGENT_H
#define RANDOM_AGENT_H

#include <vector>

#include "arena.hpp"
#include "cell.hpp"
#include "agents/abstract_agent.hpp"


class RandomAgent : public AbstractAgent {
    public:
        RandomAgent(Arena const& arena, int team, double attack_proba=1.0);

        void init_turn() override;
        std::pair<CellCoords, CellCoords> attack();
        bool end_attack() const;
        CellCoords grow(int nb_cells_to_grow);
        bool end_growth() const;

    private:
        double attack_proba;

        std::vector<CellCoords> passive_cells;
        bool is_passive(CellCoords coords) const;
};


#endif // RANDOM_AGENT_H
