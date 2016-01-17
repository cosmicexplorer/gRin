#ifndef ___RING_BUF_H___
#define ___RING_BUF_H___

namespace gRin {
template <typename T>
class ring_queue {
private:
  T * ring;
  size_t max;
  size_t bot;
  /* one past the end */
  size_t top;
  bool is_empty;

  inline size_t calc_new_max_size(size_t, size_t);

public:
  ring_queue(size_t = INITIAL_SIZE);
  ~ring_queue();
  explicit ring_queue(const ring_queue &);

  constexpr static size_t INITIAL_SIZE  = 1000;
  constexpr static double GROWTH_FACTOR = 1.5;

  /* need to know breadth of range inserted, so don't offer push_range without
     knowing how many elements need to be inserted */
  template <typename InputIterator>
  void push_range(InputIterator, size_t);

  template <typename OutputIterator>
  size_t pull_range(OutputIterator, size_t);
  template <typename OutputIterator>
  size_t peek_range(OutputIterator, size_t) const;

  inline size_t size() const noexcept;
  size_t resize(size_t);
  inline bool empty() const noexcept;
};
}

#include "gRin.tpp"

#endif /* ___RING_BUF_H___ */
