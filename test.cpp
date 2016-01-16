#include <cassert>
#include <iostream>
#include "ring.hpp"

template <typename InputIterator>
bool array_eq(InputIterator arr1,
              size_t size1,
              InputIterator arr2,
              size_t size2) {
  if (size1 != size2) { return false; }
  for (size_t i = 0; i < size1; ++i) {
    if (arr1[i] != arr2[i]) { return false; }
  }
  return true;
}

template <typename InputIterator>
void print_array(InputIterator arr, size_t size) {
  std::cerr << '[';
  for (size_t i = 0; i < size; ++i) {
    std::cerr << arr[i];
    if (i < size - 1) { std::cerr << ','; }
  }
  std::cerr << ']' << std::endl;
}

using gRin::ring_queue;

void resize_test() {}

int main() {
  resize_test();
  ring_queue<int> r;
  int test_arr_in[] = {1, 2, 3};
  r.push_range(test_arr_in, 3);
  r.resize(400);
  assert(r.size() == 3);
  int test_arr_out[3];
  const ring_queue<int> s(r);
  assert(s.peek_range(test_arr_out, 3) == 3);
  assert(s.size() == 3);
  assert(array_eq(test_arr_out, 3, test_arr_in, 3));
  assert(array_eq(test_arr_out, 3, test_arr_in, 3));
  assert(r.pull_range(test_arr_out, 3) == 3);
  assert(r.size() == 0);
}
