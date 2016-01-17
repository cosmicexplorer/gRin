#!/bin/sh

echo clang++ test.cpp -std=c++14 -Wall -Wextra -Werror -g -O0 -o test
clang++ test.cpp -std=c++14 -Wall -Wextra -Werror -g -O0 -o test
echo ./test
./test
