## Запуск:
```
SIMPLE CORRECTNESS TESTS
Results: 5/5 tests passed

RANDOM GRAPH TESTS
Results: 1000/1000 random graphs passed

SMALL CUBE TEST
Small cube test passed (125 vertices)

ALL CORRECTNESS TESTS PASSED!

PERFORMANCE TEST (300x300x300)

Creating 300x300x300 cube graph
Graph created: 27000000 vertices
Creation time: 10419 ms


Testing Sequential BFS (5 runs avg)
Avg time: 9886 ms

Testing Parallel BFS (5 runs avg)
Avg time: 2966 ms
PERFORMANCE RESULTS
Sequential BFS: 9886 ms
Parallel BFS:   2966 ms
Speedup: 3.33x

Quick correctness check on large graph
```

К сожалению, иногда бывает ускорение и в 5 раз(пока не знаю с чем связано)