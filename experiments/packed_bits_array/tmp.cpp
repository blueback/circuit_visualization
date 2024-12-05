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

typedef struct __attribute__((packed)) {
  union __attribute__((packed)) {
    uint8_t E[17];
    struct __attribute__((packed)) {
      uint64_t E1 : 17;
      uint64_t E2 : 17;
      uint64_t E3 : 17;
      uint64_t E4 : 17;
      uint64_t E5 : 17;
      uint64_t E6 : 17;
      uint64_t E7 : 17;
      uint64_t E8 : 17;
    } s;
  } u;
} packed17_t;

static_assert(sizeof(packed17_t) == 17);

uint16_t *array_16 = nullptr;
packed17_t *array_17 = nullptr;

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

#define SZ (1lu << 28)

__attribute__((noinline)) void fill(uint16_t *arr) {
  for (size_t i = 0; i < SZ; i++) {
    arr[i] = i % 256;
  }
}

__attribute__((noinline)) void fill(packed17_t *arr) {
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr[j].u.s.E1 = i % 256;
    arr[j].u.s.E2 = (i + 1) % 256;
    arr[j].u.s.E3 = (i + 2) % 256;
    arr[j].u.s.E4 = (i + 3) % 256;
    arr[j].u.s.E5 = (i + 4) % 256;
    arr[j].u.s.E6 = (i + 5) % 256;
    arr[j].u.s.E7 = (i + 6) % 256;
    arr[j].u.s.E8 = (i + 7) % 256;
  }
}

__attribute__((noinline)) void copyarrINC01(uint16_t *arr, packed17_t *arr2) {
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E1 = arr[i];
    arr2[j].u.s.E2 = arr[i + 1];
    arr2[j].u.s.E3 = arr[i + 2];
    arr2[j].u.s.E4 = arr[i + 3];
    arr2[j].u.s.E5 = arr[i + 4];
    arr2[j].u.s.E6 = arr[i + 5];
    arr2[j].u.s.E7 = arr[i + 6];
    arr2[j].u.s.E8 = arr[i + 7];
  }
}

__attribute__((noinline)) void copyarrINC02(uint16_t *arr, packed17_t *arr2) {
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E1 = arr[i];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E2 = arr[i + 1];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E3 = arr[i + 2];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E4 = arr[i + 3];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E5 = arr[i + 4];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E6 = arr[i + 5];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E7 = arr[i + 6];
  }
  for (size_t i = 0, j = 0; i < SZ; i += 8, j++) {
    arr2[j].u.s.E8 = arr[i + 7];
  }
}

__attribute__((noinline)) void copyarrINC03(uint16_t *arr, packed17_t *arr2) {
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E1 = arr[i];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E2 = arr[i + 1];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E3 = arr[i + 2];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E4 = arr[i + 3];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E5 = arr[i + 4];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E6 = arr[i + 5];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E7 = arr[i + 6];
  }
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[i >> 3llu].u.s.E8 = arr[i + 7];
  }
}

__attribute__((noinline)) void copyarrINC04(uint16_t *arr, packed17_t *arr2) {
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E1 = arr[i << 3llu];
    arr2[i + 1].u.s.E1 = arr[(i + 1) << 3llu];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E2 = arr[(i << 3llu) + 1];
    arr2[i + 1].u.s.E2 = arr[((i + 1) << 3llu) + 1];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E3 = arr[(i << 3llu) + 2];
    arr2[i + 1].u.s.E3 = arr[((i + 1) << 3llu) + 2];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E4 = arr[(i << 3llu) + 3];
    arr2[i + 1].u.s.E4 = arr[((i + 1) << 3llu) + 3];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E5 = arr[(i << 3llu) + 4];
    arr2[i + 1].u.s.E5 = arr[((i + 1) << 3llu) + 4];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E6 = arr[(i << 3llu) + 5];
    arr2[i + 1].u.s.E6 = arr[((i + 1) << 3llu) + 5];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E7 = arr[(i << 3llu) + 6];
    arr2[i + 1].u.s.E7 = arr[((i + 1) << 3llu) + 6];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E8 = arr[(i << 3llu) + 7];
    arr2[i + 1].u.s.E8 = arr[((i + 1) << 3llu) + 7];
  }
}

