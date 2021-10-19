
#ifndef ABSTRACT_AGENT_H
#define ABSTRACT_AGENT_H

#include "arena.h"
#include "cell.h"


class AbstractAgent {
    public:
        AbstractAgent(Arena const& arena, int team);

        virtual void init_turn() {};
        virtual std::pair<CellCoords, CellCoords> attack() = 0;
        virtual bool end_attack() const = 0;
        virtual CellCoords grow(int nb_cells_to_grow) = 0;
        virtual bool end_growth() const = 0;

    protected:
        Arena const& arena;
        int team;

        std::vector<CellCoords> find_cells() const;
        std::vector<CellCoords> find_border() const;
        std::vector<CellCoords> find_open_neighs(CellCoords coords) const;
};


#endif // ABSTRACT_AGENT_H
