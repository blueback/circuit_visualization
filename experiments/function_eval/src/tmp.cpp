#include "stdint.h"
#include "stdio.h"
#include "x86intrin.h"
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

#define LAYERS (1llu << 15)
#define NODES_PER_LAYER (1llu << 7)
#define EDGES_PER_LAYER (NODES_PER_LAYER * NODES_PER_LAYER)
#define SZ_NODE_ARR (LAYERS * NODES_PER_LAYER)
#define SZ_EDGE_ARR ((LAYERS - 1) * EDGES_PER_LAYER * 2)

#define INPUT_EDGES_PER_LAYER (EDGES_PER_LAYER)
#define INPUT_EDGES ((LAYERS - 1) * INPUT_EDGES_PER_LAYER)

#define OUTPUT_EDGES_PER_LAYER (EDGES_PER_LAYER)
#define OUTPUT_EDGES ((LAYERS - 1) * OUTPUT_EDGES_PER_LAYER)
#define TOTAL_MEMORY_LIMIT (25.0)
#define NUM_OF_SCANS 10

////////////////////////////////////////////////////////////////////////////
///                                                                      ///
////////////////////////////////////////////////////////////////////////////
class TRY_PACKED Edge01 {
public:
  TRY_NOMAGIC(Edge01)

private:
  union TRY_PACKED {
    uint8_t E[3];
    struct TRY_PACKED {
      uint64_t second_node_index : 32;
      uint64_t left_edge_index_offset : 8;
      uint64_t right_edge_index_offset : 8;
    } s;
  } u;

public:
  TRY_INLINE void setSecondNodeIndex(const uint64_t second_node_index) {
    assert(second_node_index < UINT32_MAX);
    u.s.second_node_index = second_node_index;
  }

  TRY_INLINE void setLeftEdgeIndexOffset(const uint64_t offset) {
    assert(offset < UINT8_MAX);
    u.s.left_edge_index_offset = offset;
  }

  TRY_INLINE void setRightEdgeIndexOffset(const uint64_t offset) {
    assert(offset < UINT8_MAX);
    u.s.right_edge_index_offset = offset;
  }
  TRY_INLINE uint64_t getSecondNodeIndex(void) const {
    return u.s.second_node_index;
  }

  TRY_INLINE uint64_t getLeftEdgeIndexOffset(void) const {
    return u.s.left_edge_index_offset;
  }

  TRY_INLINE uint64_t getRightEdgeIndexOffset(void) const {
    return u.s.right_edge_index_offset;
  }
};

class TRY_PACKED Node01 {
public:
  TRY_NOMAGIC(Node01)

private:
  union TRY_PACKED {
    uint8_t E[3];
    struct TRY_PACKED {
      uint64_t input_count : 8;
      uint64_t input_edge_index : 32;
      uint64_t lut_function : 16;
      uint64_t output_count : 8;
      uint64_t output_edge_index : 32;
      uint64_t current_value : 8;
    } s;
  } u;

public:
  TRY_INLINE void setInputEdgeIndex(const uint64_t edge_index) {
    assert(edge_index < UINT32_MAX);
    u.s.input_edge_index = edge_index;
  }
  TRY_INLINE void setInputCount(const uint64_t count) {
    assert(count < UINT8_MAX);
    u.s.input_count = count;
  }

  TRY_INLINE void setLutFunction(const uint64_t f) {
    assert(f < UINT16_MAX);
    u.s.lut_function = f;
  }

  TRY_INLINE void setOutputEdgeIndex(const uint64_t edge_index) {
    assert(edge_index < UINT32_MAX);
    u.s.output_edge_index = edge_index;
  }
  TRY_INLINE void setOutputCount(const uint64_t count) {
    assert(count < UINT8_MAX);
    u.s.output_count = count;
  }

  TRY_INLINE void setCurrentValue(const uint64_t val) {
    assert(val < UINT8_MAX);
    u.s.current_value = val;
  }

  TRY_INLINE uint64_t getInputEdgeIndex(void) const {
    return u.s.input_edge_index;
  }
  TRY_INLINE uint64_t getInputCount(void) const { return u.s.input_count; }

  TRY_INLINE uint64_t getLutFunction(void) const { return u.s.lut_function; }

  TRY_INLINE uint64_t getOutputEdgeIndex(void) const {
    return u.s.output_edge_index;
  }
  TRY_INLINE uint64_t getOutputCount(void) const { return u.s.output_count; }

  TRY_INLINE uint64_t getCurrentValue(void) const { return u.s.current_value; }
};

static_assert(sizeof(Edge01) == 6);
static_assert(sizeof(Node01) == 13);

Node01 *node01_arr = nullptr;
Edge01 *edge01_arr = nullptr;

