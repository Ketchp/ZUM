#include "tic_tac_toe.h"


TicTacToe::TicTacToe()
    : board{}
{
    board.fill({NO_PLAYER, NO_PLAYER, NO_PLAYER});
}

[[nodiscard]] std::vector<TicTacToe> TicTacToe::next_states() const {
    std::vector<TicTacToe> res;
    for(int x = 0; x < size; x++) {
        for(int y = 0; y < size; y++) {
            if(board[y][x] == NO_PLAYER)
                res.emplace_back(next_state(x, y));
        }
    }
    return res;
}

[[nodiscard]] T_util TicTacToe::get_heuristic() {
    return 0;
}

[[nodiscard]] TicTacToe TicTacToe::next_state(uint8_t x, uint8_t y) const {
    auto copy = *this;

    copy.board[y][x] = turn;
    copy.turn = !turn;

    auto winner = copy.board_winner();

    if(winner == 0)
        copy.utility = 1;
    else if(winner == 1)
        copy.utility = -1;
    else {
        for(const auto &r: copy.board)
            for(const auto &e: r)
                if(e == NO_PLAYER)
                    return copy;
        copy.utility = 0;
    }

    return copy;
}

[[nodiscard]] T_turn TicTacToe::board_winner() const {
    for(int row = 0; row < size; row++) {
        T_turn winner = board[row][0];
        for(auto e: board[row])
            if(e != winner)
                winner = NO_PLAYER;

        if(winner != NO_PLAYER)
            return winner;
    }

    for(int col = 0; col < size; col++) {
        T_turn winner = board[0][col];
        for(int row = 1; row < size; row++)
            if(board[row][col] != winner)
                winner = NO_PLAYER;

        if(winner != NO_PLAYER)
            return winner;
    }

    T_turn winner = board[0][0];
    for(int i = 1; i < size; i++) {
        if(board[i][i] != winner)
            winner = NO_PLAYER;
    }
    if(winner != NO_PLAYER)
        return winner;

    winner = board[0][2];
    for(int i = 1; i < size; i++) {
        if(board[i][2-i] != winner)
            winner = NO_PLAYER;
    }
    return winner;
}


std::ostream &operator<<(std::ostream &os, const TicTacToe &v) {
    os << "+---+\n";
    for(const auto &row: v.board) {
        os << '|';
        for(const auto &e: row) {
            if(e == 0)
                os << 'X';
            else if(e == 1)
                os << 'O';
            else
                os << ' ';
        }
        os << "|\n";
    }

    return os << "+---+\n";
}

