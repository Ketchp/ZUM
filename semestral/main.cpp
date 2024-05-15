#include <iostream>

#include "minimax.h"
#include "games.h"


template<typename game>
void run_game() {
    game state{};

    std::cout << state;
    while(!state.is_final()) {

        auto r = minimax(state, state.turn == 0, 7);
        std::cout << "Utility: " << r.utility << std::endl << std::endl;

        if(!r.best_response.has_value())
            break;
        state = r.best_response.value();
        std::cout << state;
    }
}


int main() {
//    run_game<TicTacToe>();
    run_game<Checkers>();
}
