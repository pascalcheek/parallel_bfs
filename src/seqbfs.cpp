#include "seqbfs.h"
#include <queue>

// Если писать нормальный BFS, то это не честно
std::vector<int> sequential_bfs(const std::vector<std::vector<int>>& graph, int start) {
    size_t* cur = new size_t[graph.size()];
    size_t* next = new size_t[graph.size()];
    size_t size = 1;
    size_t next_size;
    std::vector<int> res(graph.size(), -1);
    std::vector<bool> visited(graph.size(), false);
    cur[0] = start;
    res[start] = 0;
    visited[start] = true;
    while (size > 0) {
        next_size = 0;
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < graph[cur[i]].size(); j++) {
                if (!visited[graph[cur[i]][j]]) {
                    next[next_size++] = graph[cur[i]][j];
                    res[graph[cur[i]][j]] = res[cur[i]] + 1;
                    visited[graph[cur[i]][j]] = true;
                }
            }
        }
        std::swap(cur, next);
        size = next_size;
    }
    delete[] cur;
    delete[] next;

    return res;
}