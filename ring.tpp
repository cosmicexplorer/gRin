#ifndef ___RING_BUF_TPP___
#define ___RING_BUF_TPP___

#include <cstring>
#include <algorithm>

namespace ring {
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

/* always resets the bottom of ring to 0 when reallocating */
template <typename T>
void ring_queue<T>::push_range(T * in, size_t num) {
  if (num == 0) { return; }
  /* if empty, can assume bot, top == 0 */
  if (empty) {
    /* number of free elems == max */
    if (max >= num) {
      memcpy(ring, in, num * sizeof(T));
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
        memcpy(ring + top, in, num * sizeof(T));
        top += num;
      } else {
        /* free_at_top < num */
        size_t free_elems = max - free_at_top + bot;
        size_t old_size = top - bot;
        if (free_elems >= num) {
          /* wraparound */
          memcpy(ring + top, in, free_at_top * sizeof(T));
          memcpy(ring, in + free_at_top, (num - free_at_top) * sizeof(T));
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
        memcpy(ring + top, in, num * sizeof(T));
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
size_t ring_queue<T>::pull_or_peek(T * out, size_t num, bool do_modify) {
  if (0 == num) { return 0; }
  if (empty) { return 0; }
  if (bot < top) {
    size_t old_size = top - bot;
    if (old_size < num) { num = old_size; }
    memcpy(out, ring + bot, num * sizeof(T));
    if (do_modify) {
      bot += num;
      if (bot == top) { empty = true; }
    }
    return num;
  } else {
    size_t old_size    = max - bot - top;
    size_t used_at_top = max - bot;
    if (old_size < num) { num = old_size; }
    /* copy from top */
    memcpy(out, ring + bot, used_at_top * sizeof(T));
    /* copy from bottom */
    memcpy(out + used_at_top, ring, top * sizeof(T));
    if (do_modify) {
      bot = (bot + num) % max;
      if (bot == top) { empty = true; }
    }
    return num;
  }
}

template <typename T>
size_t ring_queue<T>::pull_range(T * out, size_t num) {
  return pull_or_peek(out, num, true);
}

template <typename T>
size_t ring_queue<T>::peek_range(T * out, size_t num) {
  return pull_or_peek(out, num, false);
}

template <typename T>
size_t ring_queue<T>::size() const {
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
    memcpy(new_ring, ring + bot, (top - bot) * sizeof(T));
  } else {
    size_t used_at_top = max - bot;
    memcpy(new_ring, ring + bot, used_at_top * sizeof(T));
    memcpy(new_ring + used_at_top, ring, top * sizeof(T));
  }
  std::swap(ring, new_ring);
  max = new_max;
  bot = 0;
  top = old_size;
  delete[] new_ring;
  return max;
}
}

#endif /* ___RING_BUF_TPP___ */
