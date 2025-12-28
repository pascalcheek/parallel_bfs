#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <thread>
#include <random>
#include <algorithm>
#include <queue>
#include <cassert>
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

// Крайние случаи и простые графы
bool test_extreme_cases() {
    std::cout << "\nEXTREME CASES" << std::endl;
    int passed = 0;
    int total = 0;


    // Одна вершина без рёбер
    total++;
    {
        std::vector<std::vector<int>> graph = {{}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        if (seq.size() == 1 && seq[0] == 0 && par.size() == 1 && par[0] == 0) {
            passed++;
            std::cout << "Single vertex test passed" << std::endl;
        } else {
            std::cout << "FAIL: Single vertex test" << std::endl;
        }
    }

    // Две изолированные компоненты
    total++;
    {
        std::vector<std::vector<int>> graph = {{1}, {0}, {3}, {2}};
        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);
        bool correct = (seq[0] == 0 && seq[1] == 1 && seq[2] == -1 && seq[3] == -1 &&
                       par[0] == 0 && par[1] == 1 && par[2] == -1 && par[3] == -1);
        if (correct) {
            passed++;
            std::cout << "Disconnected graph test passed" << std::endl;
        } else {
            std::cout << "FAIL: Disconnected graph test" << std::endl;
        }
    }

    // Полный граф
    total++;
    {
        int n = 50;
        std::vector<std::vector<int>> graph(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j) graph[i].push_back(j);
            }
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            int expected = (i == 0) ? 0 : 1;
            if (seq[i] != expected || par[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "Complete graph test passed" << std::endl;
        } else {
            std::cout << "FAIL: Complete graph test" << std::endl;
        }
    }

    // Цепочка (бамбук)
    total++;
    {
        int n = 1000;
        std::vector<std::vector<int>> graph(n);
        for (int i = 0; i < n; i++) {
            if (i > 0) graph[i].push_back(i - 1);
            if (i < n - 1) graph[i].push_back(i + 1);
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            if (seq[i] != i || par[i] != i) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "Chain graph (1000 vertices) test passed" << std::endl;
        } else {
            std::cout << "FAIL: Chain graph test" << std::endl;
        }
    }

    // Звезда
    total++;
    {
        int n = 101;
        std::vector<std::vector<int>> graph(n);
        for (int i = 1; i < n; i++) {
            graph[0].push_back(i);
            graph[i].push_back(0);
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            int expected = (i == 0) ? 0 : 1;
            if (seq[i] != expected || par[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "Star graph test passed" << std::endl;
        } else {
            std::cout << "FAIL: Star graph test" << std::endl;
        }
    }

    // Большая степень
    total++;
    {
        int n = 1000;
        std::vector<std::vector<int>> graph(n);
        // Вершина 0 соединена со всеми остальными
        for (int i = 1; i < n; i++) {
            graph[0].push_back(i);
            graph[i].push_back(0);
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            int expected = (i == 0) ? 0 : 1;
            if (seq[i] != expected || par[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "High-degree vertex test passed" << std::endl;
        } else {
            std::cout << "FAIL: High-degree vertex test" << std::endl;
        }
    }

    std::cout << "\nResults: " << passed << "/" << total << " extreme cases passed" << std::endl;
    return passed == total;
}

// Рандомные графы разной структуры
bool test_random_graphs() {
    std::cout << "\nRANDOM GRAPHS" << std::endl;
    int passed = 0;
    int total = 0;

    std::mt19937 rng(42);

    // Разные типы рандомных графов
    std::vector<std::pair<std::string, int>> graph_types = {
        {"Sparse (degree ~ 3)", 3},
        {"Medium (degree ~ 10)", 10},
        {"Dense (degree ~ 50)", 50},
        {"Very sparse (tree-like)", 2}
    };

    for (const auto& [type_name, avg_degree] : graph_types) {
        std::cout << "\nTesting " << type_name << " graphs:" << std::endl;

        for (int graph_num = 0; graph_num < 50; graph_num++) {
            total++;

            // Размер графа от 50 до 500 вершин
            int n = 50 + rng() % 451;
            std::vector<std::vector<int>> graph(n);

            // Создаём рёбра
            for (int u = 0; u < n; u++) {
                // Примерное количество соседей
                int degree = std::max(1, avg_degree + (int)(rng() % 5) - 2);
                degree = std::min(degree, n - 1);

                // Добавляем случайных соседей
                for (int d = 0; d < degree; d++) {
                    int v = rng() % n;
                    if (u != v) {
                        if (std::find(graph[u].begin(), graph[u].end(), v) == graph[u].end()) {
                            graph[u].push_back(v);
                            graph[v].push_back(u);
                        }
                    }
                }
            }

            // Тестируем с несколькими стартовыми вершинами
            bool graph_correct = true;
            int tests_per_graph = std::min(5, n);

            for (int test = 0; test < tests_per_graph; test++) {
                int start = rng() % n;

                auto seq = sequential_bfs(graph, start);
                auto par = parallel_bfs(graph, start);

                // Проверяем корректность расстояний
                for (int i = 0; i < n; i++) {
                    if (seq[i] != par[i]) {
                        graph_correct = false;
                        std::cout << "Mismatch at graph " << graph_num
                                  << ", start=" << start << ", vertex=" << i
                                  << ": seq=" << seq[i] << ", par=" << par[i] << std::endl;
                        break;
                    }

                    // Проверка корректности расстояний (опционально)
                    if (seq[i] >= 0 && i != start) {
                        // Хотя бы один сосед должен быть на расстоянии на 1 меньше
                        bool has_closer_neighbor = false;
                        for (int neighbor : graph[i]) {
                            if (seq[neighbor] == seq[i] - 1) {
                                has_closer_neighbor = true;
                                break;
                            }
                        }
                        if (!has_closer_neighbor && seq[i] > 0) {
                            graph_correct = false;
                            std::cout << "Invalid distance at graph " << graph_num
                                      << ", vertex=" << i << ": distance=" << seq[i] << std::endl;
                            break;
                        }
                    }
                }

                if (!graph_correct) break;
            }

            if (graph_correct) {
                passed++;
                if (graph_num % 10 == 0) {
                    std::cout << "Progress: " << graph_num << "/50" << std::endl;
                }
            } else {
                break;
            }
        }

        std::cout << type_name << ": " << (total > 0 ? (passed * 100 / total) : 0)
                  << "% passed" << std::endl;
    }

    std::cout << "\nResults: " << passed << "/" << total << " random graphs passed" << std::endl;
    return passed == total;
}

// Специальные графы
bool test_special_graphs() {
    std::cout << "\nSPECIAL GRAPHS" << std::endl;
    int passed = 0;
    int total = 0;

    // Кольцо
    total++;
    {
        int n = 100;
        std::vector<std::vector<int>> graph(n);
        for (int i = 0; i < n; i++) {
            graph[i].push_back((i + 1) % n);
            graph[i].push_back((i + n - 1) % n);
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            int expected = std::min(i, n - i);
            if (seq[i] != expected || par[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "Cycle graph test passed" << std::endl;
        } else {
            std::cout << "FAIL: Cycle graph test" << std::endl;
        }
    }

    // Решётка 2D
    total++;
    {
        int size = 100;
        int n = size * size;
        std::vector<std::vector<int>> graph(n);

        auto get_index = [&](int x, int y) {
            return x + y * size;
        };

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int idx = get_index(x, y);
                if (x > 0) graph[idx].push_back(get_index(x - 1, y));
                if (x < size - 1) graph[idx].push_back(get_index(x + 1, y));
                if (y > 0) graph[idx].push_back(get_index(x, y - 1));
                if (y < size - 1) graph[idx].push_back(get_index(x, y + 1));
            }
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            if (seq[i] != par[i]) {
                correct = false;
                std::cout << "Mismatch at vertex " << i << std::endl;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "2D grid test passed" << std::endl;
        } else {
            std::cout << "FAIL: 2D grid test" << std::endl;
        }
    }

    // Полный двудольный граф
    total++;
    {
        int n1 = 50, n2 = 50;
        int n = n1 + n2;
        std::vector<std::vector<int>> graph(n);

        for (int i = 0; i < n1; i++) {
            for (int j = n1; j < n; j++) {
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }

        auto seq = sequential_bfs(graph, 0);
        auto par = parallel_bfs(graph, 0);

        bool correct = true;
        for (int i = 0; i < n; i++) {
            int expected = (i == 0) ? 0 : (i < n1 ? 2 : 1);
            if (seq[i] != expected || par[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            passed++;
            std::cout << "Complete bipartite graph test passed" << std::endl;
        } else {
            std::cout << "FAIL: Complete bipartite graph test" << std::endl;
        }
    }

    std::cout << "\nResults: " << passed << "/" << total << " special graphs passed" << std::endl;
    return passed == total;
}

// Маленький кубический граф
bool test_small_cube() {
    std::cout << "\nSMALL CUBE 5x5x5" << std::endl;

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

    // Дополнительная проверка: расстояния должны соответствовать Манхэттенскому расстоянию
    bool distances_correct = true;
    for (int z = 0; z < 5; z++) {
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                int idx = x + y * 5 + z * 5 * 5;
                int expected = x + y + z;
                if (seq[idx] != expected) {
                    distances_correct = false;
                    std::cout << "Wrong distance at (" << x << "," << y << "," << z
                              << "): expected=" << expected << ", got=" << seq[idx] << std::endl;
                }
            }
        }
    }

    if (distances_correct) {
        std::cout << "Small cube test passed (125 vertices, correct distances)" << std::endl;
    }

    return distances_correct;
}

// Тест производительности на большом кубе
void performance_test() {
    std::cout << "\nPERFORMANCE TEST" << std::endl;

    std::cout << "\nCreating 300x300x300 cube graph" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto graph = create_cube_grid(300, 300, 300);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto create_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int start = 0;
    std::cout << "Graph created: " << graph.size() << " vertices" << std::endl;
    std::cout << "Creation time: " << create_duration.count() << " ms" << std::endl;

    // Тестирование последовательного BFS
    std::cout << "\nTesting Sequential BFS (5 runs)" << std::endl;
    std::vector<long long> seq_times;

    for (int run = 0; run < 5; run++) {
        start_time = std::chrono::high_resolution_clock::now();
        auto seq_result = sequential_bfs(graph, start);
        end_time = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        seq_times.push_back(duration.count());

        std::cout << "  Run " << (run + 1) << ": " << duration.count() << " ms" << std::endl;
    }

    long long avg_seq = std::accumulate(seq_times.begin(), seq_times.end(), 0LL) / seq_times.size();
    std::cout << "Average sequential time: " << avg_seq << " ms" << std::endl;

    std::cout << "\nTesting Parallel BFS (5 runs)" << std::endl;
    std::vector<long long> par_times;

    for (int run = 0; run < 5; run++) {
        start_time = std::chrono::high_resolution_clock::now();
        auto par_result = parallel_bfs(graph, start);
        end_time = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        par_times.push_back(duration.count());

        std::cout << "  Run " << (run + 1) << ": " << duration.count() << " ms" << std::endl;
    }

    long long avg_par = std::accumulate(par_times.begin(), par_times.end(), 0LL) / par_times.size();
    std::cout << "Average parallel time: " << avg_par << " ms" << std::endl;

    std::cout << "\nPERFORMANCE RESULTS" << std::endl;
    std::cout << "Sequential BFS: " << avg_seq << " ms" << std::endl;
    std::cout << "Parallel BFS:   " << avg_par << " ms" << std::endl;


    double speedup = static_cast<double>(avg_seq) / avg_par;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;

}

int main() {
    std::cout << "PARALLEL BFS TEST SUITE" << std::endl;

    bool all_tests_passed = true;

    // Запуск всех тестов на корректность
    std::cout << "\nStarting correctness tests" << std::endl;

    if (!test_extreme_cases()) {
        all_tests_passed = false;
        std::cout << "\nExtreme cases tests failed!" << std::endl;
    }

    if (!test_random_graphs()) {
        all_tests_passed = false;
        std::cout << "\nRandom graphs tests failed!" << std::endl;
    }

    if (!test_special_graphs()) {
        all_tests_passed = false;
        std::cout << "\nSpecial graphs tests failed!" << std::endl;
    }

    if (!test_small_cube()) {
        all_tests_passed = false;
        std::cout << "\nSmall cube test failed!" << std::endl;
    }

    if (!all_tests_passed) {
        std::cout << "\nCORRECTNESS TESTS FAILED! Aborting performance test." << std::endl;
        return 1;
    }

    std::cout << "\nALL CORRECTNESS TESTS PASSED!" << std::endl;

    performance_test();

    std::cout << "\nTEST SUITE COMPLETE" << std::endl;

    return 0;
}