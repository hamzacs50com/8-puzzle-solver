# 8-Puzzle AI Solver 

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

An optimized Artificial Intelligence agent designed to solve the classic 8-Puzzle game using Informed and Uninformed search algorithms. 

This project was developed for the **Artificial Intelligence Course at Alexandria University (Faculty of Engineering)**.

## Features
- **Uninformed Search:** Breadth-First Search (BFS), Depth-First Search (DFS), Iterative Deepening DFS (IDS).
- **Informed Search:** A* Search utilizing **Manhattan** and **Euclidean** distance heuristics.
- **Optimized C++17 Engine:** $O(1)$ state hashing and flat 1D string representations for blazing-fast node expansion.
- **Web Visualizer:** Interactive frontend to playback the algorithm's path to the goal.

## Project Structure
```text
8-puzzle-solver/
├── include/
│   ├── Board.hpp       # State representation & neighbor generation
│   ├── Heuristics.hpp  # Manhattan & Euclidean distance functions
│   └── Search.hpp      # Search algorithms logic
├── src/
│   └── main.cpp        # CLI Driver & JSON exporter
├── output/
│   ├── visualizer.html # Interactive web GUI
│   └── solution.json   # Auto-generated metrics & path (Created at runtime)
├── CMakeLists.txt      # Build configuration
└── README.md
```

## Build Instructions (Ubuntu/Linux)

This project uses CMake for clean, out-of-source builds.

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Run the solver from the terminal, providing an initial state (9 characters) and the desired algorithm.

**Syntax:**
```bash
./solver <initial_state> <algorithm>
```

**Available Algorithms:**
`bfs`, `dfs`, `ids`, `astar_manhattan`, `astar_euclidean`, `all`

**Example:**
```bash
./solver 125340678 astar_manhattan
```

## Running the Web Visualizer
To see the step-by-step animation of the solution:
1. Run the solver so it generates `output/solution.json`.
2. Start a local server in the project root:
   ```bash
   python3 -m http.server 8000
   ```
3. Open your browser and navigate to: `http://localhost:8000/output/visualizer.html`

## 👥 Group Members
1. **[Hamza Abdel Aziz Hamza]** - [9290]
2. **[Karim Mohamed Khaled]** - [9004]
3. **[Ezz Eldeen Mohamed Fathy]** - [9114]