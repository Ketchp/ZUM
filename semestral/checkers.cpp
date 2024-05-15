#include "checkers.h"



constexpr static Checkers::Pos INVALID{-1, -1};

Checkers::Pos Checkers::Pos::operator+(Pos other) const {
    auto copy = *this;
    copy.first += other.first;
    copy.second += other.second;
    return copy;
}

Checkers::Pos Checkers::Pos::operator-() const {
    return -1 * *this;
}

Checkers::Pos Checkers::Pos::operator-(Pos other) const {
    return *this + (-other);
}

[[nodiscard]] bool Checkers::Pos::is_valid() const {
    return first < 8 && second < 8;
}

[[nodiscard]] std::vector<Checkers::Pos> Checkers::Pos::get_neighbors(T_turn turn) const {
    std::vector<Pos> res;

    auto &&li = turn == 0 ? std::array<Pos, 2>{Pos{-1, 1}, {1, 1}} :
                                            std::array<Pos, 2>{Pos{-1, -1}, {1, -1}};

    for(auto off: li) {
        auto n = *this + off;
        if(n.is_valid())
            res.emplace_back(n);
    }

    return res;
}

[[nodiscard]] Checkers::Pos Checkers::Pos::jump_over(Pos tile) const {
    Pos off = tile - *this;
    Pos n = *this + 2 * off;

    if(n.is_valid())
        return n;
    return INVALID;
}


inline Checkers::Pos operator*(int mul, const Checkers::Pos &vec) {
    return {mul * vec.first, mul * vec.second};
}



Checkers::Checkers()
    : positions{}
{
    for(auto w: {Pos{0, 0}, {2, 0}, {4, 0}, {6, 0},
                          {1, 1}, {3, 1}, {5, 1}, {7, 1}})
        positions[0].emplace(w);

    for(auto b: {Pos{0, 6}, {2, 6}, {4, 6}, {6, 6},
                          {1, 7}, {3, 7}, {5, 7}, {7, 7}})
        positions[1].emplace(b);
}

[[nodiscard]] std::vector<Checkers> Checkers::next_states() const {
    std::vector<Checkers> res;
    auto turn_n = next_turn(turn);
    for(auto p: positions[turn]) {
        for(auto n: p.get_neighbors(turn)) {
            auto copy = *this;
            copy.turn = turn_n;

            if(positions[turn].contains(n))
                continue;

            if(positions[turn_n].contains(n)) {
                auto temp = p.jump_over(n);
                if(temp == INVALID ||
                   positions[turn].contains(temp) ||
                   positions[turn_n].contains(temp))
                    continue;
                copy.positions[turn_n].erase(n);
                n = temp;
            }

            copy.positions[turn].erase(p);
            copy.positions[turn].emplace(n);

            copy.set_utility();
            res.emplace_back(copy);
        }
    }

    return res;
}

void Checkers::set_utility() {
    for(auto [_, y]: positions[0])
        if(y < 4)
            return;
    for(auto [_, y]: positions[1])
        if(y >= 4)
            return;
    utility = get_heuristic();
}

[[nodiscard]] T_util Checkers::get_heuristic() const {
    auto s0 = static_cast<double>(positions.at(0).size());
    auto s1 = static_cast<double>(positions.at(1).size());
    if(s0 > s1)
        return (s0 + 1) / (s1 + 1);
    if(s1 > s0)
        return -(s1 + 1) / (s0 + 1);
    return 0;
}

std::ostream &operator<<(std::ostream &os, const Checkers &v) {
    std::array<std::array<T_turn, 8>, 8> board{};
    for(auto &r: board)
        r.fill(NO_PLAYER);

    for(int x = 0; x < 8; x++)
        for(int y = ~x%2; y < 8; y+=2)
            board[y][x] = 2;

    for(auto p: {0, 1})
        for(auto [x, y]: v.positions.at(p))
            board[y][x] = p;

    os << "X--------X\n";
    for(auto row: board) {
        os << '|';
        for (auto e: row) {
            if(e == 0)
                os << 'X';
            else if(e == 1)
                os << 'O';
            else if(e == 2)
                os << '#';
            else
                os << ' ';
        }
        os << "|\n";
    }

    return os << "O--------O\n";
}

