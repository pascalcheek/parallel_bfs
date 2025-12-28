## Запуск:
```
C:\Users\paska\CLionProjects\parallel_bfs\cmake-build-debug\speed_measure.exe
PARALLEL BFS TEST SUITE

Starting correctness tests

EXTREME CASES
Single vertex test passed
Disconnected graph test passed
Complete graph test passed
Chain graph (1000 vertices) test passed
Star graph test passed
High-degree vertex test passed

Results: 6/6 extreme cases passed

RANDOM GRAPHS

Testing Sparse (degree ~ 3) graphs:
Progress: 0/50
Progress: 10/50
Progress: 20/50
Progress: 30/50
Progress: 40/50
Sparse (degree ~ 3): 100% passed

Testing Medium (degree ~ 10) graphs:
Progress: 0/50
Progress: 10/50
Progress: 20/50
Progress: 30/50
Progress: 40/50
Medium (degree ~ 10): 100% passed

Testing Dense (degree ~ 50) graphs:
Progress: 0/50
Progress: 10/50
Progress: 20/50
Progress: 30/50
Progress: 40/50
Dense (degree ~ 50): 100% passed

Testing Very sparse (tree-like) graphs:
Progress: 0/50
Progress: 10/50
Progress: 20/50
Progress: 30/50
Progress: 40/50
Very sparse (tree-like): 100% passed

Results: 200/200 random graphs passed

SPECIAL GRAPHS
Cycle graph test passed
2D grid test passed
Complete bipartite graph test passed

Results: 3/3 special graphs passed

SMALL CUBE 5x5x5
Small cube test passed (125 vertices, correct distances)

ALL CORRECTNESS TESTS PASSED!

PERFORMANCE TEST

Creating 300x300x300 cube graph
Graph created: 27000000 vertices
Creation time: 5358 ms

Testing Sequential BFS (5 runs)
  Run 1: 7673 ms
  Run 2: 7203 ms
  Run 3: 6244 ms
  Run 4: 6672 ms
  Run 5: 7161 ms
Average sequential time: 6990 ms

Testing Parallel BFS (5 runs)
  Run 1: 2236 ms
  Run 2: 2068 ms
  Run 3: 2126 ms
  Run 4: 2225 ms
  Run 5: 2048 ms
Average parallel time: 2140 ms

PERFORMANCE RESULTS
Sequential BFS: 6990 ms
Parallel BFS:   2140 ms
Speedup: 3.27x

TEST SUITE COMPLETE

```