TRY_NOINLINE void allocate_arrays01(void) {
  const size_t node_arr_sz = SZ_NODE_ARR * sizeof(Node01);
  const size_t edge_arr_sz = SZ_EDGE_ARR * sizeof(Edge01);
  const size_t total_sz = node_arr_sz + edge_arr_sz;
  const float total_sz_float = 1.0 * total_sz;
  const float total_sz_in_gb = total_sz_float / (1llu << 30);
  printf("node_arr_sz = %lu\n", node_arr_sz);
  printf("edge_arr_sz = %lu\n", edge_arr_sz);
  printf("total = %lu Bytes\n", total_sz);
  printf("      = %.2f Gb\n", total_sz_in_gb);
  if (total_sz_in_gb > TOTAL_MEMORY_LIMIT) {
    printf("ERROR: not enough memory! (limit : %.2f Gb)\n", TOTAL_MEMORY_LIMIT);
    exit(1);
    return;
  }
  node01_arr = static_cast<Node01 *>(aligned_malloc_multiple(node_arr_sz, 32));
  edge01_arr = static_cast<Edge01 *>(aligned_malloc_multiple(edge_arr_sz, 32));
}

TRY_NOINLINE void fill_arrays01(void) {
  for (size_t l = 0; l < LAYERS; l++) {
    for (size_t n = 0; n < NODES_PER_LAYER; n++) {
      const size_t node_index = l * NODES_PER_LAYER + n;
      node01_arr[node_index].setCurrentValue(0);
      if (l > 0) {
        const size_t root_input_edge_index = node_index * NODES_PER_LAYER;

        node01_arr[node_index].setInputCount(NODES_PER_LAYER);
        node01_arr[node_index].setInputEdgeIndex(root_input_edge_index);

        for (size_t i = 0; i < NODES_PER_LAYER; i++) {
          const size_t input_edge_index = root_input_edge_index + i;
          const size_t second_node_index = (l - 1) * NODES_PER_LAYER + i;
          edge01_arr[input_edge_index].setSecondNodeIndex(second_node_index);
          edge01_arr[input_edge_index].setLeftEdgeIndexOffset(0);
          edge01_arr[input_edge_index].setRightEdgeIndexOffset(0);
        }
      } else {
        node01_arr[node_index].setInputCount(0);
        node01_arr[node_index].setInputEdgeIndex(0);
      }
      if (l < LAYERS - 1) {
        const size_t root_output_edge_index =
            INPUT_EDGES + node_index * NODES_PER_LAYER;

        node01_arr[node_index].setOutputCount(NODES_PER_LAYER);
        node01_arr[node_index].setOutputEdgeIndex(root_output_edge_index);

        for (size_t i = 0; i < NODES_PER_LAYER; i++) {
          const size_t input_edge_index = root_output_edge_index + i;
          const size_t second_node_index = (l + 1) * NODES_PER_LAYER + i;
          edge01_arr[input_edge_index].setSecondNodeIndex(second_node_index);
          edge01_arr[input_edge_index].setLeftEdgeIndexOffset(0);
          edge01_arr[input_edge_index].setRightEdgeIndexOffset(0);
        }
      } else {
        node01_arr[node_index].setOutputCount(0);
        node01_arr[node_index].setOutputEdgeIndex(0);
      }
    }
  }
}

TRY_INLINE void eval_node_01(const size_t n) {
  const size_t input_count = node01_arr[n].getInputCount();
  const size_t input_edge_index = node01_arr[n].getInputEdgeIndex();
  size_t input_val(0);
  for (size_t i = 0; i < input_count; i++) {
    const size_t second_node_index =
        edge01_arr[input_edge_index + i].getSecondNodeIndex();
    const size_t val = node01_arr[second_node_index].getCurrentValue();
    assert(val == 1 || val == 0);
    input_val = input_val | (val << i);
  }
  const size_t lut_function = node01_arr[n].getLutFunction();
  const size_t new_val = lut_function | (1llu << input_val);
  node01_arr[n].setCurrentValue(new_val ? 1 : 0);
}

TRY_NOINLINE void execute_01(uint64_t iter) {
  for (size_t n = 0; n < NODES_PER_LAYER; n++) {
    node01_arr[n].setCurrentValue((iter + n) % 2);
  }

  for (size_t l = 0; l < LAYERS; l++) {
    for (size_t n = 0; n < NODES_PER_LAYER; n++) {
      const size_t node_index = l * NODES_PER_LAYER + n;
      eval_node_01(node_index);
    }
  }
}
////////////////////////////////////////////////////////////////////////////
///                                                                      ///
////////////////////////////////////////////////////////////////////////////

int main() {
  {
    auto start = std::chrono::high_resolution_clock::now();
    allocate_arrays01();
    fill_arrays01();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time AllocateAndFill : %lu\n", duration.count());
  }
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < NUM_OF_SCANS; i++) {
      execute_01(i);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time Execute : %lu\n", duration.count());
  }
  return 0;
}
