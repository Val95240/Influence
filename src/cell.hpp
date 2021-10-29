
#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <string>


struct CellCoords {
    int x;
    int y;
};

bool operator==(CellCoords const& a, CellCoords const& b);

struct Cell {
    bool exists;
    bool links[6];
    bool limit_12;
    int team;
    int value;

    Cell();
    Cell(bool links[6], bool limit_12, int team, int value);

    void reset();

    uint16_t serialize() const;
    void unserialize(uint16_t data);

    void read_from_token(std::string token);
};

std::ostream& operator<<(std::ostream& os, Cell const& cell);
std::ostream& operator<<(std::ostream& os, CellCoords const& coords);


#endif // CELL_H
