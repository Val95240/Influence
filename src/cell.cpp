
#include "cell.h"

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

    std::cout << "Data: " << data << std::endl;

    data >>= 1;
    for (int i=0; i<6; i++) {
        links[i] = (bool) (data & 0x01);
        data >>= 1;
        std::cout << "  links[i]: " << links[i] << std::endl;
        std::cout << "  data: " << data << std::endl;
    }

    limit_12 = (bool) (data & 0x01);
    data >>= 1;
    std::cout << "  limit_12: " << limit_12 << std::endl;
    std::cout << "  data: " << data << std::endl;
    team = data & 0x0f;
    data >>= 4;
    std::cout << "  team: " << team << std::endl;
    std::cout << "  data: " << data << std::endl;
    value = data & 0x0f;
    std::cout << "  value: " << value << std::endl;
    std::cout << "  data: " << data << std::endl;
    std::cout << std::endl << std::endl;
}

void Cell::read_from_token(std::string const& token) {
    if (token.empty()) {
        reset();
        return;
    }

    exists = true;
    size_t index_col = token.find(":");
    if (index_col != std::string::npos)
        team = std::stoi(token.substr(0, index_col));
    else
        index_col = -1;

    size_t index_slash = token.find("/");
    value = std::stoi(token.substr(index_col+1, index_slash-index_col-1));

    for (char c : token.substr(index_slash+1)) {
        if (c == '+') {
            limit_12 = true;
            continue;
        }
        links[c - '1'] = true;
    }
}

std::ostream& operator<<(std::ostream& os, Cell const& cell) {
    os << "(";
    for (int i=0; i<5; i++)
        os << cell.links[i] << ", ";
    os << cell.links[5] << "), ";
    os << "[" << cell.value << "/" << (cell.limit_12 ? "12" : "8") << "]";
    os << "[T:" << cell.team << "]";
    return os;
}
