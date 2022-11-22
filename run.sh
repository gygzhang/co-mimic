#! /bin/bash
g++ coroutine.h coroutine_test.h coroutine.cpp main.cpp -o coroutine_test --std=c++17
g++ pthread_test.cpp -o pthread_test -pthread --std=c++17

time ./coroutine_test
time ./pthread_test



