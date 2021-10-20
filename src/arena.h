
#ifndef ARENA_H
#define ARENA_H

#include <string>
#include <vector>

#include "map.h"


class AbstractAgent;

class Arena : public Map {
    public:
        Arena(std::string const& path, std::vector<int> agent_levels={});

        bool attack(CellCoords src, CellCoords dst);
        int get_growth_limit(int team) const;
        void grow_random_cells(int team, int nb_cells);

        bool play_agent_turn(int player_id);

    private:
        std::vector<AbstractAgent*> agents;
        std::vector<bool> dead_agents;
        int last_player_id = 1;
        bool attack_phase = true;
        int nb_cells_to_grow;
};


#endif // ARENA_H
