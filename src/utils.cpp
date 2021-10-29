
#include "utils.hpp"

#include <cmath>
#include <cstdlib>


using namespace utils;


static std::random_device rd;
std::default_random_engine utils::rng(rd());

static std::uniform_real_distribution<double> double_unif(0, 1);

bool utils::random_chance(double limit) {
    double x = double_unif(rng);
    return x < limit;
}

int utils::randrange(int limit) {
    std::uniform_int_distribution<int> uniform(0, limit-1);
    return uniform(rng);
}

int utils::distance(CellCoords src, CellCoords dst) {
    int dx = std::abs(dst.x - src.x);
    int dy = std::abs(dst.y - src.y);
    bool penalty = (src.x % 2 == 0 && dst.x % 2 == 1 && (src.y < dst.y)) || (dst.x % 2 == 0 && src.x % 2 == 1 && (dst.y < src.y));

    return std::max(dx, dy + dx / 2 + penalty);
}
