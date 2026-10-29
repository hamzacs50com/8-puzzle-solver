#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include "Board.hpp"
#include "Search.hpp"

// Define the static constant goal state declared in Board.hpp
const std::string Board::GOAL_STATE = "012345678";


/**
 * @brief Checks if a given 8-puzzle board configuration is mathematically solvable.
 * 
 * In the 8-puzzle game, half of the possible board configurations are completely 
 * unsolvable due to the physical constraints of how tiles slide. We determine 
 * solvability by counting "inversions". An inversion occurs when a larger numbered 
 * tile appears before a smaller numbered tile in the 1D flat string.
 * If the total number of inversions is even, the puzzle is solvable. If odd, it's impossible.
 * 
 * @param state The 9-character starting board state.
 * @return true if solvable, false if unsolvable.
 */
bool is_solvable(const std::string& state) {
    int inversions = 0;
    std::string filtered = "";
    
    // Step 1: Remove the empty space ('0') from our inversion calculation
    for (char c : state) {
        if (c != '0') filtered += c;
    }
    
    // Step 2: Compare every tile against all tiles that come after it
    for (size_t i = 0; i < filtered.size(); ++i) {
        for (size_t j = i + 1; j < filtered.size(); ++j) {
            // If a larger number appears before a smaller number, it's an inversion
            if (filtered[i] > filtered[j]) inversions++;
        }
    }
    
    // Step 3: The puzzle is solvable ONLY if the total inversion count is even
    return (inversions % 2 == 0);
}


/**
 * @brief Exports the search results to a JSON file format for the Web UI Visualizer.
 * 
 * By exporting an array of results `[{...}, {...}]`, the HTML frontend can read 
 * multiple algorithmic solutions at once and populate a dropdown menu to switch between them.
 * 
 * @param runs A vector of pairs, where each pair contains the algorithm's string name and its SearchResult.
 * @param filepath The destination path for the JSON file (e.g., "output/solution.json").
 */
void export_json_array(const std::vector<std::pair<std::string, SearchResult>>& runs, const std::string& filepath) {
    std::ofstream out(filepath);
    out << "[\n"; // Begin JSON Array
    
    for (size_t r = 0; r < runs.size(); ++r) {
        const auto& algo_name = runs[r].first;
        const auto& res = runs[r].second;
        
        out << "  {\n";
        out << "    \"algorithm\": \"" << algo_name << "\",\n";
        out << "    \"success\": " << (res.success ? "true" : "false") << ",\n";
        out << "    \"path_cost\": " << res.path_cost << ",\n";
        out << "    \"nodes_expanded\": " << res.nodes_expanded << ",\n";
        out << "    \"max_depth\": " << res.max_search_depth << ",\n";
        out << "    \"running_time_ms\": " << res.running_time_ms << ",\n";
        
        // Export the sequence of string states so the web UI can animate the tiles moving
        out << "    \"path\": [\n";
        for (size_t i = 0; i < res.path.size(); ++i) {
            out << "      \"" << res.path[i] << "\"" << (i + 1 < res.path.size() ? "," : "") << "\n";
        }
        out << "    ]\n";
        
        // Add a comma between objects in the JSON array, except for the very last object
        out << "  }" << (r + 1 < runs.size() ? "," : "") << "\n";
    }
    out << "]\n"; // End JSON Array
    out.close();
}


/**
 * @brief Prints the performance metrics and step-by-step path matrix to the terminal.
 * 
 * @param res The final SearchResult object containing the metrics and path.
 * @param algo_name The name of the algorithm that just finished running.
 * @param print_steps If true, prints the 3x3 ASCII matrix for every step. 
 *                    If false, only prints the final numeric metrics.
 */
