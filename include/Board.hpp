#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cmath>


/**
 * @struct Node
 * @brief Represents a single state (or node) in the search tree.
 * 
 * We use a struct instead of a class here because all members are public data 
 * containers meant to be easily accessed by the search algorithms.
 */
struct Node {
    std::string state;              // The 9-character board state (e.g., "125340678")
    int zero_idx;                   // The 1D array index (0-8) where the empty space '0' is located
    int g_cost;                     // Path cost: Number of steps taken from the initial state to reach this node
    int h_cost;                     // Heuristic cost: Estimated distance to the goal (calculated by Manhattan/Euclidean)
    std::shared_ptr<Node> parent;   // Pointer to the previous node (used to trace the path back once the goal is found)
    std::string move;               // The move ("Up", "Down", "Left", "Right") that resulted in this state

    /**
     * @brief Calculates the total estimated cost of this node for A* Search.
     * f(n) = g(n) + h(n)
     */
    int f_cost() const { return g_cost + h_cost; }
};

class Board {
public:
    // The target configuration we want to reach.
    static const std::string GOAL_STATE; // "012345678"


    /**
     * @brief Generates all valid neighboring states from the current state.
     * 
     * @param current A shared pointer to the current Node in the search tree.
     * @return std::vector<Node> A list of valid adjacent states after moving the blank space.
     */
    static std::vector<Node> get_neighbors(const std::shared_ptr<Node>& current) {
        std::vector<Node> neighbors;

        // Step 1: Find the 2D grid coordinates of the empty space ('0')
        int idx = current->zero_idx;
        int row = idx / 3;      // Integer division gives the row (0, 1, or 2)
        int col = idx % 3;      // Modulus gives the column (0, 1, or 2)

        /**
         * @brief A lambda function (inline function) to test and apply a tile slide.
         * It captures external variables by reference `[&]`.
         */
        auto try_swap = [&](int new_row, int new_col, const std::string& move) {

            // Step 2: Boundary Check. Ensure the move doesn't push '0' off the 3x3 board.
            if (new_row >= 0 && new_row < 3 && new_col >= 0 && new_col < 3) {

                // Step 3: Convert the valid 2D coordinate back into a 1D string index
                int new_idx = new_row * 3 + new_col;

                // Step 4: Create the new board state by swapping '0' with the target tile
                std::string next_state = current->state;
                std::swap(next_state[idx], next_state[new_idx]);

                // Step 5: Add the new state to our list of neighbors.
                // Note: h_cost is temporarily set to 0. The SearchEngine will calculate it later if needed.
                neighbors.push_back({next_state, new_idx, current->g_cost + 1, 0, current, move});
            }
        };

        // Step 6: Attempt all 4 possible orthogonal moves
        // If the empty space is at the top edge (row 0), "Up" (row - 1) will fail the boundary check and be ignored.
        try_swap(row - 1, col, "Up");
        try_swap(row + 1, col, "Down");
        try_swap(row, col - 1, "Left");
        try_swap(row, col + 1, "Right");

        return neighbors;
    }
};