__attribute__((noinline)) void copyarrINC05(uint16_t *arr, packed17_t *arr2) {
  for (size_t i = 0; i < (SZ >> 3llu); i += 4) {
    arr2[i].u.s.E1 = arr[i << 3llu];
    arr2[i + 1].u.s.E1 = arr[(i + 1) << 3llu];
    arr2[i + 2].u.s.E1 = arr[(i + 2) << 3llu];
    arr2[i + 3].u.s.E1 = arr[(i + 3) << 3llu];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E2 = arr[(i << 3llu) + 1];
    arr2[i + 1].u.s.E2 = arr[((i + 1) << 3llu) + 1];
    arr2[i + 2].u.s.E2 = arr[((i + 2) << 3llu) + 1];
    arr2[i + 3].u.s.E2 = arr[((i + 3) << 3llu) + 1];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E3 = arr[(i << 3llu) + 2];
    arr2[i + 1].u.s.E3 = arr[((i + 1) << 3llu) + 2];
    arr2[i + 2].u.s.E3 = arr[((i + 2) << 3llu) + 2];
    arr2[i + 3].u.s.E3 = arr[((i + 3) << 3llu) + 2];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E4 = arr[(i << 3llu) + 3];
    arr2[i + 1].u.s.E4 = arr[((i + 1) << 3llu) + 3];
    arr2[i + 2].u.s.E4 = arr[((i + 2) << 3llu) + 3];
    arr2[i + 3].u.s.E4 = arr[((i + 3) << 3llu) + 3];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E5 = arr[(i << 3llu) + 4];
    arr2[i + 1].u.s.E5 = arr[((i + 1) << 3llu) + 4];
    arr2[i + 2].u.s.E5 = arr[((i + 2) << 3llu) + 4];
    arr2[i + 3].u.s.E5 = arr[((i + 3) << 3llu) + 4];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E6 = arr[(i << 3llu) + 5];
    arr2[i + 1].u.s.E6 = arr[((i + 1) << 3llu) + 5];
    arr2[i + 2].u.s.E6 = arr[((i + 2) << 3llu) + 5];
    arr2[i + 3].u.s.E6 = arr[((i + 3) << 3llu) + 5];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E7 = arr[(i << 3llu) + 6];
    arr2[i + 1].u.s.E7 = arr[((i + 1) << 3llu) + 6];
    arr2[i + 2].u.s.E7 = arr[((i + 2) << 3llu) + 6];
    arr2[i + 3].u.s.E7 = arr[((i + 3) << 3llu) + 6];
  }
  for (size_t i = 0; i < (SZ >> 3llu); i += 2) {
    arr2[i].u.s.E8 = arr[(i << 3llu) + 7];
    arr2[i + 1].u.s.E8 = arr[((i + 1) << 3llu) + 7];
    arr2[i + 2].u.s.E8 = arr[((i + 2) << 3llu) + 7];
    arr2[i + 3].u.s.E8 = arr[((i + 3) << 3llu) + 7];
  }
}

