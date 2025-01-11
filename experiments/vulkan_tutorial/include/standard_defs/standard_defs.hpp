#ifndef __STANDARD_DEFS_HPP__
#define __STANDARD_DEFS_HPP__

#include "standard_defs/standard_includes.hpp"

enum IteratorStatus : uint8_t { IterationBreak, IterationContinue };

#define TRY_NOMAGIC(X)                                                         \
  X(void) = delete;                                                            \
  X(const X &) = delete;                                                       \
  X(X &) = delete;                                                             \
  X(const X &&) = delete;                                                      \
  X(X &&) = delete;                                                            \
  const X &operator=(const X &) = delete;                                      \
  const X &operator=(X &) = delete;                                            \
  const X &operator=(const X &&) = delete;                                     \
  const X &operator=(X &&) = delete;                                           \
  void *operator new(size_t) = delete;                                         \
  void operator delete(void *p) = delete;                                      \
  void *operator new[](size_t) = delete;                                       \
  void operator delete[](void *p) = delete;

#define TRY_INLINE inline __attribute__((always_inline))
#define TRY_PACKED __attribute__((packed))
#define TRY_NOINLINE __attribute__((noinline))

void *operator new(std::size_t size);
void operator delete(void *ptr) throw();
void *operator new[](std::size_t size);
void operator delete[](void *ptr) throw();

TRY_INLINE void *aligned_malloc(size_t size, size_t alignment) {
  void *ptr = NULL;
  if (posix_memalign(&ptr, alignment, size) != 0) {
    return NULL; // Allocation failed
  }
  return ptr;
}

TRY_INLINE void *aligned_malloc_multiple(size_t size, size_t alignment) {
  assert(size == ((size + alignment - 1) / alignment * alignment));
  return aligned_alloc(alignment,
                       (size + alignment - 1) / alignment * alignment);
}

#endif // __STANDARD_DEFS_HPP__
