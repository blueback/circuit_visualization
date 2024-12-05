#include "stdint.h"
#include "stdio.h"
#include "x86intrin.h"
#include <assert.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC target("avx2")
#pragma GCC optimize("O3")

void *aligned_malloc(size_t size, size_t alignment) {
  void *ptr = NULL;
  if (posix_memalign(&ptr, alignment, size) != 0) {
    return NULL; // Allocation failed
  }
  return ptr;
}

void *aligned_malloc_multiple(size_t size, size_t alignment) {
  assert(size == ((size + alignment - 1) / alignment * alignment));
  return aligned_alloc(alignment,
                       (size + alignment - 1) / alignment * alignment);
}

#define SZ (1lu << 20)

__attribute__((noinline)) void fill(uint16_t *arr) {
  for (size_t i = 0; i < SZ; i++) {
    arr[i] = i % 256;
  }
}

void block_copy_avx512(const uint16_t *src, uint8_t *dest, size_t num_blocks) {
  const size_t SRC_BLOCK_SIZE = 1024;  // 512 uint16_t elements
  const size_t DEST_BLOCK_SIZE = 1088; // 512 packed 17-bit integers

  // Mask for gather/scatter (all active for 16 elements)
  __mmask16 mask = 0xFFFF;

  // Scale factor for gather/scatter (2 bytes per uint16_t in source)
  const int scale = 2;

  // Loop over each block
  for (size_t block = 0; block < num_blocks; ++block) {
    // Base addresses for this block
    const uint16_t *src_base = src + block * (SRC_BLOCK_SIZE / 2);
    uint8_t *dest_base = dest + block * DEST_BLOCK_SIZE;

    // Gather 16 uint16_t values from the source
    __m512i indices =
        _mm512_set_epi32(15 * (SRC_BLOCK_SIZE / 2), 14 * (SRC_BLOCK_SIZE / 2),
                         13 * (SRC_BLOCK_SIZE / 2), 12 * (SRC_BLOCK_SIZE / 2),
                         11 * (SRC_BLOCK_SIZE / 2), 10 * (SRC_BLOCK_SIZE / 2),
                         9 * (SRC_BLOCK_SIZE / 2), 8 * (SRC_BLOCK_SIZE / 2),
                         7 * (SRC_BLOCK_SIZE / 2), 6 * (SRC_BLOCK_SIZE / 2),
                         5 * (SRC_BLOCK_SIZE / 2), 4 * (SRC_BLOCK_SIZE / 2),
                         3 * (SRC_BLOCK_SIZE / 2), 2 * (SRC_BLOCK_SIZE / 2),
                         1 * (SRC_BLOCK_SIZE / 2), 0 * (SRC_BLOCK_SIZE / 2));

    __m512i gathered = _mm512_i32gather_epi32(indices, src_base, scale);

    // Convert 16-bit integers to 17-bit integers
    // AVX-512 does not have 17-bit integer support, so manually mask
    __m512i packed = _mm512_and_epi32(gathered, _mm512_set1_epi32(0x1FFFF));

    // Scatter 17-bit packed integers into the destination
    _mm512_i32scatter_epi32(dest_base, indices, packed, scale);
  }
}

int main() {
  uint16_t *arr = static_cast<uint16_t *>(malloc(SZ * sizeof(uint16_t)));
  fill(arr);
  // for (size_t i = 120; i < 150; i++) {
  //   printf("arr[%lu] = %hhu\n", i, arr[i]);
  // }
  {
    assert((SZ & 7llu) == 0);
    uint16_t *arr_dest = static_cast<uint16_t *>(malloc(sizeof(uint16_t) * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    memcpy(arr_dest, arr, SZ * sizeof(uint16_t));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[i]);
    }
  }
  {
    assert((SZ & 7llu) == 0);
    packed17_t *arr_dest =
        static_cast<packed17_t *>(malloc(sizeof(packed17_t) * (SZ >> 3)));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC05(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time5:%lu\n", duration.count());
    for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
      assert(arr[i] == arr_dest[j].u.s.E1);
      assert(arr[i + 1] == arr_dest[j].u.s.E2);
      assert(arr[i + 2] == arr_dest[j].u.s.E3);
      assert(arr[i + 3] == arr_dest[j].u.s.E4);
      assert(arr[i + 4] == arr_dest[j].u.s.E5);
      assert(arr[i + 5] == arr_dest[j].u.s.E6);
      assert(arr[i + 6] == arr_dest[j].u.s.E7);
      assert(arr[i + 7] == arr_dest[j].u.s.E8);
    }
  }
  return 0;
}