#if PACKED_BLOCK_STRID
void pack_16bit_to_17bit_block_strided_vectorized(const uint16_t *src,
                                                  uint8_t *dest,
                                                  size_t num_blocks) {
  const size_t INPUT_BLOCK_SIZE = 512;  // 256 uint16_t elements
  const size_t OUTPUT_BLOCK_SIZE = 544; // Packed 17-bit output size

  // Process 16 blocks at a time
  for (size_t block = 0; block < num_blocks; block += 16) {
    // Load 16 uint16_t values from 16 blocks
    __m256i input = _mm256_set_epi16(src[(block + 15) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 14) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 13) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 12) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 11) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 10) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 9) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 8) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 7) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 6) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 5) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 4) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 3) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 2) * (INPUT_BLOCK_SIZE / 2)],
                                     src[(block + 1) * (INPUT_BLOCK_SIZE / 2)],
                                     src[block * (INPUT_BLOCK_SIZE / 2)]);

    dest[(block + 1) * (OUTPUT_BLOCK_SIZE) + 1];
    dest[(block + 1) * (OUTPUT_BLOCK_SIZE)];
    dest[(block) * (OUTPUT_BLOCK_SIZE) + 1];
    dest[(block) * (OUTPUT_BLOCK_SIZE)];
    // Mask values to 17-bit (if needed)
    __m256i masked = _mm256_and_si256(input, _mm256_set1_epi32(0x1FFFF));

    // Compute destination indices
    // Write the packed 17-bit results (bit manipulation required here)
    // [Packing logic similar to the scalar implementation]
  }
}
#endif // PACKED_BLOCK_STRID

size_t __attribute__((noinline)) access_speed_test(uint16_t *array16,
                                                   const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < 25; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array16[i]);
    }
  }
  return sum;
}

size_t __attribute__((noinline)) access_speed_test(packed17_t *array17,
                                                   const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < 25; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E1);
      sum = (sum + array17[i].u.s.E2);
      sum = (sum + array17[i].u.s.E3);
      sum = (sum + array17[i].u.s.E4);
      sum = (sum + array17[i].u.s.E5);
      sum = (sum + array17[i].u.s.E6);
      sum = (sum + array17[i].u.s.E7);
      sum = (sum + array17[i].u.s.E8);
    }
  }
  return sum;
}

size_t __attribute__((noinline)) access_speed_test(uint32_t *array32,
                                                   const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < 25; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array32[i]);
    }
  }
  return sum;
}

__attribute__((noinline)) void memcpy_16_to_32_01(const uint16_t *src,
                                                  uint32_t *dst, size_t count) {
  for (size_t i = 0; i < count; i++) {
    dst[i] = src[i];
  }
}

int main() {
  uint16_t *arr = static_cast<uint16_t *>(malloc(SZ * sizeof(uint16_t)));
  fill(arr);
  // for (size_t i = 120; i < 150; i++) {
  //   printf("arr[%lu] = %hhu\n", i, arr[i]);
  // }
#if PACKED17_COPY
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
    copyarrINC01(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
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
  {
    assert((SZ & 7llu) == 0);
    packed17_t *arr_dest =
        static_cast<packed17_t *>(malloc(sizeof(packed17_t) * (SZ >> 3)));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC02(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
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
  {
    assert((SZ & 7llu) == 0);
    packed17_t *arr_dest =
        static_cast<packed17_t *>(malloc(sizeof(packed17_t) * (SZ >> 3)));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC03(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
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
  {
    assert((SZ & 7llu) == 0);
    packed17_t *arr_dest =
        static_cast<packed17_t *>(malloc(sizeof(packed17_t) * (SZ >> 3)));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC04(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
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
#endif
  {
    assert((SZ & 7llu) == 0);
    packed17_t *arr_dest =
        static_cast<packed17_t *>(malloc(sizeof(packed17_t) * (SZ >> 3)));
    {
      auto start = std::chrono::high_resolution_clock::now();
      copyarrINC01(arr, arr_dest);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu\n", duration.count());
    }
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
    {
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test(arr, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %lu\n", duration.count(), sum);
    }
    {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test(arr_dest, SZ >> 3llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  {
    uint32_t *arr_dest = static_cast<uint32_t *>(malloc(sizeof(uint32_t) * SZ));
    {
      auto start = std::chrono::high_resolution_clock::now();
      memcpy_16_to_32_01(arr, arr_dest, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu\n", duration.count());
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[i]);
    }
    {
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test(arr, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %lu\n", duration.count(), sum);
    }
    {
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test(arr_dest, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  return 0;
}
