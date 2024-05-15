cmake_minimum_required(VERSION 3.1...3.28)
set(CMAKE_CXX_STANDARD 20)


add_executable(hw01 hw01/hw01.cpp)

add_executable(hw02 hw02/hw02.cpp)

add_executable(semestral
        semestral/main.cpp
        semestral/minimax.h
        semestral/tic_tac_toe.cpp semestral/tic_tac_toe.h
        semestral/state.h
        semestral/games.h
        semestral/micro_chess.cpp
        semestral/micro_chess.h
        semestral/checkers.cpp
        semestral/checkers.h
)


project(ZUM)
