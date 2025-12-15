#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>
#include <random>
#include <algorithm>
#include "seqbfs.h"
#include "parbfs.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::vector<std::vector<int>> create_cube_grid(int size_x, int size_y, int size_z) {
    int n = size_x * size_y * size_z;
    std::vector<std::vector<int>> graph(n);

    auto get_index = [&](int x, int y, int z) {
        return x + y * size_x + z * size_x * size_y;
    };

    for (int z = 0; z < size_z; ++z) {
        for (int y = 0; y < size_y; ++y) {
            for (int x = 0; x < size_x; ++x) {
                int idx = get_index(x, y, z);

                if (x > 0) graph[idx].push_back(get_index(x - 1, y, z));
                if (x < size_x - 1) graph[idx].push_back(get_index(x + 1, y, z));
                if (y > 0) graph[idx].push_back(get_index(x, y - 1, z));
                if (y < size_y - 1) graph[idx].push_back(get_index(x, y + 1, z));
                if (z > 0) graph[idx].push_back(get_index(x, y, z - 1));
                if (z < size_z - 1) graph[idx].push_back(get_index(x, y, z + 1));
            }
        }
    }

    return graph;
}


bool test_simple_cases() {
    std::cout << "\nSIMPLE CORRECTNESS TESTS" << std::endl;
    int passed = 0;
    int total = 0;

    total++;
    {
        std::vector<std::vector<int>> graph = {{}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        if (seq.size() == 1 && par.size() == 1 && seq[0] == 0 && par[0] == 0) passed++;
        else std::cout << "FAIL: Single vertex test" << std::endl;
    }

    total++;
    {
        std::vector<std::vector<int>> graph = {{1}, {0, 2}, {1, 3}, {2}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        std::vector<int> expected = {0, 1, 2, 3};
        if (seq == expected && par == expected) passed++;
        else std::cout << "FAIL: Chain graph test" << std::endl;
    }

    total++;
    {
        std::vector<std::vector<int>> graph = {{1, 2, 3}, {0}, {0}, {0}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        std::vector<int> expected = {0, 1, 1, 1};
        if (seq == expected && par == expected) passed++;
        else std::cout << "FAIL: Star graph test" << std::endl;
    }

    total++;
    {
        std::vector<std::vector<int>> graph = {{1}, {0}, {3}, {2}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        if (seq[0] == 0 && seq[1] == 1 && seq[2] == -1 && seq[3] == -1 &&
            par[0] == 0 && par[1] == 1 && par[2] == -1 && par[3] == -1) passed++;
        else std::cout << "FAIL: Disconnected graph test" << std::endl;
    }

    total++;
    {
        int n = 5;
        std::vector<std::vector<int>> graph(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j) graph[i].push_back(j);
            }
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool ok = true;
        for (int i = 0; i < n; i++) {
            int expected = (i == 0) ? 0 : 1;
            if (seq[i] != expected || par[i] != expected) {
                ok = false;
                break;
            }
        }
        if (ok) passed++;
        else std::cout << "FAIL: Complete graph test" << std::endl;
    }

    std::cout << "Results: " << passed << "/" << total << " tests passed" << std::endl;
    return passed == total;
}

bool test_random_graphs() {
    std::cout << "\nRANDOM GRAPH TESTS" << std::endl;
    int passed = 0;
    int total = 0;

    std::mt19937 rng(42);

    for (int test_num = 0; test_num < 1000; test_num++) {
        total++;

        int n = 20 + rng() % 30;
        std::vector<std::vector<int>> graph(n);

        int edges = n * 2 + rng() % (n * 3);
        for (int i = 0; i < edges; i++) {
            int u = rng() % n;
            int v = rng() % n;
            if (u != v) {
                if (std::find(graph[u].begin(), graph[u].end(), v) == graph[u].end()) {
                    graph[u].push_back(v);
                    graph[v].push_back(u);
                }
            }
        }

        bool test_passed = true;
        for (int start = 0; start < std::min(3, n); start++) {
            auto seq = sequential_bfs(graph, start);
            auto par = parallel_bfs(graph, start);

            for (int i = 0; i < n; i++) {
                if (seq[i] != par[i]) {
                    test_passed = false;
                    std::cout << "FAIL: Random graph test " << test_num
                              << ", start=" << start << ", vertex=" << i << std::endl;
                    break;
                }
            }
            if (!test_passed) break;
        }

        if (test_passed) passed++;
    }

    std::cout << "Results: " << passed << "/" << total << " random graphs passed" << std::endl;
    return passed == total;
}

bool test_cube_small() {
    std::cout << "\nSMALL CUBE TEST" << std::endl;

    auto graph = create_cube_grid(5, 5, 5);
    int start = 0;

    auto seq = sequential_bfs(graph, start);
    auto par = parallel_bfs(graph, start);

    for (size_t i = 0; i < seq.size(); i++) {
        if (seq[i] != par[i]) {
            std::cout << "FAIL: Cube test mismatch at vertex " << i
                      << ": seq=" << seq[i] << ", par=" << par[i] << std::endl;
            return false;
        }
    }

    std::cout << "Small cube test passed (125 vertices)" << std::endl;
    return true;
}


int main() {
#ifdef _WIN32
    _putenv_s("PARLAY_NUM_THREADS", "4");
#else
    setenv("PARLAY_NUM_THREADS", "4", 1);
#endif
    bool all_tests_passed = true;

    if (!test_simple_cases()) all_tests_passed = false;
    if (!test_random_graphs()) all_tests_passed = false;
    if (!test_cube_small()) all_tests_passed = false;

    if (!all_tests_passed) {
        std::cout << "\n✗ Correctness tests failed! Aborting performance test" << std::endl;
        return 1;
    }

    std::cout << "\nALL CORRECTNESS TESTS PASSED!\n" << std::endl;

    std::cout << "PERFORMANCE TEST (300x300x300)" << std::endl;

    std::cout << "\nCreating 300x300x300 cube graph" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto graph = create_cube_grid(300, 300, 300);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto create_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int start = 0;
    std::cout << "Graph created: " << graph.size() << " vertices" << std::endl;
    std::cout << "Creation time: " << create_duration.count() << " ms" << std::endl;
    std::cout << std::endl;

    std::cout << "\nTesting Sequential BFS (5 runs avg)" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 5; i++) {
        sequential_bfs(graph, start);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto seq_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) / 5;
    std::cout << "Avg time: " << seq_duration.count() << " ms" << std::endl;

    std::cout << "\nTesting Parallel BFS (5 runs avg)" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 5; i++) {
        parallel_bfs(graph, start);
    }
    end_time = std::chrono::high_resolution_clock::now();
    auto par_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) / 5;
    std::cout << "Avg time: " << par_duration.count() << " ms" << std::endl;

    std::cout << "PERFORMANCE RESULTS" << std::endl;
    std::cout << "Sequential BFS: " << seq_duration.count() << " ms" << std::endl;
    std::cout << "Parallel BFS:   " << par_duration.count() << " ms" << std::endl;

    if (seq_duration.count() > 0 && par_duration.count() > 0) {
        double speedup = static_cast<double>(seq_duration.count()) / par_duration.count();
        std::cout << "Speedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }

    std::cout << "\nQuick correctness check on large graph" << std::endl;
    auto seq_result = sequential_bfs(graph, start);
    auto par_result = parallel_bfs(graph, start);

    srand(42);
    bool correct = true;
    int checks = 100;

    for (int i = 0; i < checks; i++) {
        int idx = rand() % graph.size();
        if (seq_result[idx] != par_result[idx]) {
            std::cout << "ERROR: Mismatch at vertex " << idx
                      << ": seq=" << seq_result[idx]
                      << ", par=" << par_result[idx] << std::endl;
            correct = false;
            break;
        }
    }

    if (correct) {
        std::cout << "Results are consistent (sampled " << checks << " vertices)" << std::endl;
    } else {
        std::cout << "Results are inconsistent!" << std::endl;
    }

    return 0;
}