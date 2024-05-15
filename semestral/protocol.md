# Minimax algorithm

The minimax algorithm is a decision-making algorithm commonly used in game theory and artificial intelligence. It's primarily employed in two-player zero-sum games, where one player's gain is another player's loss, such as chess, checkers, or tic-tac-toe.

- Algorithm uses tree representation of game, each level representing a move by one player. The root of the tree is the current state of the game, and each subsequent level represents possible moves by alternating players.
- At the leaf nodes of the tree (end states of the game), utility function is applied to determine the "value" of final state. This function assigns a score to each leaf node, indicating how favorable the state is for the player.
- The algorithm works recursively, starting from the root moving to leafs and back up. At each level, it alternates between two types of nodes:
    - Max nodes: algorithm chooses move that maximises utility
    - Min nodes: algorithm chooses move that minimises utility

## Alpha - Beta pruning
Alpha-beta pruning optimizes the minimax algorithm by eliminating branches of the search tree
that cannot affect the final decision. It maintains two values, alpha (the best utility found so far)
and beta (the lowest utility found so far). Nodes are pruned if they cannot change the current best choice
for either player. This reduces the number of nodes evaluated, making the search more efficient while preserving the same result
as the standard minimax algorithm.

## Max depth
Certain games may take to many moves and/or may branch a lot.
This makes it unfeasible to traverse whole tree.
We need to make some shortcuts by approximating utility of nodes once we reach certain recursion depth.
We use some sort of heuristic for this.

# Results 
1. We created general minimax algorithm that accepts general game represented with class inherited from `class State`.
Game needs to implement three methods.
   1. Default constructor that construct starting state.
   2. `next_states()` which returns states reachable from current state.
   3. `get_heuristic()` should return approximate value of node.

    Rest of **state** functionality is implemented by base class.
2. To test algorithm we created two games: Tic-Tac-Toe and Checkers
   - Tic-Tac-Toe: As expected utility of starting state is **0**.
     Algorithm can correctly win if starting state is changes so that it is possible to win.
   - Checkers: We modified checkers to more simplified version; 
     pieces do not promote to queen, game ends once no moves are possible and
     score is calculated from survived pieces on each side.
     This game is already complicated enough so that we can not search whole game tree.
     We use utility function same as score function.
     For depth 12 or less player one always wins.
     We can see that from start the utility from heuristic is **0**,
     but only later in game, the utility increases to **1.2**.
