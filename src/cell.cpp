
#include "cell.hpp"


bool operator==(CellCoords const& a, CellCoords const& b) {
    return (a.x == b.x) && (a.y == b.y);
}

Cell::Cell() {
    reset();
}

Cell::Cell(bool links[6], bool limit_12, int team, int value) :
    limit_12(limit_12), team(team), value(value) {
    exists = true;
    for (int i=0; i<6; i++) {
        this->links[i] = links[i];
    }
}

void Cell::reset() {
    exists = false;
    for (int i=0; i<6; i++)
        links[i] = false;
    limit_12 = false;
    team = 0;
    value = 0;
}


uint16_t Cell::serialize() const {
    if (!exists)
        return 0;

    uint16_t data = 0;

    data |= value & 0x0f;
    data = (data << 4) | (team & 0x0f);
    data = (data << 1) | limit_12;

    for (int i=0; i<6; i++)
        data = (data << 1) | links[i];

    data = (data << 1) | 0x01;
    return data;
}

void Cell::unserialize(uint16_t data) {
    if (!(data & 0x01))
        return;

    exists = 1;
    data >>= 1;
    for (int i=0; i<6; i++) {
        links[5-i] = (bool) (data & 0x01);
        data >>= 1;
    }

    limit_12 = (bool) (data & 0x01);
    data >>= 1;
    team = data & 0x0f;
    data >>= 4;
    value = data & 0x0f;
}

void Cell::read_from_token(std::string token) {
    if (token.empty()) {
        reset();
        return;
    }

    exists = true;
    if (token.back() == '+') {
        limit_12 = true;
        token.pop_back();
    }

    size_t index_col = token.find(":");
    if (index_col != std::string::npos)
        team = std::stoi(token.substr(0, index_col));
    else
        index_col = -1;

    size_t index_slash = token.find("/");
    if (index_slash == std::string::npos)
        value = std::stoi(token.substr(index_col+1));
    else
        value = std::stoi(token.substr(index_col+1, index_slash-index_col-1));

    for (int i=0; i<6; i++)
        links[i] = true;

    if (index_slash != std::string::npos) {
        for (char c : token.substr(index_slash+1))
            links[c - '1'] = false;
    }
}

std::ostream& operator<<(std::ostream& os, Cell const& cell) {
    os << "(";
    for (int i=0; i<5; i++)
        os << cell.links[i] << ", ";
    os << cell.links[5] << "), ";
    os << "[" << cell.value << "/" << (cell.limit_12 ? "12" : " 8") << "]";
    os << "[T:" << cell.team << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, CellCoords const& coords) {
    os << "(" << coords.x << ", " << coords.y << ")";
    return os;
}
