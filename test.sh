#!/bin/sh

clang++ test.cpp -std=c++14 -Wall -Wextra -Werror -g -O0 -o test && ./test
