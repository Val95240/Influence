
#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <random>

#include "cell.h"


namespace utils {

    extern std::default_random_engine rng;

    int distance(CellCoords src, CellCoords dst);
    bool random_chance(double limit);
    int randrange(int limit);

    template<typename T>
        void shuffle(std::vector<T>& my_vector) {
            std::shuffle(my_vector.begin(), my_vector.end(), rng);
        }

    template<typename T>
    T choice(std::vector<T> const& my_vector) {
        size_t index = utils::randrange(my_vector.size());
        return my_vector[index];
    }
}

#endif // UTILS_H

