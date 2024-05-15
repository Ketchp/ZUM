#include <iostream>

#include "minimax.h"
#include "games.h"


//int main() {
//    TicTacToe state{};
//
//    std::cout << state;
//    while(!state.is_final()) {
//
//        auto r = minimax(state, state.turn == 0);
//        std::cout << "Utility: " << r.utility << std::endl << std::endl;
//
//        if(!r.best_response.has_value())
//            break;
//        state = r.best_response.value();
//        std::cout << state;
//    }
//}

int main() {
    Checkers state{};

    std::cout << state;
    while(!state.is_final()) {

        auto r = minimax(state, state.turn == 0, 12);
        std::cout << "Utility: " << r.utility << std::endl << std::endl;

        if(!r.best_response.has_value())
            break;
        state = r.best_response.value();
        std::cout << state;
    }
}
