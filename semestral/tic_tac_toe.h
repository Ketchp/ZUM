#pragma once

#include <vector>
#include <iostream>
#include <array>

#include "state.h"


class TicTacToe: public State<TicTacToe> {
public:
    TicTacToe();
    [[nodiscard]] std::vector<TicTacToe> next_states() const;
    [[nodiscard]] static T_util get_heuristic();

private:
    constexpr static int size = 3;

    [[nodiscard]] TicTacToe next_state(uint8_t x, uint8_t y) const;
    [[nodiscard]] T_turn board_winner() const;
    std::array<std::array<T_turn, size>, size> board;

    friend std::ostream &operator<<(std::ostream &os, const TicTacToe &v);
};


std::ostream &operator<<(std::ostream &os, const TicTacToe &v);
