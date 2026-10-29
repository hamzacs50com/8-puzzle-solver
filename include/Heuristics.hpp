#pragma once
#include <string>
#include <cmath>

/**
 * @brief Calculates the Manhattan Distance heuristic for the 8-puzzle.
 * 
 * Manhattan distance is the sum of the absolute vertical and horizontal distances 
 * each tile is from its target goal position. It assumes tiles can move through each other.
 * 
 * @param state The current 9-character string representation of the board.
 * @return int The total Manhattan distance of all misplaced tiles.
 */
inline int manhattan_distance(const std::string& state) {
    int distance = 0;
    for (int i = 0; i < 9; ++i) {
        char val = state[i];

        // We do not calculate the distance for the empty space ('0').
        // Only the physical tiles (1-8) matter for the heuristic cost.
        if (val == '0') continue;

        // Convert the character to its integer value (e.g., '5' becomes 5).
        // Since our goal state is "012345678", the integer value IS the target index!
        int target = val - '0';

        // Convert the 1D target index into a 2D (row, col) coordinate
        int target_row = target / 3, target_col = target % 3;

        // Convert the 1D current index 'i' into a 2D (row, col) coordinate
        int current_row = i / 3, current_col = i % 3;

        // Add the absolute orthogonal distances (dx + dy) to the total
        distance += std::abs(current_row - target_row) + std::abs(current_col - target_col);
    }
    return distance;
}


/**
 * @brief Calculates the Euclidean Distance heuristic for the 8-puzzle.
 * 
 * Euclidean distance is the straight-line ("as the crow flies") distance between 
 * a tile's current position and its goal position using the Pythagorean theorem.
 * 
 * @param state The current 9-character string representation of the board.
 * @return double The total Euclidean distance of all misplaced tiles.
 */
inline double euclidean_distance(const std::string& state) {
    double distance = 0.0;
    for (int i = 0; i < 9; ++i) {
        char val = state[i];
        if (val == '0') continue;
        int target = val - '0';
        int target_row = target / 3, target_col = target % 3;
        int current_row = i / 3, current_col = i % 3;

        // Calculate straight-line distance: sqrt((x2 - x1)^2 + (y2 - y1)^2)
        distance += std::sqrt(std::pow(current_row - target_row, 2) + std::pow(current_col - target_col, 2));
    }
    return distance;
}