void print_terminal_trace(const SearchResult& res, const std::string& algo_name, bool print_steps) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "  ALGORITHM: " << algo_name << std::endl;
    std::cout << "==========================================" << std::endl;

    if (!res.success) {
        std::cout << "No solution found." << std::endl;
        return;
    }

    if (print_steps) {
        std::cout << "\n--- Path Trace ---" << std::endl;
        for (size_t step = 0; step < res.path.size(); ++step) {
            const auto& s = res.path[step];
            std::cout << "Step " << step << ":\n";
            std::cout << " " << s[0] << " " << s[1] << " " << s[2] << "\n";
            std::cout << " " << s[3] << " " << s[4] << " " << s[5] << "\n";
            std::cout << " " << s[6] << " " << s[7] << " " << s[8] << "\n\n";
        }
    }

    std::cout << "--- Performance Metrics ---" << std::endl;
    std::cout << "Cost of path     : " << res.path_cost << std::endl;
    std::cout << "Nodes expanded   : " << res.nodes_expanded << std::endl;
    std::cout << "Search depth     : " << res.max_search_depth << std::endl;
    std::cout << "Running time     : " << res.running_time_ms << " ms" << std::endl;
}


/**
 * @brief The main entry point of the application.
 * 
 * Usage: ./solver <initial_state> <algorithm>
 * Example: ./solver 125340678 astar_manhattan
 */
int main(int argc, char* argv[]) {
    // Read command-line arguments, providing fallbacks if the user forgets to type them
    std::string initial_state = (argc > 1) ? argv[1] : "125340678";
    std::string algo = (argc > 2) ? argv[2] : "astar_manhattan";

    // Input Validation 1: Ensure the string is exactly 9 characters long
    if (initial_state.length() != 9) {
        std::cerr << "Error: Board state must be 9 characters long!" << std::endl;
        return 1;
    }

    // Input Validation 2: Ensure the user didn't enter a mathematically impossible puzzle
    if (!is_solvable(initial_state)) {
        std::cerr << "Warning: The provided board state (" << initial_state << ") is UNSOLVABLE!" << std::endl;
        return 1;
    }

    // Benchmark Mode: Execute all 5 algorithms sequentially and package them for the GUI
    if (algo == "all") {
        std::cout << "\nExecuting Benchmark for All Algorithms...\n";
        std::vector<std::pair<std::string, SearchResult>> all_runs;

        SearchResult r_bfs = SearchEngine::run_bfs(initial_state);
        print_terminal_trace(r_bfs, "BFS", false);
        all_runs.push_back({"bfs", r_bfs});

        SearchResult r_dfs = SearchEngine::run_dfs(initial_state);
        print_terminal_trace(r_dfs, "DFS", false);
        all_runs.push_back({"dfs", r_dfs});

        SearchResult r_ids = SearchEngine::run_ids(initial_state);
        print_terminal_trace(r_ids, "IDS", false);
        all_runs.push_back({"ids", r_ids});

        SearchResult r_astarm = SearchEngine::run_astar(initial_state, true);
        print_terminal_trace(r_astarm, "A* (Manhattan)", false);
        all_runs.push_back({"astar_manhattan", r_astarm});

        SearchResult r_astare = SearchEngine::run_astar(initial_state, false);
        print_terminal_trace(r_astare, "A* (Euclidean)", false);
        all_runs.push_back({"astar_euclidean", r_astare});

        // Export the ENTIRE array of 5 algorithms to the GUI dropdown
        export_json_array(all_runs, "output/solution.json");
        std::cout << "\nExported all 5 solutions to output/solution.json for UI Selection\n";
    } 
    // Single Run Mode: Execute only the algorithm specified by the user
    else {
        SearchResult res;
        
        // Map the string argument to the correct function call
        if (algo == "bfs") res = SearchEngine::run_bfs(initial_state);
        else if (algo == "dfs") res = SearchEngine::run_dfs(initial_state);
        else if (algo == "ids") res = SearchEngine::run_ids(initial_state);
        else if (algo == "astar_euclidean") res = SearchEngine::run_astar(initial_state, false);
        else {
            res = SearchEngine::run_astar(initial_state, true);
            algo = "astar_manhattan"; // Fallback safety
        }

        // Print the full path matrix to the terminal so the user can visually trace the logic
        print_terminal_trace(res, algo, true);
        
        // Export as an array of 1 algorithm so the GUI JSON parser doesn't break
        std::vector<std::pair<std::string, SearchResult>> single_run = {{algo, res}};
        export_json_array(single_run, "output/solution.json");
        std::cout << "Exported solution to output/solution.json\n";
    }

    return 0;
}