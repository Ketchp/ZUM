#pragma once

#include <cstdint>
#include <limits>
#include <optional>


using T_turn = uint8_t;
constexpr T_turn NO_PLAYER = -1;

template<T_turn N_PLAYERS = 2>
inline T_turn next_turn(T_turn current) {
    return (current + 1) % N_PLAYERS;
}

using T_util = double;
constexpr auto MAX_UTIL = std::numeric_limits<T_util>::infinity();
constexpr auto MIN_UTIL = -MAX_UTIL;


template<typename TS>
class State {
public:
    [[nodiscard]] T_util get_utility() const {
        if(utility.has_value())
            return utility.value();

        return static_cast<const TS &>(*this).get_heuristic();
    }

    [[nodiscard]] bool is_final() const {
        return utility.has_value();
    }

    T_turn turn{0};
protected:
    std::optional<T_util> utility;
};
