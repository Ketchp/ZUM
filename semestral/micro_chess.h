#pragma once

#include <vector>
#include <iostream>
#include <array>

#include "state.h"


struct Pos: public std::pair<uint8_t, uint8_t> {
    using std::pair<uint8_t, uint8_t>::pair;

    Pos operator+(Pos other) const {
        auto copy = *this;
        copy.first += other.first;
        copy.second += other.second;
        return copy;
    }
};



class MicroChess: public State<MicroChess> {
public:
    MicroChess();
    [[nodiscard]] std::vector<MicroChess> next_states() const;
    [[nodiscard]] static T_util get_heuristic();

private:
    struct Piece {
        enum class Type {
            EMPTY,
            KING,
            QUEEN,
            ROOK,
            HORSE,
            BISHOP,
            PAWN
        } type;

        enum class Color {
            EMPTY,
            WHITE,
            BLACK
        } color;

        bool operator==(const auto &other) const {
            return type == other.type && color == other.color;
        }

        constexpr static std::array<Pos, 8> KING_OFFSETS{Pos
               {-1, -1}, {-1, 0}, {-1, 1},
               {0, -1},           {0, 1},
               {1, -1}, {1, 0}, {1, 1},
        };
        constexpr static std::array<Pos, 26> QUEEN_OFFSETS{Pos
            {1, 0}, {2, 0}, {3, 0},
            {-1, 0}, {-2, 0}, {-3, 0},
            {0, 1}, {0, 2}, {0, 3}, {0, 4},
            {0, -1}, {0, -2}, {0, -3}, {0, -4},
            {1, 1}, {2, 2}, {3, 3},
            {1, -1}, {2, -2}, {3, -3},
            {-1, 1}, {-2, 2}, {-3, 3},
            {-1, -1}, {-2, -2}, {-3, -3},
        };
        constexpr static std::array<Pos, 14> ROOK_OFFSETS{Pos
            {1, 0}, {2, 0}, {3, 0},
            {-1, 0}, {-2, 0}, {-3, 0},
            {0, 1}, {0, 2}, {0, 3}, {0, 4},
            {0, -1}, {0, -2}, {0, -3}, {0, -4},
        };
        constexpr static std::array<Pos, 12> BISHOP_OFFSETS{Pos
            {1, 1}, {2, 2}, {3, 3},
            {1, -1}, {2, -2}, {3, -3},
            {-1, 1}, {-2, 2}, {-3, 3},
            {-1, -1}, {-2, -2}, {-3, -3},
        };
        constexpr static std::array<Pos, 8> HORSE_OFFSETS{Pos
            {2, 1}, {2, -1},
            {-2, 1}, {-2, -1},
            {1, 2}, {1, -2},
            {-1, 2}, {-1, -2},
        };
        constexpr static std::array<Pos, 2> BLACK_PAWN_TAKES_OFFSETS{Pos
            {-1, 1}, {1, 1},
        };
        constexpr static std::array<Pos, 2> WHITE_PAWN_TAKES_OFFSETS{Pos
            {-1, -1}, {1, -1},
        };
        constexpr static std::array<Pos, 2> BLACK_PAWN_MOVE_OFFSETS{Pos
            {0, 1},
        };
        constexpr static std::array<Pos, 2> WHITE_PAWN_MOVE_OFFSETS{Pos
            {0, -1},
        };



        std::vector<Pos> reachable_from(Pos from) {
            if(type == Type::EMPTY)
                return {};

            std::vector<Pos> offsets;
            if(type == Type::KING)
                offsets.assign(KING_OFFSETS.begin(), KING_OFFSETS.end());
            if(type == Type::QUEEN)
                offsets.assign(QUEEN_OFFSETS.begin(), QUEEN_OFFSETS.end());
            if(type == Type::ROOK)
                offsets.assign(ROOK_OFFSETS.begin(), ROOK_OFFSETS.end());
            if(type == Type::BISHOP)
                offsets.assign(BISHOP_OFFSETS.begin(), BISHOP_OFFSETS.end());
            if(type == Type::HORSE)
                offsets.assign(HORSE_OFFSETS.begin(), HORSE_OFFSETS.end());


        }
    };

    constexpr static uint8_t WIDTH = 4;
    constexpr static uint8_t HEIGHT = 5;

    constexpr static Piece EMPTY_TILE{Piece::Type::EMPTY, Piece::Color::EMPTY};
    constexpr static Piece W_K{Piece::Type::KING, Piece::Color::WHITE};
    constexpr static Piece W_R{Piece::Type::ROOK, Piece::Color::WHITE};
    constexpr static Piece W_B{Piece::Type::BISHOP, Piece::Color::WHITE};
    constexpr static Piece W_H{Piece::Type::HORSE, Piece::Color::WHITE};
    constexpr static Piece W_P{Piece::Type::PAWN, Piece::Color::WHITE};
    constexpr static Piece B_K{Piece::Type::KING, Piece::Color::BLACK};
    constexpr static Piece B_R{Piece::Type::ROOK, Piece::Color::BLACK};
    constexpr static Piece B_B{Piece::Type::BISHOP, Piece::Color::BLACK};
    constexpr static Piece B_H{Piece::Type::HORSE, Piece::Color::BLACK};
    constexpr static Piece B_P{Piece::Type::PAWN, Piece::Color::BLACK};

    std::array<std::array<Piece, WIDTH>, HEIGHT> board;

    [[nodiscard]] Pos get_king_position(Piece::Color color) const;
    [[nodiscard]] bool is_attacked(Pos from, Pos to) const  {
        Piece attacker = at(from);
        Piece victim = at(to);
        if(attacker.color == Piece::Color::EMPTY)
            return false;
        if(attacker.color == victim.color)
            return false;

        if(attacker.type == Piece::Type::PAWN) {
            if(attacker.color == Piece::Color::BLACK) {
            }
        }
    }

    [[nodiscard]] static bool valid_position(Pos pos) {
        return 0 <= pos.first && pos.first < WIDTH &&
               0 <= pos.second && pos.second < HEIGHT;
    }

    [[nodiscard]] Piece at(Pos pos) const {
        return operator[](pos);
    }

    [[nodiscard]] Piece operator[](Pos pos) const {
        return board[pos.second][pos.first];
    }


    friend std::ostream &operator<<(std::ostream &os, const MicroChess &v);
};


std::ostream &operator<<(std::ostream &os, const MicroChess &v);
