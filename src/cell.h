
#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <string>


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
    std::string to_token() const;
};

std::ostream& operator<<(std::ostream& os, Cell const& cell);


#endif // CELL_H
