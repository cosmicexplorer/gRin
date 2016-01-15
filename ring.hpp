#ifndef ___RING_BUF_H___
#define ___RING_BUF_H___

#include <stdexcept>
#include <memory>

namespace grin {
template <typename T>
class ring_queue {
private:
  T * ring;
  size_t max;
  size_t bot;
  /* one past the end */
  size_t top;
  bool empty;

  inline size_t calc_new_max_size(size_t, size_t);
  size_t pull_or_peek(T *, size_t, bool);

public:
  ring_queue(size_t = INITIAL_SIZE);
  ~ring_queue();

  constexpr static size_t INITIAL_SIZE  = 1000;
  constexpr static double GROWTH_FACTOR = 1.5;

  /* important new stuff */
  void push_range(T *, size_t);
  inline size_t pull_range(T *, size_t);
  inline size_t peek_range(T *, size_t);

  /* TODO: add basic stl stuff */
  size_t size() const;
  size_t resize(size_t);
};
}

#include "ring.tpp"

#endif /* ___RING_BUF_H___ */
