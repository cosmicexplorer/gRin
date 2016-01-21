#ifndef ___RING_BUF_TPP___
#define ___RING_BUF_TPP___

#include <algorithm>
#include <string.h>

namespace gRin {
template <typename T>
size_t ring_queue<T>::calc_new_max_size(size_t cur_size, size_t must_hold) {
  if (0 == cur_size) { return must_hold; }
  size_t newmax = (size_t)((double) (cur_size) *GROWTH_FACTOR);
  if (newmax < must_hold) { return must_hold; }
  return newmax;
}

template <typename T>
ring_queue<T>::ring_queue(size_t size)
    : ring(new T[size]), max(size), bot(0), top(0), is_empty(true) {}

template <typename T>
ring_queue<T>::~ring_queue() {
  delete[] ring;
}

template <typename T>
ring_queue<T>::ring_queue(const ring_queue<T> & other)
    : ring(nullptr), max(0), bot(0), top(0), is_empty(true) {
  size_t other_size = other.size();
  if (other_size > 0) {
    ring = new T[other_size];
    max = other_size;
    other.peek_range(ring, other_size);
    is_empty = false;
  }
}

template <typename T>
ring_queue<T> & ring_queue<T>::operator=(const ring_queue<T> & rhs) {
  if (this != &rhs) {
    ring_queue<T> copy(rhs);
    swap(copy);
  }
  return *this;
}

template <typename T>
void ring_queue<T>::swap(ring_queue<T> & rhs) noexcept {
  std::swap(ring, rhs.ring);
  std::swap(max, rhs.max);
  std::swap(bot, rhs.bot);
  std::swap(top, rhs.top);
  std::swap(is_empty, rhs.is_empty);
}

/* TODO: use template specialization to accept arbitrary iterators in most
   cases, but memmove on pointers, and memcpy on restrict pointers? */
template <typename T>
void ring_queue<T>::push_range(const T * __restrict__ in, size_t num) {
  size_t num_bytes = num * sizeof(T);
  if (0 == num) { return; }
  /* if is_empty, can assume bot, top == 0 */
  if (is_empty) {
    /* number of free elems == max */
    if (max >= num) {
      memcpy(ring, in, num_bytes);
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
        memcpy(ring + top * sizeof(T), in, num_bytes);
        top = (top + num) % max;
      } else {
        /* free_at_top < num */
        size_t free_elems = free_at_top + bot;
        size_t free_at_top_bytes = free_at_top * sizeof(T);
        size_t old_size = top - bot;
        if (free_elems >= num) {
          /* wraparound */
          memcpy(ring + top * sizeof(T), in, free_at_top_bytes);
          memcpy(ring, in + free_at_top_bytes, num_bytes - free_at_top_bytes);
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
        memcpy(ring + top * sizeof(T), in, num_bytes);
        top = (top + num) % max;
      } else {
        /* free_elems < num (reallocate) */
        size_t old_size = max - (bot - top);
        resize(old_size + num);
        push_range(in, num);
      }
    }
  }
  is_empty = false;
}

template <typename T>
size_t ring_queue<T>::pull_range(T * __restrict__ out, size_t num) {
  size_t num_bytes = num * sizeof(T);
  if (0 == num) { return 0; }
  if (is_empty) { return 0; }
  if (bot < top) {
    size_t old_size = top - bot;
    if (old_size < num) { num = old_size; }
    memcpy(out, ring + bot * sizeof(T), num_bytes);
    bot += num;
    if (bot == top) {
      bot = top = 0;
      is_empty = true;
    }
    return num;
  } else {
    size_t used_at_top = max - bot, used_at_top_bytes = used_at_top * sizeof(T);
    size_t old_size = used_at_top + top;
    if (old_size < num) { num = old_size; }
    if (used_at_top >= num) {
      memcpy(out, ring + bot * sizeof(T), num_bytes);
      bot = (bot + num) % max;
    } else {
      /* used_at_top < num */
      /* copy from top */
      memcpy(out, ring + bot * sizeof(T), used_at_top_bytes);
      /* copy from bottom */
      memcpy(out + used_at_top_bytes, ring, num_bytes - used_at_top_bytes);
      bot = (bot + num) % max;
      if (bot == top) {
        bot = top = 0;
        is_empty = true;
      }
    }
    return num;
  }
}

/* same as pull_range, without modifying member variables. unfortunate code
 duplication */
template <typename T>
size_t ring_queue<T>::peek_range(T * __restrict__ out, size_t num) const {
  size_t num_bytes = num * sizeof(T);
  if (0 == num) { return 0; }
  if (is_empty) { return 0; }
  if (bot < top) {
    size_t old_size = top - bot;
    if (old_size < num) { num = old_size; }
    memcpy(out, num_bytes, ring + bot * sizeof(T));
    return num;
  } else {
    size_t used_at_top = max - bot, used_at_top_bytes = used_at_top * sizeof(T);
    size_t old_size = used_at_top + top;
    if (old_size < num) { num = old_size; }
    if (used_at_top >= num) {
      memcpy(out, ring + bot * sizeof(T), num_bytes);
    } else {
      /* used_at_top < num */
      /* copy from top */
      memcpy(out, ring + bot * sizeof(T), used_at_top_bytes);
      /* copy from bottom */
      memcpy(out + used_at_top_bytes, ring, num_bytes - used_at_top_bytes);
    }
    return num;
  }
}

template <typename T>
size_t ring_queue<T>::size() const noexcept {
  if (is_empty) { return 0; }
  if (bot < top) { return top - bot; }
  return max - (bot - top);
}

/* resets bot to 0 */
template <typename T>
size_t ring_queue<T>::resize(size_t fin) {
  if (fin <= max) { return max; }
  size_t new_max            = calc_new_max_size(max, fin);
  size_t old_size           = size();
  T * __restrict__ new_ring = new T[new_max];
  if (!is_empty) {
    if (bot < top) {
      memcpy(new_ring, ring + bot * sizeof(T), (top - bot) * sizeof(T));
    } else {
      size_t used_at_top = max - bot, used_at_top_bytes = used_at_top * sizeof(T);
      memcpy(new_ring, ring + bot * sizeof(T), used_at_top_bytes);
      memcpy(new_ring + used_at_top_bytes, ring, top * sizeof(T));
    }
  }
  std::swap(ring, new_ring);
  max = new_max;
  bot = 0;
  top = old_size;
  delete[] new_ring;
  return max;
}

template <typename T>
bool ring_queue<T>::empty() const noexcept {
  return is_empty;
}

template <typename T>
void ring_queue<T>::push_back(const T & val) {
  push_range(&val, 1);
}
}

namespace std {
template <typename T>
void swap(gRin::ring_queue<T> & lhs, gRin::ring_queue<T> & rhs) noexcept {
  lhs.swap(rhs);
}
}

#endif /* ___RING_BUF_TPP___ */
