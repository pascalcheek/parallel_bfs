#include "parbfs.h"
#include <parlay/parallel.h>
#include <parlay/alloc.h>
#include <numeric>
#include <atomic>
#include <memory>

namespace {
size_t scan(size_t* a, size_t n) {
    if (n == 0) return 0;

    size_t res = a[n - 1];

    for (size_t i = 1; i < n; i *= 2) {
        parlay::parallel_for(1, n / (2 * i) + 1,
            [=] (size_t j) {
                size_t idx = 2 * i * j - 1;
                if (idx < n && idx - i < n) {
                    a[idx] += a[idx - i];
                }
            }
        );
    }

    a[n - 1] = 0;

    for (size_t i = n; i > 1; i /= 2) {
        parlay::parallel_for(1, n / i + 1,
            [=] (size_t j) {
                size_t x = i * j - (i / 2) - 1;
                size_t y = i * j - 1;
                if (x < n && y < n) {
                    a[x] += a[y];
                    std::swap(a[x], a[y]);
                }
            }
        );
    }

    return res + a[n - 1];
}
}

std::vector<int> parallel_bfs(const std::vector<std::vector<int>>& edges, int start_int) {
    size_t n = edges.size();

    std::vector<int> res(n, -1);
    size_t start = static_cast<size_t>(start_int);

    if (n == 0) return res;

    std::atomic_flag* visited = static_cast<std::atomic_flag*>(parlay::p_malloc(n * sizeof(std::atomic_flag)));

    parlay::parallel_for(0, n,
        [=] (size_t i) {
            std::atomic_flag* flag = new (visited + i) std::atomic_flag();
            flag->clear();
        }
    );

    size_t log_size = 1;
    while (log_size < n) {
        log_size *= 2;
    }

    size_t* buff_common = static_cast<size_t*>(parlay::p_malloc((3 * n + log_size) * sizeof(size_t)));

    size_t* current = buff_common;
    size_t* next = buff_common + n;
    size_t* next_by_node = buff_common + 2 * n;
    size_t* sizes = buff_common + 3 * n;

    visited[start].test_and_set();
    res[start] = 0;
    size_t current_size = 1;
    current[0] = start;

    while (current_size > 0) {
        parlay::parallel_for(0, current_size,
            [visited, &edges, &res, next_by_node, sizes, current] (size_t i) {
                sizes[i] = 0;
                size_t ind = current[i];
                size_t curr = ind;

                next_by_node[curr] = curr;

                const std::vector<int>& next_nodes = edges[ind];
                for (size_t j = 0; j < next_nodes.size(); j++) {
                    size_t k = static_cast<size_t>(next_nodes[j]);

                    if (!visited[k].test_and_set()) {
                        res[k] = res[ind] + 1;
                        sizes[i]++;
                        next_by_node[curr] = k;
                        curr = k;
                        next_by_node[curr] = curr;
                    }
                }
            }
        );


        size_t current_size2 = 1;
        while (current_size2 < current_size) {
            current_size2 *= 2;
        }

        parlay::parallel_for(current_size, current_size2,
            [=] (size_t i) {
                sizes[i] = 0;
            }
        );

        size_t k = scan(sizes, current_size2);

        parlay::parallel_for(0, current_size,
            [=] (size_t i) {
                size_t s = sizes[i];
                size_t curr = current[i];
                size_t j = 0;

                while (next_by_node[curr] != curr) {
                    curr = next_by_node[curr];
                    next[s + j] = curr;
                    j++;
                }
            }
        );


        std::swap(current, next);
        current_size = k;
    }

    parlay::p_free(buff_common);
    parlay::p_free(visited);

    return res;
}