#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cmath>

struct Node {
    std::string state;
    int zero_idx;
    int g_cost; // Path cost (depth)
    int h_cost; // Heuristic value
    std::shared_ptr<Node> parent;
    std::string move; // "Up", "Down", "Left", "Right"

    int f_cost() const { return g_cost + h_cost; }
};

class Board {
public:
    static const std::string GOAL_STATE; // "012345678"

    // Valid moves: Up (-3), Down (+3), Left (-1), Right (+1)
    static std::vector<Node> get_neighbors(const std::shared_ptr<Node>& current) {
        std::vector<Node> neighbors;
        int idx = current->zero_idx;
        int row = idx / 3, col = idx % 3;

        auto try_swap = [&](int new_row, int new_col, const std::string& move) {
            if (new_row >= 0 && new_row < 3 && new_col >= 0 && new_col < 3) {
                int new_idx = new_row * 3 + new_col;
                std::string next_state = current->state;
                std::swap(next_state[idx], next_state[new_idx]);
                neighbors.push_back({next_state, new_idx, current->g_cost + 1, 0, current, move});
            }
        };

        try_swap(row - 1, col, "Up");
        try_swap(row + 1, col, "Down");
        try_swap(row, col - 1, "Left");
        try_swap(row, col + 1, "Right");

        return neighbors;
    }
};