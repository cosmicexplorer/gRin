#ifndef ___RING_BUF_TPP___
#define ___RING_BUF_TPP___

#include <algorithm>

namespace gRin {
template <typename T>
size_t ring_queue<T>::calc_new_max_size(size_t cur_size, size_t must_hold) {
  if (0 == cur_size) { return must_hold; }
  size_t newmax = (size_t)(cur_size * GROWTH_FACTOR);
  if (newmax < must_hold) { return must_hold; }
  return newmax;
}

template <typename T>
ring_queue<T>::ring_queue(size_t size)
    : ring(new T[size]), max(size), bot(0), top(0), empty(true) {}

template <typename T>
ring_queue<T>::~ring_queue() {
  delete[] ring;
}

template <typename T>
ring_queue<T>::ring_queue(const ring_queue & other)
    : ring(nullptr), max(0), bot(0), top(0), empty(true) {
  size_t other_size = other.size();
  if (other_size > 0) {
    ring = new T[other_size];
    max = other_size;
    other.peek_range(ring, other_size);
    empty = false;
  }
}

/* always resets the bottom of ring to 0 when reallocating */
template <typename T>
template <typename InputIterator>
void ring_queue<T>::push_range(InputIterator in, size_t num) {
  if (num == 0) { return; }
  /* if empty, can assume bot, top == 0 */
  if (empty) {
    /* number of free elems == max */
    if (max >= num) {
      std::copy_n(in, num, ring);
      top = num;
    } else {
      /* max < num (reallocate) */
      resize(max + num);
      push_range(in, num);
    }
  } else {
    if (bot < top) {
      size_t free_at_top = max - top;
      if (free_at_top >= num) {
        std::copy_n(in, num, ring + top);
        top += num;
      } else {
        /* free_at_top < num */
        size_t free_elems = max - free_at_top + bot;
        size_t old_size = top - bot;
        if (free_elems >= num) {
          /* wraparound */
          std::copy_n(in, free_at_top, ring + top);
          std::copy_n(in + free_at_top, num - free_at_top, ring);
          top = num - free_at_top;
        } else {
          /* free_elems < num (reallocate) */
          resize(old_size + num);
          push_range(in, num);
        }
      }
    } else {
      /* bot >= top */
      size_t free_elems = bot - top;
      if (free_elems >= num) {
        std::copy_n(in, num, ring + top);
        top += num;
      } else {
        /* free_elems < num (reallocate) */
        size_t old_size = max - bot - top;
        resize(old_size + num);
        push_range(in, num);
      }
    }
  }
  empty = false;
}

template <typename T>
template <typename OutputIterator>
size_t ring_queue<T>::pull_range(OutputIterator out, size_t num) {
  if (0 == num) { return 0; }
  if (empty) { return 0; }
  if (bot < top) {
    size_t old_size = top - bot;
    if (old_size < num) { num = old_size; }
    std::copy_n(ring + bot, num, out);
    bot += num;
    if (bot == top) {
      bot = top = 0;
      empty = true;
    }
    return num;
  } else {
    size_t old_size    = max - bot - top;
    size_t used_at_top = max - bot;
    if (old_size < num) { num = old_size; }
    /* copy from top */
    std::copy_n(ring + bot, used_at_top, out);
    /* copy from bottom */
    std::copy_n(ring, top, out + used_at_top);
    bot = (bot + num) % max;
    if (bot == top) {
      bot = top = 0;
      empty = true;
    }
    return num;
  }
}

/* same as pull_range, without modifying member variables. unfortunate code
   duplication */
template <typename T>
template <typename OutputIterator>
size_t ring_queue<T>::peek_range(OutputIterator out, size_t num) const {
  if (0 == num) { return 0; }
  if (empty) { return 0; }
  if (bot < top) {
    size_t old_size = top - bot;
    if (old_size < num) { num = old_size; }
    std::copy_n(ring + bot, num, out);
    return num;
  } else {
    size_t old_size    = max - bot - top;
    size_t used_at_top = max - bot;
    if (old_size < num) { num = old_size; }
    /* copy from top */
    std::copy_n(ring + bot, used_at_top, out);
    /* copy from bottom */
    std::copy_n(ring, top, out + used_at_top);
    return num;
  }
}

template <typename T>
size_t ring_queue<T>::size() const noexcept {
  if (empty) { return 0; }
  if (bot < top) { return top - bot; }
  return max - bot - top;
}

/* resets bot to 0 */
template <typename T>
size_t ring_queue<T>::resize(size_t fin) {
  if (fin <= max) { return max; }
  size_t new_max  = calc_new_max_size(max, fin);
  size_t old_size = size();
  T * new_ring = new T[new_max];
  if (bot < top) {
    std::copy_n(ring + bot, top - bot, new_ring);
  } else {
    size_t used_at_top = max - bot;
    std::copy_n(ring + bot, used_at_top, new_ring);
    std::copy_n(ring, top, new_ring + used_at_top);
  }
  std::swap(ring, new_ring);
  max = new_max;
  bot = 0;
  top = old_size;
  delete[] new_ring;
  return max;
}

bool ring_queue::empty() const noexcept { return empty; }
}

#endif /* ___RING_BUF_TPP___ */
