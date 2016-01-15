#include <cassert>
#include <iostream>
#include "ring.hpp"

template <typename T>
bool array_eq(T * arr1, size_t size1, T * arr2, size_t size2) {
  if (size1 != size2) { return false; }
  for (size_t i = 0; i < size1; ++i) {
    if (arr1[i] != arr2[i]) { return false; }
  }
  return true;
}

template <typename T>
void print_array(T * arr, size_t size) {
  std::cerr << '[';
  for (size_t i = 0; i < size; ++i) {
    std::cerr << arr[i];
    if (i < size - 1) { std::cerr << ','; }
  }
  std::cerr << ']' << std::endl;
}

int main() {
  ring::ring_queue<int> r;
  int test_arr_in[] = {1, 2, 3};
  r.push_range(test_arr_in, 3);
  assert(r.size() == 3);
  int test_arr_out[3];
  assert(r.peek_range(test_arr_out, 3) == 3);
  print_array(test_arr_out, 3);
  assert(array_eq(test_arr_out, 3, test_arr_in, 3));
}
