#pragma once

#include <vector>
#include <iostream>
#include <array>
#include <map>
#include <set>

#include "state.h"


class Checkers: public State<Checkers> {
public:
    struct Pos: public std::pair<uint8_t, uint8_t> {
        using std::pair<uint8_t, uint8_t>::pair;

        Pos operator+(Pos other) const;
        Pos operator-() const;
        Pos operator-(Pos other) const;

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] std::vector<Pos> get_neighbors(T_turn turn) const;
        [[nodiscard]] Pos jump_over(Pos tile) const;
    };

    Checkers();
    [[nodiscard]] std::vector<Checkers> next_states() const;
    [[nodiscard]] T_util get_heuristic() const;

private:
    void set_utility();
    std::array<std::set<Pos>, 2> positions;  // white and black positions

    friend std::ostream &operator<<(std::ostream &os, const Checkers &v);
};


inline Checkers::Pos operator*(int mul, const Checkers::Pos &vec);
std::ostream &operator<<(std::ostream &os, const Checkers &v);
