#define private public

#include <cassert>
#include <iostream>
#include "gRin.hpp"

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
  for (size_t i = 0; i < size; ++i, ++arr) {
    std::cerr << *arr;
    if (i < size - 1) { std::cerr << ','; }
  }
  std::cerr << ']' << std::endl;
}

using gRin::ring_queue;

/* all these test functions will clobber the input */
template <typename T>
void make_bot_gt_top(ring_queue<T> & q) {
  if (q.bot < q.top) {
    size_t to_end = q.max - q.top;
    T * in = new T[to_end];
    q.push_range(in, to_end);
    delete[] in;
    static constexpr size_t from_bot = 3;
    /* this is for our own ease of writing; doesn't mean code is wrong */
    assert(q.top - q.bot > 2 * from_bot);
    T * out = new T[from_bot];
    q.pull_range(out, from_bot);
    q.pull_range(out, from_bot);
    q.push_range(out, from_bot);
    delete[] out;
    assert(q.bot > q.top);
  }
}

template <typename T>
void make_bot_lt_top(ring_queue<T> & q) {
  q.resize(q.size());
  assert(q.empty() or q.bot < q.top);
}

template <typename T>
void make_empty(ring_queue<T> & q) {
  T * out = new T[q.size()];
  q.pull_range(out, q.size());
  delete[] out;
  assert(q.empty() and q.size() == 0);
}

template <typename T>
void make_full(ring_queue<T> & q) {
  size_t diff = q.max - q.size();
  T * in = new T[diff];
  q.push_range(in, diff);
  delete[] in;
  assert(q.size() == q.max);
}

void push_pull_test() {
  ring_queue<int> r(10);
}

void peek_range_test() {
  ring_queue<int> r(10);
  assert(r.peek_range((int *) nullptr, 0) == 0);
  assert(r.peek_range((int *) nullptr, 1) == 0);
  int in[] = {1, 2, 3, 4, 5};
  int out[5];
  r.push_range(in, 5);
  assert(r.peek_range(out, 5) == 5);
  assert(array_eq(out, 5, in, 5));
  make_bot_gt_top(r);
  assert(r.peek_range(out, 5) == 5);
}

void size_resize_test() {
  ring_queue<int> r(3);
  assert(r.size() == 0);
  int in[] = {1, 2, 3};
  r.push_range(in, 3);
  r.pull_range(in, 1);
  r.push_range(in, 1);
  assert(r.bot >= r.top);
  r.resize(4);
  assert(r.size() == 3);
  assert(r.max >= 4);
  r.push_range(in, 1);
  int test_in[] = {2, 3, 1, 1};
  assert(array_eq(r.ring, 4, test_in, 4));
  r.pull_range(test_in, 4);
  assert(r.empty());
  assert(r.size() == 0);

  ring_queue<int> s;
  assert(s.size() == 0);
  assert(s.max == gRin::INITIAL_SIZE);
  s.push_range(in, 3);
  assert(s.size() == 3);
  s.resize(3e3);
  assert(s.max >= 3e3);
  assert(s.bot == 0);
  assert(s.top == 3);
}

void empty_test() {
  ring_queue<int> r;
  assert(r.empty());
  int in[] = {1, 2, 3};
  r.push_range(in, 3);
  assert(!r.empty());
  r.pull_range(in, 3);
  assert(r.empty());
}

int main() {
  empty_test();
  size_resize_test();
  peek_range_test();
  push_pull_test();
}
