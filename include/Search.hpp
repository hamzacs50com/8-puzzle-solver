#pragma once
#include "Board.hpp"
#include "Heuristics.hpp"
#include <queue>
#include <stack>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <vector>
#include <memory>
#include <iostream>

struct SearchResult {
    bool success;
    std::vector<std::string> path;
    int path_cost;
    int nodes_expanded;
    int max_search_depth;
    double running_time_ms;
};

// Priority Queue Comparator for A*
struct CompareNode {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        return a->f_cost() > b->f_cost();
    }
};

class SearchEngine {
public:
    // --- 1. A* Search (Informed) ---
    static SearchResult run_astar(const std::string& initial_state, bool use_manhattan) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, CompareNode> frontier;
        std::unordered_set<std::string> visited;

        int initial_h = use_manhattan ? manhattan_distance(initial_state) : (int)euclidean_distance(initial_state);
        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, initial_h, nullptr, "Start"});
        
        frontier.push(root);
        visited.insert(initial_state);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.top();
            frontier.pop();

            max_depth = std::max(max_depth, current->g_cost);

            if (current->state == Board::GOAL_STATE) {
                return build_result(current, nodes_expanded, max_depth, start_time);
            }

            nodes_expanded++;
            
            for (auto& neighbor : Board::get_neighbors(current)) {
                if (visited.find(neighbor.state) == visited.end()) {
                    visited.insert(neighbor.state);
                    neighbor.h_cost = use_manhattan ? manhattan_distance(neighbor.state) : (int)euclidean_distance(neighbor.state);
                    frontier.push(std::make_shared<Node>(neighbor));
                }
            }
        }
        return {false, {}, 0, nodes_expanded, max_depth, 0.0};
    }

    // --- 2. Breadth-First Search (BFS) ---
    static SearchResult run_bfs(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::queue<std::shared_ptr<Node>> frontier;
        std::unordered_set<std::string> visited;

        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
        frontier.push(root);
        visited.insert(initial_state);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.front();
            frontier.pop();

            max_depth = std::max(max_depth, current->g_cost);

            if (current->state == Board::GOAL_STATE) {
                return build_result(current, nodes_expanded, max_depth, start_time);
            }

            nodes_expanded++;

            for (auto& neighbor : Board::get_neighbors(current)) {
                if (visited.find(neighbor.state) == visited.end()) {
                    visited.insert(neighbor.state);
                    frontier.push(std::make_shared<Node>(neighbor));
                }
            }
        }
        return {false, {}, 0, nodes_expanded, max_depth, 0.0};
    }

    // --- 3. Depth-First Search (DFS) ---
    static SearchResult run_dfs(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();

        std::stack<std::shared_ptr<Node>> frontier;
        std::unordered_set<std::string> visited;

        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
        frontier.push(root);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.top();
            frontier.pop();

            if (visited.find(current->state) != visited.end()) continue;
            visited.insert(current->state);

            max_depth = std::max(max_depth, current->g_cost);

            if (current->state == Board::GOAL_STATE) {
                return build_result(current, nodes_expanded, max_depth, start_time);
            }

            nodes_expanded++;

            auto neighbors = Board::get_neighbors(current);
            // Reverse to process neighbors in standard left-to-right order
            std::reverse(neighbors.begin(), neighbors.end());
            for (auto& neighbor : neighbors) {
                if (visited.find(neighbor.state) == visited.end()) {
                    frontier.push(std::make_shared<Node>(neighbor));
                }
            }
        }
        return {false, {}, 0, nodes_expanded, max_depth, 0.0};
    }

    // --- 4. Iterative Deepening DFS (IDS) ---
    static SearchResult run_ids(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();
        int total_nodes_expanded = 0;
        int global_max_depth = 0;

        for (int depth_limit = 0; depth_limit < 100; ++depth_limit) {
            std::stack<std::shared_ptr<Node>> frontier;
            std::unordered_set<std::string> visited_in_path;

            auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
            frontier.push(root);

            while (!frontier.empty()) {
                auto current = frontier.top();
                frontier.pop();

                global_max_depth = std::max(global_max_depth, current->g_cost);

                if (current->state == Board::GOAL_STATE) {
                    return build_result(current, total_nodes_expanded, global_max_depth, start_time);
                }

                if (current->g_cost < depth_limit) {
                    total_nodes_expanded++;
                    auto neighbors = Board::get_neighbors(current);
                    std::reverse(neighbors.begin(), neighbors.end());

                    for (auto& neighbor : neighbors) {
                        frontier.push(std::make_shared<Node>(neighbor));
                    }
                }
            }
        }
        return {false, {}, 0, total_nodes_expanded, global_max_depth, 0.0};
    }

private:
    static SearchResult build_result(const std::shared_ptr<Node>& goal_node, int nodes_expanded, int max_depth, std::chrono::high_resolution_clock::time_point start_time) {
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        std::vector<std::string> path;
        auto curr = goal_node;
        while (curr) { 
            path.push_back(curr->state); 
            curr = curr->parent; 
        }
        std::reverse(path.begin(), path.end());

        return {true, path, goal_node->g_cost, nodes_expanded, max_depth, elapsed};
    }
};