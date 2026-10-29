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


/**
 * @struct SearchResult
 * @brief A container to hold all performance metrics and the final path required for the report.
 */
struct SearchResult {
    bool success;                   // True if the goal was found, false otherwise
    std::vector<std::string> path;  // The sequence of board states from start to goal
    int path_cost;                  // Total number of moves (depth of the goal node)
    int nodes_expanded;             // Total number of nodes popped from the frontier
    int max_search_depth;           // The deepest level of the tree reached during the search
    double running_time_ms;         // Total execution time in milliseconds
};


/**
 * @struct CompareNode
 * @brief Custom comparator for the A* Priority Queue.
 * 
 * The priority queue normally outputs the largest value first (Max-Heap). 
 * By using the > operator on f_cost, we flip it into a Min-Heap, ensuring 
 * A* always explores the node with the lowest f(n) = g(n) + h(n) first.
 */
struct CompareNode {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        return a->f_cost() > b->f_cost();
    }
};

class SearchEngine {
public:
    
    /**
     * @brief A* Search (Informed Search)
     * Uses a Priority Queue (Min-Heap) to always expand the node with the lowest estimated total cost.
     * Guaranteed to find the optimal (shortest) path if the heuristic is admissible.
     * 
     * @param initial_state The starting 9-character board string.
     * @param use_manhattan True to use Manhattan distance, False to use Euclidean.
     */
    static SearchResult run_astar(const std::string& initial_state, bool use_manhattan) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Frontier: A Min-Heap sorted by f(n)
        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, CompareNode> frontier;

        // Visited (Explored Set): A Hash Set for O(1) constant time lookups to prevent infinite loops
        std::unordered_set<std::string> visited;

        // Initialize the root node with its starting heuristic cost
        int initial_h = use_manhattan ? manhattan_distance(initial_state) : (int)euclidean_distance(initial_state);
        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, initial_h, nullptr, "Start"});
        
        frontier.push(root);
        visited.insert(initial_state);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.top(); // Get the node with the lowest f_cost
            frontier.pop();

            max_depth = std::max(max_depth, current->g_cost);

            // Goal Check
            if (current->state == Board::GOAL_STATE) {
                return build_result(current, nodes_expanded, max_depth, start_time);
            }

            nodes_expanded++;
            
            // Expand node and add valid neighbors to the frontier
            for (auto& neighbor : Board::get_neighbors(current)) {
                if (visited.find(neighbor.state) == visited.end()) {
                    visited.insert(neighbor.state);
                    
                    // Calculate the heuristic for the new state dynamically based on user choice
                    neighbor.h_cost = use_manhattan ? manhattan_distance(neighbor.state) : (int)euclidean_distance(neighbor.state);
                    frontier.push(std::make_shared<Node>(neighbor));
                }
            }
        }
        return {false, {}, 0, nodes_expanded, max_depth, 0.0};
    }


    /**
     * @brief Breadth-First Search (Uninformed)
     * Uses a standard Queue (FIFO - First In, First Out).
     * Explores the tree level by level. Guaranteed to find the optimal path, but uses a lot of memory.
     */    static SearchResult run_bfs(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::queue<std::shared_ptr<Node>> frontier;
        std::unordered_set<std::string> visited;

        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
        frontier.push(root);
        visited.insert(initial_state);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.front(); // FIFO access
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

    /**
     * @brief Depth-First Search (Uninformed)
     * Uses a Stack (LIFO - Last In, First Out).
     * Dives as deep as possible down one path before backtracking. Highly memory efficient but NOT optimal.
     */
    static SearchResult run_dfs(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();

        std::stack<std::shared_ptr<Node>> frontier;
        std::unordered_set<std::string> visited;

        auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
        frontier.push(root);

        int nodes_expanded = 0;
        int max_depth = 0;

        while (!frontier.empty()) {
            auto current = frontier.top(); // LIFO access
            frontier.pop();

            // DFS must check visited status AFTER popping from the stack (unlike BFS)
            // because multiple paths might push the same node to the stack before it gets processed.
            if (visited.find(current->state) != visited.end()) continue;
            visited.insert(current->state);

            max_depth = std::max(max_depth, current->g_cost);

            if (current->state == Board::GOAL_STATE) {
                return build_result(current, nodes_expanded, max_depth, start_time);
            }

            nodes_expanded++;

            auto neighbors = Board::get_neighbors(current);
            // Reverse neighbors so the stack processes them in standard Up, Down, Left, Right order
            std::reverse(neighbors.begin(), neighbors.end());
            for (auto& neighbor : neighbors) {
                if (visited.find(neighbor.state) == visited.end()) {
                    frontier.push(std::make_shared<Node>(neighbor));
                }
            }
        }
        return {false, {}, 0, nodes_expanded, max_depth, 0.0};
    }


    /**
     * @brief Iterative Deepening DFS (Uninformed)
     * Combines the memory efficiency of DFS with the optimality of BFS.
     * Repeatedly runs a depth-limited DFS, increasing the depth limit by 1 each time until the goal is found.
     */
    static SearchResult run_ids(const std::string& initial_state) {
        auto start_time = std::chrono::high_resolution_clock::now();
        int total_nodes_expanded = 0;
        int global_max_depth = 0;

        // Iterate limits from 0 up to an arbitrary maximum (100)
        for (int depth_limit = 0; depth_limit < 100; ++depth_limit) {
            std::stack<std::shared_ptr<Node>> frontier;
            std::unordered_set<std::string> visited_in_path;

            // Note: In IDS, we do not use a global visited set. We allow revisiting states 
            // across different depth iterations to ensure completeness.
            auto root = std::make_shared<Node>(Node{initial_state, (int)initial_state.find('0'), 0, 0, nullptr, "Start"});
            frontier.push(root);

            while (!frontier.empty()) {
                auto current = frontier.top();
                frontier.pop();

                global_max_depth = std::max(global_max_depth, current->g_cost);

                if (current->state == Board::GOAL_STATE) {
                    return build_result(current, total_nodes_expanded, global_max_depth, start_time);
                }

                // Only generate and push neighbors if we haven't hit the current depth limit
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

    /**
     * @brief Utility function to trace the parent pointers back to the start and calculate final metrics.
     */
    static SearchResult build_result(const std::shared_ptr<Node>& goal_node, int nodes_expanded, int max_depth, std::chrono::high_resolution_clock::time_point start_time) {
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        std::vector<std::string> path;
        auto curr = goal_node;

        // Trace back from goal to start
        while (curr) { 
            path.push_back(curr->state); 
            curr = curr->parent; 
        }

        // Reverse the vector so it prints Start -> Goal instead of Goal -> Start
        std::reverse(path.begin(), path.end());

        return {true, path, goal_node->g_cost, nodes_expanded, max_depth, elapsed};
    }
};