#include "seqbfs.h"
#include <queue>

std::vector<int> sequential_bfs(const std::vector<std::vector<int>>& graph, int start) {
    std::vector<int> distance(graph.size(), -1);
    std::queue<int> frontier;

    frontier.push(start);
    distance[start] = 0;

    while (!frontier.empty()) {
        int v = frontier.front();
        frontier.pop();

        for (int u : graph[v]) {
            if (distance[u] == -1) {
                distance[u] = distance[v] + 1;
                frontier.push(u);
            }
        }
    }
    
    return distance;
}