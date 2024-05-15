#include "micro_chess.h"


MicroChess::MicroChess()
    : board{}
{
    board[0] = {B_K, B_H, B_B, B_R};
    board[1] = {B_P, EMPTY_TILE, EMPTY_TILE, EMPTY_TILE};
    board[2] = {EMPTY_TILE, EMPTY_TILE, EMPTY_TILE, EMPTY_TILE};
    board[3] = {EMPTY_TILE, EMPTY_TILE, EMPTY_TILE, W_P};
    board[4] = {W_R, W_B, W_H, W_K};
}


[[nodiscard]] std::vector<MicroChess> MicroChess::next_states() const {
    Piece::Color player = turn == 0 ? Piece::Color::WHITE : Piece::Color::BLACK;

    auto king_position = get_king_position(player);

    return {};
}

Pos MicroChess::get_king_position(Piece::Color color) const {
    for(uint8_t x = 0; x < WIDTH; x++) {
        for(uint8_t y = 0; y < HEIGHT; y++) {
            if(board[y][x] == Piece{Piece::Type::KING, color})
                return {x, y};
        }
    }
    throw std::exception{};
}


