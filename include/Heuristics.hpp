#pragma once
#include <string>
#include <cmath>

inline int manhattan_distance(const std::string& state) {
    int distance = 0;
    for (int i = 0; i < 9; ++i) {
        char val = state[i];
        if (val == '0') continue;
        int target = val - '0';
        int target_row = target / 3, target_col = target % 3;
        int current_row = i / 3, current_col = i % 3;
        distance += std::abs(current_row - target_row) + std::abs(current_col - target_col);
    }
    return distance;
}

inline double euclidean_distance(const std::string& state) {
    double distance = 0.0;
    for (int i = 0; i < 9; ++i) {
        char val = state[i];
        if (val == '0') continue;
        int target = val - '0';
        int target_row = target / 3, target_col = target % 3;
        int current_row = i / 3, current_col = i % 3;
        distance += std::sqrt(std::pow(current_row - target_row, 2) + std::pow(current_col - target_col, 2));
    }
    return distance;
}