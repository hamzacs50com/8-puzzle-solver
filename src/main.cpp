#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Board.hpp"
#include "Search.hpp"

const std::string Board::GOAL_STATE = "012345678";

// --- Solvability Check (Inversion Count) ---
bool is_solvable(const std::string& state) {
    int inversions = 0;
    std::string filtered = "";
    for (char c : state) {
        if (c != '0') filtered += c;
    }
    for (size_t i = 0; i < filtered.size(); ++i) {
        for (size_t j = i + 1; j < filtered.size(); ++j) {
            if (filtered[i] > filtered[j]) inversions++;
        }
    }
    return (inversions % 2 == 0);
}

// --- Save Results to JSON for Web GUI ---
void export_json(const SearchResult& res, const std::string& algo_name, const std::string& filepath) {
    std::ofstream out(filepath);
    out << "{\n";
    out << "  \"algorithm\": \"" << algo_name << "\",\n";
    out << "  \"success\": " << (res.success ? "true" : "false") << ",\n";
    out << "  \"path_cost\": " << res.path_cost << ",\n";
    out << "  \"nodes_expanded\": " << res.nodes_expanded << ",\n";
    out << "  \"max_depth\": " << res.max_search_depth << ",\n";
    out << "  \"running_time_ms\": " << res.running_time_ms << ",\n";
    out << "  \"path\": [\n";
    for (size_t i = 0; i < res.path.size(); ++i) {
        out << "    \"" << res.path[i] << "\"" << (i + 1 < res.path.size() ? "," : "") << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    out.close();
    std::cout << "Exported solution to " << filepath << std::endl;
}

void print_terminal_trace(const SearchResult& res, const std::string& algo_name) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "  ALGORITHM: " << algo_name << std::endl;
    std::cout << "==========================================" << std::endl;

    if (!res.success) {
        std::cout << "No solution found." << std::endl;
        return;
    }

    std::cout << "\n--- Path Trace ---" << std::endl;
    for (size_t step = 0; step < res.path.size(); ++step) {
        const auto& s = res.path[step];
        std::cout << "Step " << step << ":\n";
        std::cout << " " << s[0] << " " << s[1] << " " << s[2] << "\n";
        std::cout << " " << s[3] << " " << s[4] << " " << s[5] << "\n";
        std::cout << " " << s[6] << " " << s[7] << " " << s[8] << "\n\n";
    }

    std::cout << "--- Performance Metrics ---" << std::endl;
    std::cout << "Cost of path     : " << res.path_cost << std::endl;
    std::cout << "Nodes expanded   : " << res.nodes_expanded << std::endl;
    std::cout << "Search depth     : " << res.max_search_depth << std::endl;
    std::cout << "Running time     : " << res.running_time_ms << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string initial_state = (argc > 1) ? argv[1] : "125340678";
    std::string algo = (argc > 2) ? argv[2] : "astar_manhattan";

    if (initial_state.length() != 9) {
        std::cerr << "Error: Board state must be 9 characters long!" << std::endl;
        return 1;
    }

    if (!is_solvable(initial_state)) {
        std::cerr << "Warning: The provided board state (" << initial_state << ") is UNSOLVABLE!" << std::endl;
        return 1;
    }

    SearchResult res;
    if (algo == "bfs") res = SearchEngine::run_bfs(initial_state);
    else if (algo == "dfs") res = SearchEngine::run_dfs(initial_state);
    else if (algo == "ids") res = SearchEngine::run_ids(initial_state);
    else if (algo == "astar_euclidean") res = SearchEngine::run_astar(initial_state, false);
    else res = SearchEngine::run_astar(initial_state, true); // default

    print_terminal_trace(res, algo);
    export_json(res, algo, "output/solution.json");

    return 0;
}