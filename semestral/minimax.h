#pragma once

#include <limits>
#include <optional>
#include <cstdint>
#include <functional>

#include "state.h"


constexpr auto ALPHA_INIT = MIN_UTIL;
constexpr auto BETA_INIT = MAX_UTIL;


template<typename State_T>
struct best_response {
    std::optional<const State_T> best_response;
    T_util utility;
};


template<typename State_T>
best_response<State_T> minimax(
        const State_T &current_state,
        bool maximizing = true,
        uint8_t max_depth = 10,
        T_util alpha = ALPHA_INIT,
        T_util beta = BETA_INIT
) {
    if(max_depth == 0 || current_state.is_final())
        return {
            current_state,
            current_state.get_utility()
        };
    max_depth--;

    T_util best_util = MAX_UTIL;
    std::optional<State_T> best_response;
    std::function func_comp = [](const T_util &a, const T_util &b){ return a > b; };
    auto *s_metric = &beta;

    if(maximizing) {
        best_util = MIN_UTIL;
        func_comp = [](const T_util &a, const T_util &b){ return a < b; };
        s_metric = &alpha;
    }

    for(const auto &next: current_state.next_states()) {
        auto res = minimax(next, !maximizing, max_depth, alpha, beta);
        if(func_comp(best_util, res.utility)) {
            best_util = res.utility;
            best_response = next;
        }

        if(func_comp(*s_metric, best_util))
            *s_metric = best_util;

        if(beta <= alpha)
            break;
    }

    if(best_response.has_value())
        best_util = current_state.get_utility();

    return {
        best_response,
        best_util
    };
}
