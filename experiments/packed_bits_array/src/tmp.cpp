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

// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

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

typedef struct __attribute__((packed)) {
  union __attribute__((packed)) {
    uint8_t E[9];
    struct __attribute__((packed)) {
      uint64_t E1 : 9;
      uint64_t E2 : 9;
      uint64_t E3 : 9;
      uint64_t E4 : 9;
      uint64_t E5 : 9;
      uint64_t E6 : 9;
      uint64_t E7 : 9;
      uint64_t E8 : 9;
    } s;
  } u;
} packed9_t;

typedef struct __attribute__((packed)) {
  union __attribute__((packed)) {
    uint8_t E[3];
    struct __attribute__((packed)) {
      uint64_t E1 : 12;
      uint64_t E2 : 12;
    } s;
  } u;
} packed12_t;

static_assert(sizeof(packed17_t) == 17);
static_assert(sizeof(packed9_t) == 9);
static_assert(sizeof(packed12_t) == 3);

uint16_t *array_16 = nullptr;
packed17_t *array_17 = nullptr;
packed9_t *array_9 = nullptr;

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

#define NUM_OF_SCANS 10

size_t __attribute__((noinline)) access_speed_test(uint16_t *array16,
                                                   const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array16[i]);
    }
  }
  return sum;
}

size_t __attribute__((noinline)) access_speed_test01(packed17_t *array17,
                                                     const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
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

size_t __attribute__((noinline)) access_speed_test02(packed17_t *array17,
                                                     const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E1);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E2);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E3);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E4);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E5);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E6);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E7);
    }
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array17[i].u.s.E8);
    }
  }
  return sum;
}

size_t __attribute__((noinline)) access_speed_test(uint32_t *array32,
                                                   const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
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

__attribute__((noinline)) void memcpy_16_to_9_01(uint16_t *arr,
                                                 packed9_t *arr2) {
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

__attribute__((noinline)) void memcpy_16_to_8_01(const uint16_t *src,
                                                 uint8_t *dst, size_t count) {
  for (size_t i = 0; i < count; i++) {
    dst[i] = src[i];
  }
}

size_t __attribute__((noinline))
access_speed_test_16_to_9_01(packed9_t *array9, const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array9[i].u.s.E1);
      sum = (sum + array9[i].u.s.E2);
      sum = (sum + array9[i].u.s.E3);
      sum = (sum + array9[i].u.s.E4);
      sum = (sum + array9[i].u.s.E5);
      sum = (sum + array9[i].u.s.E6);
      sum = (sum + array9[i].u.s.E7);
      sum = (sum + array9[i].u.s.E8);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_8_01(uint8_t *array8, const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array8[i]);
    }
  }
  return sum;
}

__attribute__((noinline)) void memcpy_16_to_12_01(uint16_t *arr,
                                                  packed12_t *arr2) {
  for (size_t i = 0, j = 0; i < SZ; i += 2, j++) {
    arr2[j].u.s.E1 = arr[i];
    arr2[j].u.s.E2 = arr[i + 1];
  }
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_01(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array12[i].u.s.E1);
      sum = (sum + array12[i].u.s.E2);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_02(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;
  const uint32_t maskE1 = 0xFFF;
  const uint32_t maskE2 = 0xFFF000;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < sz_tmp; i += 3) {
      const uint32_t unmasked_val =
          *(reinterpret_cast<uint32_t *>(arr_tmp + i));
      const uint32_t masked_val = (unmasked_val & maskE1);
      sum = (sum + masked_val);
    }

    for (size_t i = 0; i < sz_tmp; i += 3) {
      const uint32_t unmasked_val =
          *(reinterpret_cast<uint32_t *>(arr_tmp + i));
      const uint32_t masked_val = (unmasked_val & maskE2);
      const uint32_t shifted_val = masked_val >> 12llu;
      sum = (sum + shifted_val);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_03(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;
  const uint32_t maskE1 = 0xFFF;
  const uint32_t maskE2 = 0xFFF000;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < sz_tmp; i += 3) {
      const uint32_t unmasked_val =
          *(reinterpret_cast<uint32_t *>(arr_tmp + i));
      const uint32_t masked_val01 = (unmasked_val & maskE1);
      const uint32_t masked_val02 = (unmasked_val & maskE2);
      const uint32_t shifted_val = masked_val02 >> 12llu;
      sum = (sum + masked_val01 + shifted_val);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_04(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  const uint32_t maskE1 = 0xFFF;    // 12-bit mask for the lower part
  const uint32_t maskE2 = 0xFFF000; // 12-bit mask for the upper part

  __m256i sum_vec =
      _mm256_setzero_si256(); // Initialize a vector to accumulate the sum

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < sz_tmp; i += 3 * sizeof(uint32_t)) {

      // Load 32 bytes (8 values of uint32_t) at a time from arr_tmp
      __m256i data =
          _mm256_loadu_si256(reinterpret_cast<__m256i *>(arr_tmp + i));

      // Apply the bitmask for E1 (lower 12 bits)
      __m256i masked_val01 = _mm256_and_si256(data, _mm256_set1_epi32(maskE1));

      // Apply the bitmask for E2 (upper 12 bits)
      __m256i masked_val02 = _mm256_and_si256(data, _mm256_set1_epi32(maskE2));

      // Shift masked_val02 right by 12 positions
      __m256i shifted_val = _mm256_srli_epi32(masked_val02, 12);

      // Add masked_val01 and shifted_val
      __m256i result = _mm256_add_epi32(masked_val01, shifted_val);

      // Accumulate the results (use horizontal addition)
      sum_vec = _mm256_add_epi32(sum_vec, result);
    }
  }

  // Perform horizontal sum to reduce the vector to a scalar
  int32_t tmp_sum[8];
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(tmp_sum), sum_vec);

  for (int k = 0; k < 8; k++) {
    sum += tmp_sum[k];
  }

  return sum;
}

#ifdef PERPLEXITY_VERSION
size_t __attribute__((noinline))
access_speed_test_16_to_12_04(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;
  const uint32_t maskE1 = 0xFFF;
  const uint32_t maskE2 = 0xFFF000;

  // Ensure sz_tmp is a multiple of 12 for AVX2 (3 * 4 bytes)
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    // Process in chunks of 12 bytes (3 uint32_t values)
    for (size_t i = 0; i <= sz_tmp - 12; i += 12) {
      // Load 3 uint32_t values from arr_tmp
      __m128i data1 = _mm_loadu_si128(reinterpret_cast<__m128i *>(arr_tmp + i));
      __m128i data2 =
          _mm_loadu_si128(reinterpret_cast<__m128i *>(arr_tmp + i + 4));
      __m128i data3 =
          _mm_loadu_si128(reinterpret_cast<__m128i *>(arr_tmp + i + 8));

      // Convert to uint32_t
      __m256i vals =
          _mm256_set_epi32(_mm_cvtsi128_si32(data3), _mm_cvtsi128_si32(data2),
                           _mm_cvtsi128_si32(data1), 0, 0, 0, 0, 0);

      // Apply masks
      __m256i masked_val01 = _mm256_and_si256(vals, _mm256_set1_epi32(maskE1));
      __m256i masked_val02 = _mm256_and_si256(vals, _mm256_set1_epi32(maskE2));

      // Shift masked_val02 right by 12 bits
      masked_val02 = _mm256_srli_epi32(masked_val02, 12);

      // Sum masked values
      __m256i sum_vals = _mm256_add_epi32(masked_val01, masked_val02);
      sum_vals = _mm256_hadd_epi32(sum_vals, sum_vals);
      sum_vals = _mm256_hadd_epi32(sum_vals, sum_vals);

      // Extract the final sum from the vector
      sum += _mm_cvtsi256_si32(sum_vals);
    }

    // Handle remaining elements (if sz_tmp is not a multiple of 12)
    for (; i < sz_tmp; i += 3) {
      const uint32_t unmasked_val =
          *(reinterpret_cast<uint32_t *>(arr_tmp + i));
      const uint32_t masked_val01 = (unmasked_val & maskE1);
      const uint32_t masked_val02 = (unmasked_val & maskE2);
      const uint32_t shifted_val = masked_val02 >> 12llu;
      sum += (masked_val01 + shifted_val);
    }
  }

  return sum;
}
#endif // PERPLEXITY_VERSION

size_t __attribute__((noinline))
access_speed_test_16_to_12_05(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  const uint32_t maskE1 = 0xFFF;    // 12-bit mask for the lower part
  const uint32_t maskE2 = 0xFFF000; // 12-bit mask for the upper part

  __m256i sum_vec =
      _mm256_setzero_si256(); // Initialize a vector to accumulate the sum

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < sz_tmp; i += 3) { // Step by 3 bytes per packed12_t

      // Load 32-bit data from arr_tmp. We load 4 bytes at a time, since we're
      // using 32-bit registers.
      __m256i data =
          _mm256_loadu_si256(reinterpret_cast<__m256i *>(arr_tmp + i));

      // Since each packed12_t is 3 bytes, but we load 32 bits at a time, we
      // have to process each 3-byte segment.

      // Mask the lower 12 bits of the 32-bit data
      __m256i masked_val01 = _mm256_and_si256(data, _mm256_set1_epi32(maskE1));

      // Mask the upper 12 bits of the 32-bit data
      __m256i masked_val02 = _mm256_and_si256(data, _mm256_set1_epi32(maskE2));

      // Shift the upper bits by 12 (to position them correctly)
      __m256i shifted_val = _mm256_srli_epi32(masked_val02, 12);

      // Add the lower bits (masked_val01) and the shifted upper bits
      // (shifted_val)
      __m256i result = _mm256_add_epi32(masked_val01, shifted_val);

      // Accumulate the results into sum_vec
      sum_vec = _mm256_add_epi32(sum_vec, result);
    }
  }

  // Perform a horizontal sum to reduce the vector to a scalar
  int32_t tmp_sum[8];
  _mm256_storeu_si256(reinterpret_cast<__m256i *>(tmp_sum), sum_vec);

  // Sum the elements in tmp_sum to get the final scalar sum
  for (int k = 0; k < 8; k++) {
    sum += tmp_sum[k];
  }

  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_06(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  // 12-bit mask for the upper part
  const uint64_t maskE01 = 0x0000000000000FFF;
  const uint64_t maskE02 = 0x0000000000FFF000;
  const uint64_t maskE03 = 0x0000000FFF000000;
  const uint64_t maskE04 = 0x0000FFF000000000;
  const uint64_t maskE05 = 0x0FFF000000000000;
  const uint64_t maskE06_1 = 0xF000000000000000;
  const uint64_t maskE06_2 = 0x00000000000000FF;
  const uint64_t maskE07 = 0x00000000000FFF00;
  const uint64_t maskE08 = 0x00000000FFF00000;
  const uint64_t maskE09 = 0x00000FFF00000000;
  const uint64_t maskE10 = 0x00FFF00000000000;
  const uint64_t maskE11_1 = 0xFF00000000000000;
  const uint64_t maskE11_2 = 0x000000000000000F;
  const uint64_t maskE12 = 0x000000000000FFF0;
  const uint64_t maskE13 = 0x000000000FFF0000;
  const uint64_t maskE14 = 0x000000FFF0000000;
  const uint64_t maskE15 = 0x000FFF0000000000;
  const uint64_t maskE16 = 0xFFF0000000000000;

#if 0
  __v4du maskVE01 = {0, 0, 0, maskE01};
  __v4du maskVE02 = {0, 0, 0, maskE02};
  __v4du maskVE03 = {0, 0, 0, maskE03};
  __v4du maskVE04 = {0, 0, 0, maskE04};
  __v4du maskVE05 = {0, 0, 0, maskE05};
  __v4du maskVE06 = {0, 0, maskE06_2, maskE06_1};
  __v4du maskVE07 = {0, 0, maskE07, 0};
  __v4du maskVE08 = {0, 0, maskE08, 0};
  __v4du maskVE09 = {0, 0, maskE09, 0};
  __v4du maskVE10 = {0, 0, maskE10, 0};
  __v4du maskVE11 = {0, maskE11_2, maskE11_1, 0};
  __v4du maskVE12 = {0, maskE12, 0, 0};
  __v4du maskVE13 = {0, maskE13, 0, 0};
  __v4du maskVE14 = {0, maskE14, 0, 0};
  __v4du maskVE15 = {0, maskE15, 0, 0};
  __v4du maskVE16 = {0, maskE16, 0, 0};

  __v4du maskVE17 = {maskE01, 0, 0, 0};
  __v4du maskVE18 = {maskE02, 0, 0, 0};
  __v4du maskVE19 = {maskE03, 0, 0, 0};
  __v4du maskVE20 = {maskE04, 0, 0, 0};
  __v4du maskVE21 = {maskE05, 0, 0, 0};
  __v4du maskVE22_1 = {maskE06_1, 0, 0, 0};
  __v4du maskVE22_2 = {0, 0, 0, maskE06_2};
  __v4du maskVE23 = {0, 0, 0, maskE07};
  __v4du maskVE24 = {0, 0, 0, maskE08};
  __v4du maskVE25 = {0, 0, 0, maskE09};
  __v4du maskVE26 = {0, 0, 0, maskE10};
  __v4du maskVE27 = {0, 0, maskE11_2, maskE11_1};
  __v4du maskVE28 = {0, 0, maskE12, 0};
  __v4du maskVE29 = {0, 0, maskE13, 0};
  __v4du maskVE30 = {0, 0, maskE14, 0};
  __v4du maskVE31 = {0, 0, maskE15, 0};
  __v4du maskVE32 = {0, 0, maskE16, 0};

  __v4du maskVE33 = {0, maskE01, 0, 0};
  __v4du maskVE34 = {0, maskE02, 0, 0};
  __v4du maskVE35 = {0, maskE03, 0, 0};
  __v4du maskVE36 = {0, maskE04, 0, 0};
  __v4du maskVE37 = {0, maskE05, 0, 0};
  __v4du maskVE38 = {maskE06_2, maskE06_1, 0, 0};
  __v4du maskVE39 = {maskE07, 0, 0, 0};
  __v4du maskVE40 = {maskE08, 0, 0, 0};
  __v4du maskVE41 = {maskE09, 0, 0, 0};
  __v4du maskVE42 = {maskE10, 0, 0, 0};
  __v4du maskVE43_1 = {maskE11_1, 0, 0, 0};
  __v4du maskVE43_2 = {0, 0, 0, maskE11_2};
  __v4du maskVE44 = {0, 0, 0, maskE12};
  __v4du maskVE45 = {0, 0, 0, maskE13};
  __v4du maskVE46 = {0, 0, 0, maskE14};
  __v4du maskVE47 = {0, 0, 0, maskE15};
  __v4du maskVE48 = {0, 0, 0, maskE16};

  __v4du maskVE49 = {0, 0, maskE01, 0};
  __v4du maskVE50 = {0, 0, maskE02, 0};
  __v4du maskVE51 = {0, 0, maskE03, 0};
  __v4du maskVE52 = {0, 0, maskE04, 0};
  __v4du maskVE53 = {0, 0, maskE05, 0};
  __v4du maskVE54 = {0, maskE06_2, maskE06_1, 0};
  __v4du maskVE55 = {0, maskE07, 0, 0};
  __v4du maskVE56 = {0, maskE08, 0, 0};
  __v4du maskVE57 = {0, maskE09, 0, 0};
  __v4du maskVE58 = {0, maskE10, 0, 0};
  __v4du maskVE59 = {maskE11_2, maskE11_1, 0, 0};
  __v4du maskVE60 = {maskE12, 0, 0, 0};
  __v4du maskVE61 = {maskE13, 0, 0, 0};
  __v4du maskVE62 = {maskE14, 0, 0, 0};
  __v4du maskVE63 = {maskE15, 0, 0, 0};
  __v4du maskVE64 = {maskE16, 0, 0, 0};
#endif

  // Initialize vector to accumulate the sum
  __m256i sum_vec = _mm256_setzero_si256();

  __m256i mask_sum_vec_B1_V01 = {maskE01, 0, 0, maskE01};
  __m256i mask_sum_vec_B2_V01 = {0, 0, maskE01, 0};
  __m256i mask_sum_vec_B3_V01 = {0, maskE01, 0, 0};

  __m256i mask_sum_vec_B1_V02 = {maskE02, 0, 0, maskE02};
  __m256i mask_sum_vec_B2_V02 = {0, 0, maskE02, 0};
  __m256i mask_sum_vec_B3_V02 = {0, maskE02, 0, 0};

  __m256i mask_sum_vec_B1_V03 = {maskE03, 0, 0, maskE03};
  __m256i mask_sum_vec_B2_V03 = {0, 0, maskE03, 0};
  __m256i mask_sum_vec_B3_V03 = {0, maskE03, 0, 0};

  __m256i mask_sum_vec_B1_V04 = {maskE04, 0, 0, maskE04};
  __m256i mask_sum_vec_B2_V04 = {0, 0, maskE04, 0};
  __m256i mask_sum_vec_B3_V04 = {0, maskE04, 0, 0};

  __m256i mask_sum_vec_B1_V05 = {maskE05, 0, 0, maskE05};
  __m256i mask_sum_vec_B2_V05 = {0, 0, maskE05, 0};
  __m256i mask_sum_vec_B3_V05 = {0, maskE05, 0, 0};

  __m256i mask_sum_vec_B1_V06_1 = {static_cast<long long>(maskE06_1), 0, 0,
                                   static_cast<long long>(maskE06_1)};
  __m256i mask_sum_vec_B2_V06_1 = {0, 0, static_cast<long long>(maskE06_1), 0};
  __m256i mask_sum_vec_B3_V06_1 = {0, static_cast<long long>(maskE06_1), 0, 0};

  __m256i mask_sum_vec_B1_V06_2 = {0, maskE06_2, 0, 0};
  __m256i mask_sum_vec_B2_V06_2 = {maskE06_2, 0, 0, maskE06_2};
  __m256i mask_sum_vec_B3_V06_2 = {0, 0, maskE06_2, 0};

  __m256i mask_sum_vec_B1_V07 = {0, maskE07, 0, 0};
  __m256i mask_sum_vec_B2_V07 = {maskE07, 0, 0, maskE07};
  __m256i mask_sum_vec_B3_V07 = {0, 0, maskE07, 0};

  __m256i mask_sum_vec_B1_V08 = {0, maskE08, 0, 0};
  __m256i mask_sum_vec_B2_V08 = {maskE08, 0, 0, maskE08};
  __m256i mask_sum_vec_B3_V08 = {0, 0, maskE08, 0};

  __m256i mask_sum_vec_B1_V09 = {0, maskE09, 0, 0};
  __m256i mask_sum_vec_B2_V09 = {maskE09, 0, 0, maskE09};
  __m256i mask_sum_vec_B3_V09 = {0, 0, maskE09, 0};

  __m256i mask_sum_vec_B1_V10 = {0, maskE10, 0, 0};
  __m256i mask_sum_vec_B2_V10 = {maskE10, 0, 0, maskE10};
  __m256i mask_sum_vec_B3_V10 = {0, 0, maskE10, 0};

  __m256i mask_sum_vec_B1_V11_1 = {0, static_cast<long long>(maskE11_1), 0, 0};
  __m256i mask_sum_vec_B2_V11_1 = {static_cast<long long>(maskE11_1), 0, 0,
                                   static_cast<long long>(maskE11_1)};
  __m256i mask_sum_vec_B3_V11_1 = {0, 0, static_cast<long long>(maskE11_1), 0};

  __m256i mask_sum_vec_B1_V11_2 = {0, 0, maskE11_2, 0};
  __m256i mask_sum_vec_B2_V11_2 = {0, maskE11_2, 0, 0};
  __m256i mask_sum_vec_B3_V11_2 = {maskE11_2, 0, 0, maskE11_2};

  __m256i mask_sum_vec_B1_V12 = {0, 0, maskE12, 0};
  __m256i mask_sum_vec_B2_V12 = {0, maskE12, 0, 0};
  __m256i mask_sum_vec_B3_V12 = {maskE12, 0, 0, maskE12};

  __m256i mask_sum_vec_B1_V13 = {0, 0, maskE13, 0};
  __m256i mask_sum_vec_B2_V13 = {0, maskE13, 0, 0};
  __m256i mask_sum_vec_B3_V13 = {maskE13, 0, 0, maskE13};

  __m256i mask_sum_vec_B1_V14 = {0, 0, maskE14, 0};
  __m256i mask_sum_vec_B2_V14 = {0, maskE14, 0, 0};
  __m256i mask_sum_vec_B3_V14 = {maskE14, 0, 0, maskE14};

  __m256i mask_sum_vec_B1_V15 = {0, 0, maskE15, 0};
  __m256i mask_sum_vec_B2_V15 = {0, maskE15, 0, 0};
  __m256i mask_sum_vec_B3_V15 = {maskE15, 0, 0, maskE15};

  __m256i mask_sum_vec_B1_V16 = {0, 0, static_cast<long long>(maskE16), 0};
  __m256i mask_sum_vec_B2_V16 = {0, static_cast<long long>(maskE16), 0, 0};
  __m256i mask_sum_vec_B3_V16 = {static_cast<long long>(maskE16), 0, 0,
                                 static_cast<long long>(maskE16)};

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    size_t i = 0;
    for (; i < sz_tmp; i += 96) { // Step by 3 bytes per packed12_t

      // Load 32-bit data from arr_tmp. We load 4 bytes at a time, since we're
      // using 32-bit registers.
      __m256i data_W1 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i));
      __m256i data_W2 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 32));
      __m256i data_W3 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 64));

      __m256i masked_val_V01 = _mm256_or_si256(
          _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V01, data_W1),
                          _mm256_and_si256(mask_sum_vec_B2_V01, data_W2)),
          _mm256_and_si256(mask_sum_vec_B3_V01, data_W3));

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V02 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V02, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V02, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V02, data_W3)),
          12);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V03 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V03, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V03, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V03, data_W3)),
          24);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V04 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V04, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V04, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V04, data_W3)),
          36);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V05 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V05, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V05, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V05, data_W3)),
          48);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V06_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_1, data_W3)),
          60);
      __m256i masked_val_V06_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_2, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V07 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V07, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V07, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V07, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V08 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V08, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V08, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V08, data_W3)),
          20);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V09 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V09, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V09, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V09, data_W3)),
          32);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V10 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V10, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V10, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V10, data_W3)),
          44);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V11_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_1, data_W3)),
          56);
      __m256i masked_val_V11_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_2, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V12 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V12, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V12, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V12, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V13 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V13, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V13, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V13, data_W3)),
          16);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V14 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V14, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V14, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V14, data_W3)),
          28);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V15 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V15, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V15, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V15, data_W3)),
          40);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V16 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V16, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V16, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V16, data_W3)),
          52);

      // Accumulate the results into sum_vec
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V01);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V02);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V03);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V04);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V05);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V06_1);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V06_2);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V07);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V08);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V09);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V10);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V11_1);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V11_2);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V12);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V13);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V14);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V15);
      sum_vec = _mm256_add_epi64(sum_vec, masked_val_V16);
    }
    // assert(i == sz_tmp);
  }

  // Perform a horizontal sum to reduce the vector to a scalar
  uint64_t tmp_sum[4];
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum), sum_vec);

  // Sum the elements in tmp_sum to get the final scalar sum
  for (size_t k = 0; k < 4; k++) {
    sum += tmp_sum[k];
  }

  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_07(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  // 12-bit mask for the upper part
  const uint64_t maskE01 = 0x0000000000000FFF;
  const uint64_t maskE02 = 0x0000000000FFF000;
  const uint64_t maskE03 = 0x0000000FFF000000;
  const uint64_t maskE04 = 0x0000FFF000000000;
  const uint64_t maskE05 = 0x0FFF000000000000;
  const uint64_t maskE06_1 = 0xF000000000000000;
  const uint64_t maskE06_2 = 0x00000000000000FF;
  const uint64_t maskE07 = 0x00000000000FFF00;
  const uint64_t maskE08 = 0x00000000FFF00000;
  const uint64_t maskE09 = 0x00000FFF00000000;
  const uint64_t maskE10 = 0x00FFF00000000000;
  const uint64_t maskE11_1 = 0xFF00000000000000;
  const uint64_t maskE11_2 = 0x000000000000000F;
  const uint64_t maskE12 = 0x000000000000FFF0;
  const uint64_t maskE13 = 0x000000000FFF0000;
  const uint64_t maskE14 = 0x000000FFF0000000;
  const uint64_t maskE15 = 0x000FFF0000000000;
  const uint64_t maskE16 = 0xFFF0000000000000;

  // Initialize vector to accumulate the sum
  __m256i sum_vec_V01 = _mm256_setzero_si256();
  __m256i sum_vec_V02 = _mm256_setzero_si256();
  __m256i sum_vec_V03 = _mm256_setzero_si256();
  __m256i sum_vec_V04 = _mm256_setzero_si256();
  __m256i sum_vec_V05 = _mm256_setzero_si256();
  __m256i sum_vec_V06_1 = _mm256_setzero_si256();
  __m256i sum_vec_V06_2 = _mm256_setzero_si256();
  __m256i sum_vec_V07 = _mm256_setzero_si256();
  __m256i sum_vec_V08 = _mm256_setzero_si256();
  __m256i sum_vec_V09 = _mm256_setzero_si256();
  __m256i sum_vec_V10 = _mm256_setzero_si256();
  __m256i sum_vec_V11_1 = _mm256_setzero_si256();
  __m256i sum_vec_V11_2 = _mm256_setzero_si256();
  __m256i sum_vec_V12 = _mm256_setzero_si256();
  __m256i sum_vec_V13 = _mm256_setzero_si256();
  __m256i sum_vec_V14 = _mm256_setzero_si256();
  __m256i sum_vec_V15 = _mm256_setzero_si256();
  __m256i sum_vec_V16 = _mm256_setzero_si256();

  __m256i mask_sum_vec_B1_V01 = {maskE01, 0, 0, maskE01};
  __m256i mask_sum_vec_B2_V01 = {0, 0, maskE01, 0};
  __m256i mask_sum_vec_B3_V01 = {0, maskE01, 0, 0};

  __m256i mask_sum_vec_B1_V02 = {maskE02, 0, 0, maskE02};
  __m256i mask_sum_vec_B2_V02 = {0, 0, maskE02, 0};
  __m256i mask_sum_vec_B3_V02 = {0, maskE02, 0, 0};

  __m256i mask_sum_vec_B1_V03 = {maskE03, 0, 0, maskE03};
  __m256i mask_sum_vec_B2_V03 = {0, 0, maskE03, 0};
  __m256i mask_sum_vec_B3_V03 = {0, maskE03, 0, 0};

  __m256i mask_sum_vec_B1_V04 = {maskE04, 0, 0, maskE04};
  __m256i mask_sum_vec_B2_V04 = {0, 0, maskE04, 0};
  __m256i mask_sum_vec_B3_V04 = {0, maskE04, 0, 0};

  __m256i mask_sum_vec_B1_V05 = {maskE05, 0, 0, maskE05};
  __m256i mask_sum_vec_B2_V05 = {0, 0, maskE05, 0};
  __m256i mask_sum_vec_B3_V05 = {0, maskE05, 0, 0};

  __m256i mask_sum_vec_B1_V06_1 = {static_cast<long long>(maskE06_1), 0, 0,
                                   static_cast<long long>(maskE06_1)};
  __m256i mask_sum_vec_B2_V06_1 = {0, 0, static_cast<long long>(maskE06_1), 0};
  __m256i mask_sum_vec_B3_V06_1 = {0, static_cast<long long>(maskE06_1), 0, 0};

  __m256i mask_sum_vec_B1_V06_2 = {0, maskE06_2, 0, 0};
  __m256i mask_sum_vec_B2_V06_2 = {maskE06_2, 0, 0, maskE06_2};
  __m256i mask_sum_vec_B3_V06_2 = {0, 0, maskE06_2, 0};

  __m256i mask_sum_vec_B1_V07 = {0, maskE07, 0, 0};
  __m256i mask_sum_vec_B2_V07 = {maskE07, 0, 0, maskE07};
  __m256i mask_sum_vec_B3_V07 = {0, 0, maskE07, 0};

  __m256i mask_sum_vec_B1_V08 = {0, maskE08, 0, 0};
  __m256i mask_sum_vec_B2_V08 = {maskE08, 0, 0, maskE08};
  __m256i mask_sum_vec_B3_V08 = {0, 0, maskE08, 0};

  __m256i mask_sum_vec_B1_V09 = {0, maskE09, 0, 0};
  __m256i mask_sum_vec_B2_V09 = {maskE09, 0, 0, maskE09};
  __m256i mask_sum_vec_B3_V09 = {0, 0, maskE09, 0};

  __m256i mask_sum_vec_B1_V10 = {0, maskE10, 0, 0};
  __m256i mask_sum_vec_B2_V10 = {maskE10, 0, 0, maskE10};
  __m256i mask_sum_vec_B3_V10 = {0, 0, maskE10, 0};

  __m256i mask_sum_vec_B1_V11_1 = {0, static_cast<long long>(maskE11_1), 0, 0};
  __m256i mask_sum_vec_B2_V11_1 = {static_cast<long long>(maskE11_1), 0, 0,
                                   static_cast<long long>(maskE11_1)};
  __m256i mask_sum_vec_B3_V11_1 = {0, 0, static_cast<long long>(maskE11_1), 0};

  __m256i mask_sum_vec_B1_V11_2 = {0, 0, maskE11_2, 0};
  __m256i mask_sum_vec_B2_V11_2 = {0, maskE11_2, 0, 0};
  __m256i mask_sum_vec_B3_V11_2 = {maskE11_2, 0, 0, maskE11_2};

  __m256i mask_sum_vec_B1_V12 = {0, 0, maskE12, 0};
  __m256i mask_sum_vec_B2_V12 = {0, maskE12, 0, 0};
  __m256i mask_sum_vec_B3_V12 = {maskE12, 0, 0, maskE12};

  __m256i mask_sum_vec_B1_V13 = {0, 0, maskE13, 0};
  __m256i mask_sum_vec_B2_V13 = {0, maskE13, 0, 0};
  __m256i mask_sum_vec_B3_V13 = {maskE13, 0, 0, maskE13};

  __m256i mask_sum_vec_B1_V14 = {0, 0, maskE14, 0};
  __m256i mask_sum_vec_B2_V14 = {0, maskE14, 0, 0};
  __m256i mask_sum_vec_B3_V14 = {maskE14, 0, 0, maskE14};

  __m256i mask_sum_vec_B1_V15 = {0, 0, maskE15, 0};
  __m256i mask_sum_vec_B2_V15 = {0, maskE15, 0, 0};
  __m256i mask_sum_vec_B3_V15 = {maskE15, 0, 0, maskE15};

  __m256i mask_sum_vec_B1_V16 = {0, 0, static_cast<long long>(maskE16), 0};
  __m256i mask_sum_vec_B2_V16 = {0, static_cast<long long>(maskE16), 0, 0};
  __m256i mask_sum_vec_B3_V16 = {static_cast<long long>(maskE16), 0, 0,
                                 static_cast<long long>(maskE16)};

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    size_t i = 0;
    for (; i < sz_tmp; i += 96) { // Step by 3 bytes per packed12_t

      // Load 32-bit data from arr_tmp. We load 4 bytes at a time, since we're
      // using 32-bit registers.
      __m256i data_W1 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i));
      __m256i data_W2 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 32));
      __m256i data_W3 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 64));

      __m256i masked_val_V01 = _mm256_or_si256(
          _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V01, data_W1),
                          _mm256_and_si256(mask_sum_vec_B2_V01, data_W2)),
          _mm256_and_si256(mask_sum_vec_B3_V01, data_W3));

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V02 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V02, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V02, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V02, data_W3)),
          12);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V03 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V03, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V03, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V03, data_W3)),
          24);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V04 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V04, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V04, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V04, data_W3)),
          36);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V05 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V05, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V05, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V05, data_W3)),
          48);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V06_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_1, data_W3)),
          60);
      __m256i masked_val_V06_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_2, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V07 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V07, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V07, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V07, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V08 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V08, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V08, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V08, data_W3)),
          20);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V09 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V09, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V09, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V09, data_W3)),
          32);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V10 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V10, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V10, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V10, data_W3)),
          44);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V11_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_1, data_W3)),
          56);
      __m256i masked_val_V11_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_2, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V12 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V12, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V12, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V12, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V13 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V13, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V13, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V13, data_W3)),
          16);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V14 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V14, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V14, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V14, data_W3)),
          28);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V15 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V15, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V15, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V15, data_W3)),
          40);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V16 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V16, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V16, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V16, data_W3)),
          52);

      // Accumulate the results into sum_vec
      sum_vec_V01 = _mm256_add_epi64(sum_vec_V01, masked_val_V01);
      sum_vec_V02 = _mm256_add_epi64(sum_vec_V02, masked_val_V02);
      sum_vec_V03 = _mm256_add_epi64(sum_vec_V03, masked_val_V03);
      sum_vec_V04 = _mm256_add_epi64(sum_vec_V04, masked_val_V04);
      sum_vec_V05 = _mm256_add_epi64(sum_vec_V05, masked_val_V05);
      sum_vec_V06_1 = _mm256_add_epi64(sum_vec_V06_1, masked_val_V06_1);
      sum_vec_V06_2 = _mm256_add_epi64(sum_vec_V06_2, masked_val_V06_2);
      sum_vec_V07 = _mm256_add_epi64(sum_vec_V07, masked_val_V07);
      sum_vec_V08 = _mm256_add_epi64(sum_vec_V08, masked_val_V08);
      sum_vec_V09 = _mm256_add_epi64(sum_vec_V09, masked_val_V09);
      sum_vec_V10 = _mm256_add_epi64(sum_vec_V10, masked_val_V10);
      sum_vec_V11_1 = _mm256_add_epi64(sum_vec_V11_1, masked_val_V11_1);
      sum_vec_V11_2 = _mm256_add_epi64(sum_vec_V11_2, masked_val_V11_2);
      sum_vec_V12 = _mm256_add_epi64(sum_vec_V12, masked_val_V12);
      sum_vec_V13 = _mm256_add_epi64(sum_vec_V13, masked_val_V13);
      sum_vec_V14 = _mm256_add_epi64(sum_vec_V14, masked_val_V14);
      sum_vec_V15 = _mm256_add_epi64(sum_vec_V15, masked_val_V15);
      sum_vec_V16 = _mm256_add_epi64(sum_vec_V16, masked_val_V16);
    }
    // assert(i == sz_tmp);
  }

  // Perform a horizontal sum to reduce the vector to a scalar
  uint64_t tmp_sum_V01[4];
  uint64_t tmp_sum_V02[4];
  uint64_t tmp_sum_V03[4];
  uint64_t tmp_sum_V04[4];
  uint64_t tmp_sum_V05[4];
  uint64_t tmp_sum_V06_1[4];
  uint64_t tmp_sum_V06_2[4];
  uint64_t tmp_sum_V07[4];
  uint64_t tmp_sum_V08[4];
  uint64_t tmp_sum_V09[4];
  uint64_t tmp_sum_V10[4];
  uint64_t tmp_sum_V11_1[4];
  uint64_t tmp_sum_V11_2[4];
  uint64_t tmp_sum_V12[4];
  uint64_t tmp_sum_V13[4];
  uint64_t tmp_sum_V14[4];
  uint64_t tmp_sum_V15[4];
  uint64_t tmp_sum_V16[4];
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V01), sum_vec_V01);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V02), sum_vec_V02);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V03), sum_vec_V03);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V04), sum_vec_V04);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V05), sum_vec_V05);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V06_1), sum_vec_V06_1);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V06_2), sum_vec_V06_2);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V07), sum_vec_V07);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V08), sum_vec_V08);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V09), sum_vec_V09);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V10), sum_vec_V10);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V11_1), sum_vec_V11_1);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V11_2), sum_vec_V11_2);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V12), sum_vec_V12);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V13), sum_vec_V13);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V14), sum_vec_V14);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V15), sum_vec_V15);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V16), sum_vec_V16);

  // Sum the elements in tmp_sum to get the final scalar sum
  for (size_t k = 0; k < 4; k++) {
    sum += tmp_sum_V01[k];
    sum += tmp_sum_V02[k];
    sum += tmp_sum_V03[k];
    sum += tmp_sum_V04[k];
    sum += tmp_sum_V05[k];
    sum += tmp_sum_V06_1[k];
    sum += tmp_sum_V06_2[k];
    sum += tmp_sum_V07[k];
    sum += tmp_sum_V08[k];
    sum += tmp_sum_V09[k];
    sum += tmp_sum_V10[k];
    sum += tmp_sum_V11_1[k];
    sum += tmp_sum_V11_2[k];
    sum += tmp_sum_V12[k];
    sum += tmp_sum_V13[k];
    sum += tmp_sum_V14[k];
    sum += tmp_sum_V15[k];
    sum += tmp_sum_V16[k];
  }

  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_08(packed12_t *array12, const size_t size) {
  size_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  // 12-bit mask for the upper part
  const uint64_t maskE01 = 0x0000000000000FFF;
  const uint64_t maskE02 = 0x0000000000FFF000;
  const uint64_t maskE03 = 0x0000000FFF000000;
  const uint64_t maskE04 = 0x0000FFF000000000;
  const uint64_t maskE05 = 0x0FFF000000000000;
  const uint64_t maskE06_1 = 0xF000000000000000;
  const uint64_t maskE06_2 = 0x00000000000000FF;
  const uint64_t maskE07 = 0x00000000000FFF00;
  const uint64_t maskE08 = 0x00000000FFF00000;
  const uint64_t maskE09 = 0x00000FFF00000000;
  const uint64_t maskE10 = 0x00FFF00000000000;
  const uint64_t maskE11_1 = 0xFF00000000000000;
  const uint64_t maskE11_2 = 0x000000000000000F;
  const uint64_t maskE12 = 0x000000000000FFF0;
  const uint64_t maskE13 = 0x000000000FFF0000;
  const uint64_t maskE14 = 0x000000FFF0000000;
  const uint64_t maskE15 = 0x000FFF0000000000;
  const uint64_t maskE16 = 0xFFF0000000000000;

  // Initialize vector to accumulate the sum
  __m256i sum_vec_V01 = _mm256_setzero_si256();
  __m256i sum_vec_V02 = _mm256_setzero_si256();
  __m256i sum_vec_V03 = _mm256_setzero_si256();
  __m256i sum_vec_V04 = _mm256_setzero_si256();
  __m256i sum_vec_V05 = _mm256_setzero_si256();
  __m256i sum_vec_V06_1 = _mm256_setzero_si256();
  __m256i sum_vec_V06_2 = _mm256_setzero_si256();
  __m256i sum_vec_V07 = _mm256_setzero_si256();
  __m256i sum_vec_V08 = _mm256_setzero_si256();
  __m256i sum_vec_V09 = _mm256_setzero_si256();
  __m256i sum_vec_V10 = _mm256_setzero_si256();
  __m256i sum_vec_V11_1 = _mm256_setzero_si256();
  __m256i sum_vec_V11_2 = _mm256_setzero_si256();
  __m256i sum_vec_V12 = _mm256_setzero_si256();
  __m256i sum_vec_V13 = _mm256_setzero_si256();
  __m256i sum_vec_V14 = _mm256_setzero_si256();
  __m256i sum_vec_V15 = _mm256_setzero_si256();
  __m256i sum_vec_V16 = _mm256_setzero_si256();

  __m256i mask_sum_vec_B1_V01 = {maskE01, 0, 0, maskE01};
  __m256i mask_sum_vec_B2_V01 = {0, 0, maskE01, 0};
  __m256i mask_sum_vec_B3_V01 = {0, maskE01, 0, 0};

  __m256i mask_sum_vec_B1_V02 = {maskE02, 0, 0, maskE02};
  __m256i mask_sum_vec_B2_V02 = {0, 0, maskE02, 0};
  __m256i mask_sum_vec_B3_V02 = {0, maskE02, 0, 0};

  __m256i mask_sum_vec_B1_V03 = {maskE03, 0, 0, maskE03};
  __m256i mask_sum_vec_B2_V03 = {0, 0, maskE03, 0};
  __m256i mask_sum_vec_B3_V03 = {0, maskE03, 0, 0};

  __m256i mask_sum_vec_B1_V04 = {maskE04, 0, 0, maskE04};
  __m256i mask_sum_vec_B2_V04 = {0, 0, maskE04, 0};
  __m256i mask_sum_vec_B3_V04 = {0, maskE04, 0, 0};

  __m256i mask_sum_vec_B1_V05 = {maskE05, 0, 0, maskE05};
  __m256i mask_sum_vec_B2_V05 = {0, 0, maskE05, 0};
  __m256i mask_sum_vec_B3_V05 = {0, maskE05, 0, 0};

  __m256i mask_sum_vec_B1_V06_1 = {static_cast<long long>(maskE06_1), 0, 0,
                                   static_cast<long long>(maskE06_1)};
  __m256i mask_sum_vec_B2_V06_1 = {0, 0, static_cast<long long>(maskE06_1), 0};
  __m256i mask_sum_vec_B3_V06_1 = {0, static_cast<long long>(maskE06_1), 0, 0};

  __m256i mask_sum_vec_B1_V06_2 = {0, maskE06_2, 0, 0};
  __m256i mask_sum_vec_B2_V06_2 = {maskE06_2, 0, 0, maskE06_2};
  __m256i mask_sum_vec_B3_V06_2 = {0, 0, maskE06_2, 0};

  __m256i mask_sum_vec_B1_V07 = {0, maskE07, 0, 0};
  __m256i mask_sum_vec_B2_V07 = {maskE07, 0, 0, maskE07};
  __m256i mask_sum_vec_B3_V07 = {0, 0, maskE07, 0};

  __m256i mask_sum_vec_B1_V08 = {0, maskE08, 0, 0};
  __m256i mask_sum_vec_B2_V08 = {maskE08, 0, 0, maskE08};
  __m256i mask_sum_vec_B3_V08 = {0, 0, maskE08, 0};

  __m256i mask_sum_vec_B1_V09 = {0, maskE09, 0, 0};
  __m256i mask_sum_vec_B2_V09 = {maskE09, 0, 0, maskE09};
  __m256i mask_sum_vec_B3_V09 = {0, 0, maskE09, 0};

  __m256i mask_sum_vec_B1_V10 = {0, maskE10, 0, 0};
  __m256i mask_sum_vec_B2_V10 = {maskE10, 0, 0, maskE10};
  __m256i mask_sum_vec_B3_V10 = {0, 0, maskE10, 0};

  __m256i mask_sum_vec_B1_V11_1 = {0, static_cast<long long>(maskE11_1), 0, 0};
  __m256i mask_sum_vec_B2_V11_1 = {static_cast<long long>(maskE11_1), 0, 0,
                                   static_cast<long long>(maskE11_1)};
  __m256i mask_sum_vec_B3_V11_1 = {0, 0, static_cast<long long>(maskE11_1), 0};

  __m256i mask_sum_vec_B1_V11_2 = {0, 0, maskE11_2, 0};
  __m256i mask_sum_vec_B2_V11_2 = {0, maskE11_2, 0, 0};
  __m256i mask_sum_vec_B3_V11_2 = {maskE11_2, 0, 0, maskE11_2};

  __m256i mask_sum_vec_B1_V12 = {0, 0, maskE12, 0};
  __m256i mask_sum_vec_B2_V12 = {0, maskE12, 0, 0};
  __m256i mask_sum_vec_B3_V12 = {maskE12, 0, 0, maskE12};

  __m256i mask_sum_vec_B1_V13 = {0, 0, maskE13, 0};
  __m256i mask_sum_vec_B2_V13 = {0, maskE13, 0, 0};
  __m256i mask_sum_vec_B3_V13 = {maskE13, 0, 0, maskE13};

  __m256i mask_sum_vec_B1_V14 = {0, 0, maskE14, 0};
  __m256i mask_sum_vec_B2_V14 = {0, maskE14, 0, 0};
  __m256i mask_sum_vec_B3_V14 = {maskE14, 0, 0, maskE14};

  __m256i mask_sum_vec_B1_V15 = {0, 0, maskE15, 0};
  __m256i mask_sum_vec_B2_V15 = {0, maskE15, 0, 0};
  __m256i mask_sum_vec_B3_V15 = {maskE15, 0, 0, maskE15};

  __m256i mask_sum_vec_B1_V16 = {0, 0, static_cast<long long>(maskE16), 0};
  __m256i mask_sum_vec_B2_V16 = {0, static_cast<long long>(maskE16), 0, 0};
  __m256i mask_sum_vec_B3_V16 = {static_cast<long long>(maskE16), 0, 0,
                                 static_cast<long long>(maskE16)};

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    size_t i = 0;
    for (; i < sz_tmp; i += 96) { // Step by 3 bytes per packed12_t

      // Load 32-bit data from arr_tmp. We load 4 bytes at a time, since we're
      // using 32-bit registers.
      __m256i data_W1 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i));
      __m256i data_W2 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 32));
      __m256i data_W3 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 64));

      __m256i masked_val_V01 = _mm256_or_si256(
          _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V01, data_W1),
                          _mm256_and_si256(mask_sum_vec_B2_V01, data_W2)),
          _mm256_and_si256(mask_sum_vec_B3_V01, data_W3));

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V02 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V02, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V02, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V02, data_W3)),
          12);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V03 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V03, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V03, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V03, data_W3)),
          24);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V04 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V04, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V04, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V04, data_W3)),
          36);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V05 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V05, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V05, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V05, data_W3)),
          48);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V06_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_1, data_W3)),
          60);
      __m256i masked_val_V06_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V06_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V06_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V06_2, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V07 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V07, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V07, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V07, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V08 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V08, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V08, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V08, data_W3)),
          20);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V09 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V09, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V09, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V09, data_W3)),
          32);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V10 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V10, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V10, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V10, data_W3)),
          44);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V11_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_1, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_1, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_1, data_W3)),
          56);
      __m256i masked_val_V11_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V11_2, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V11_2, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V11_2, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V12 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V12, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V12, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V12, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V13 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V13, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V13, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V13, data_W3)),
          16);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V14 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V14, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V14, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V14, data_W3)),
          28);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V15 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V15, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V15, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V15, data_W3)),
          40);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V16 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(_mm256_and_si256(mask_sum_vec_B1_V16, data_W1),
                              _mm256_and_si256(mask_sum_vec_B2_V16, data_W2)),
              _mm256_and_si256(mask_sum_vec_B3_V16, data_W3)),
          52);

      // Accumulate the results into sum_vec
      sum_vec_V01 = _mm256_add_epi64(sum_vec_V01, masked_val_V01);
      sum_vec_V02 = _mm256_add_epi64(sum_vec_V02, masked_val_V02);
      sum_vec_V03 = _mm256_add_epi64(sum_vec_V03, masked_val_V03);
      sum_vec_V04 = _mm256_add_epi64(sum_vec_V04, masked_val_V04);
      sum_vec_V05 = _mm256_add_epi64(sum_vec_V05, masked_val_V05);
      sum_vec_V06_1 = _mm256_add_epi64(sum_vec_V06_1, masked_val_V06_1);
      sum_vec_V06_2 = _mm256_add_epi64(sum_vec_V06_2, masked_val_V06_2);
      sum_vec_V07 = _mm256_add_epi64(sum_vec_V07, masked_val_V07);
      sum_vec_V08 = _mm256_add_epi64(sum_vec_V08, masked_val_V08);
      sum_vec_V09 = _mm256_add_epi64(sum_vec_V09, masked_val_V09);
      sum_vec_V10 = _mm256_add_epi64(sum_vec_V10, masked_val_V10);
      sum_vec_V11_1 = _mm256_add_epi64(sum_vec_V11_1, masked_val_V11_1);
      sum_vec_V11_2 = _mm256_add_epi64(sum_vec_V11_2, masked_val_V11_2);
      sum_vec_V12 = _mm256_add_epi64(sum_vec_V12, masked_val_V12);
      sum_vec_V13 = _mm256_add_epi64(sum_vec_V13, masked_val_V13);
      sum_vec_V14 = _mm256_add_epi64(sum_vec_V14, masked_val_V14);
      sum_vec_V15 = _mm256_add_epi64(sum_vec_V15, masked_val_V15);
      sum_vec_V16 = _mm256_add_epi64(sum_vec_V16, masked_val_V16);
    }
    // assert(i == sz_tmp);
  }

  // Perform a horizontal sum to reduce the vector to a scalar
  uint64_t tmp_sum_V01[4];
  uint64_t tmp_sum_V02[4];
  uint64_t tmp_sum_V03[4];
  uint64_t tmp_sum_V04[4];
  uint64_t tmp_sum_V05[4];
  uint64_t tmp_sum_V06_1[4];
  uint64_t tmp_sum_V06_2[4];
  uint64_t tmp_sum_V07[4];
  uint64_t tmp_sum_V08[4];
  uint64_t tmp_sum_V09[4];
  uint64_t tmp_sum_V10[4];
  uint64_t tmp_sum_V11_1[4];
  uint64_t tmp_sum_V11_2[4];
  uint64_t tmp_sum_V12[4];
  uint64_t tmp_sum_V13[4];
  uint64_t tmp_sum_V14[4];
  uint64_t tmp_sum_V15[4];
  uint64_t tmp_sum_V16[4];
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V01), sum_vec_V01);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V02), sum_vec_V02);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V03), sum_vec_V03);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V04), sum_vec_V04);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V05), sum_vec_V05);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V06_1), sum_vec_V06_1);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V06_2), sum_vec_V06_2);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V07), sum_vec_V07);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V08), sum_vec_V08);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V09), sum_vec_V09);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V10), sum_vec_V10);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V11_1), sum_vec_V11_1);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V11_2), sum_vec_V11_2);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V12), sum_vec_V12);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V13), sum_vec_V13);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V14), sum_vec_V14);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V15), sum_vec_V15);
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum_V16), sum_vec_V16);

  uint64_t tmp_sum[18] = {0};

  // Sum the elements in tmp_sum to get the final scalar sum
  for (size_t k = 0; k < 4; k++) {
    tmp_sum[0] += tmp_sum_V01[k];
    tmp_sum[1] += tmp_sum_V02[k];
    tmp_sum[2] += tmp_sum_V03[k];
    tmp_sum[3] += tmp_sum_V04[k];
    tmp_sum[4] += tmp_sum_V05[k];
    tmp_sum[5] += tmp_sum_V06_1[k];
    tmp_sum[6] += tmp_sum_V06_2[k];
    tmp_sum[7] += tmp_sum_V07[k];
    tmp_sum[8] += tmp_sum_V08[k];
    tmp_sum[9] += tmp_sum_V09[k];
    tmp_sum[10] += tmp_sum_V10[k];
    tmp_sum[11] += tmp_sum_V11_1[k];
    tmp_sum[12] += tmp_sum_V11_2[k];
    tmp_sum[13] += tmp_sum_V12[k];
    tmp_sum[14] += tmp_sum_V13[k];
    tmp_sum[15] += tmp_sum_V14[k];
    tmp_sum[16] += tmp_sum_V15[k];
    tmp_sum[17] += tmp_sum_V16[k];
  }

  for (size_t k = 0; k < 18; k++) {
    sum += tmp_sum[k];
  }

  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_09_32bitSum(packed12_t *array12, const size_t size) {
  uint32_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;
  const uint32_t maskE1 = 0xFFF;
  const uint32_t maskE2 = 0xFFF000;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < sz_tmp; i += 3) {
      const uint32_t unmasked_val =
          *(reinterpret_cast<uint32_t *>(arr_tmp + i));
      const uint32_t masked_val01 = (unmasked_val & maskE1);
      const uint32_t masked_val02 = (unmasked_val & maskE2);
      const uint32_t shifted_val = masked_val02 >> 12llu;
      sum = (sum + masked_val01 + shifted_val);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_to_12_10_32bitSum(packed12_t *array12, const size_t size) {
  uint32_t sum = 0;
  uint8_t *arr_tmp = reinterpret_cast<uint8_t *>(array12);
  size_t sz_tmp = sizeof(packed12_t) * size;

  // 12-bit mask for the upper part
  const uint32_t maskE01__ = 0x00000FFF;
  const uint32_t maskE02__ = 0x00FFF000;
  const uint32_t maskE03_1 = 0xFF000000;
  const uint32_t maskE03_2 = 0x0000000F;
  const uint32_t maskE04__ = 0x0000FFF0;
  const uint32_t maskE05__ = 0x0FFF0000;
  const uint32_t maskE06_1 = 0xF0000000;
  const uint32_t maskE06_2 = 0x000000FF;
  const uint32_t maskE07__ = 0x000FFF00;
  const uint32_t maskE08__ = 0xFFF00000;

#if 0
  __v8su maskVE01__ = {maskE01__, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE02__ = {maskE02__, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE03__ = {maskE03_1, maskE03_2, 0, 0, 0, 0, 0, 0};
  __v8su maskVE04__ = {0, maskE04__, 0, 0, 0, 0, 0, 0};
  __v8su maskVE05__ = {0, maskE05__, 0, 0, 0, 0, 0, 0};
  __v8su maskVE06__ = {0, maskE06_1, maskE06_2, 0, 0, 0, 0, 0};
  __v8su maskVE07__ = {0, 0, maskE07__, 0, 0, 0, 0, 0};
  __v8su maskVE08__ = {0, 0, maskE08__, 0, 0, 0, 0, 0};

  __v8su maskVE09__ = {0, 0, 0, maskE01__, 0, 0, 0, 0};
  __v8su maskVE10__ = {0, 0, 0, maskE02__, 0, 0, 0, 0};
  __v8su maskVE11__ = {0, 0, 0, maskE03_1, maskE03_2, 0, 0, 0};
  __v8su maskVE12__ = {0, 0, 0, 0, maskE04__, 0, 0, 0};
  __v8su maskVE13__ = {0, 0, 0, 0, maskE05__, 0, 0, 0};
  __v8su maskVE14__ = {0, 0, 0, 0, maskE06_1, maskE06_2, 0, 0};
  __v8su maskVE15__ = {0, 0, 0, 0, 0, maskE07__, 0, 0};
  __v8su maskVE16__ = {0, 0, 0, 0, 0, maskE08__, 0, 0};

  __v8su maskVE17__ = {0, 0, 0, 0, 0, 0, maskE01__, 0};
  __v8su maskVE18__ = {0, 0, 0, 0, 0, 0, maskE02__, 0};
  __v8su maskVE19__ = {0, 0, 0, 0, 0, 0, maskE03_1, maskE03_2};
  __v8su maskVE20__ = {0, 0, 0, 0, 0, 0, 0, maskE04__};
  __v8su maskVE21__ = {0, 0, 0, 0, 0, 0, 0, maskE05__};
  __v8su maskVE22_1 = {0, 0, 0, 0, 0, 0, 0, maskE06_1};
  __v8su maskVE22_2 = {maskE06_2, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE23__ = {maskE07__, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE24__ = {maskE08__, 0, 0, 0, 0, 0, 0, 0};

  __v8su maskVE25__ = {0, maskE01__, 0, 0, 0, 0, 0, 0};
  __v8su maskVE26__ = {0, maskE02__, 0, 0, 0, 0, 0, 0};
  __v8su maskVE27__ = {0, maskE03_1, maskE03_2, 0, 0, 0, 0, 0};
  __v8su maskVE28__ = {0, 0, maskE04__, 0, 0, 0, 0, 0};
  __v8su maskVE29__ = {0, 0, maskE05__, 0, 0, 0, 0, 0};
  __v8su maskVE30__ = {0, 0, maskE06_1, maskE06_2, 0, 0, 0, 0};
  __v8su maskVE31__ = {0, 0, 0, maskE07__, 0, 0, 0, 0};
  __v8su maskVE32__ = {0, 0, 0, maskE08__, 0, 0, 0, 0};

  __v8su maskVE33__ = {0, 0, 0, 0, maskE01__, 0, 0, 0};
  __v8su maskVE34__ = {0, 0, 0, 0, maskE02__, 0, 0, 0};
  __v8su maskVE35__ = {0, 0, 0, 0, maskE03_1, maskE03_2, 0, 0};
  __v8su maskVE36__ = {0, 0, 0, 0, 0, maskE04__, 0, 0};
  __v8su maskVE37__ = {0, 0, 0, 0, 0, maskE05__, 0, 0};
  __v8su maskVE38__ = {0, 0, 0, 0, 0, maskE06_1, maskE06_2, 0};
  __v8su maskVE39__ = {0, 0, 0, 0, 0, 0, maskE07__, 0};
  __v8su maskVE40__ = {0, 0, 0, 0, 0, 0, maskE08__, 0};

  __v8su maskVE41__ = {0, 0, 0, 0, 0, 0, 0, maskE01__};
  __v8su maskVE42__ = {0, 0, 0, 0, 0, 0, 0, maskE02__};
  __v8su maskVE43_1 = {0, 0, 0, 0, 0, 0, 0, maskE03_1};
  __v8su maskVE43_2 = {maskE03_2, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE44__ = {maskE04__, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE45__ = {maskE05__, 0, 0, 0, 0, 0, 0, 0};
  __v8su maskVE46__ = {maskE06_1, maskE06_2, 0, 0, 0, 0, 0, 0};
  __v8su maskVE47__ = {0, maskE07__, 0, 0, 0, 0, 0, 0};
  __v8su maskVE48__ = {0, maskE08__, 0, 0, 0, 0, 0, 0};

  __v8su maskVE49__ = {0, 0, maskE01__, 0, 0, 0, 0, 0};
  __v8su maskVE50__ = {0, 0, maskE02__, 0, 0, 0, 0, 0};
  __v8su maskVE51__ = {0, 0, maskE03_1, maskE03_2, 0, 0, 0, 0};
  __v8su maskVE52__ = {0, 0, 0, maskE04__, 0, 0, 0, 0};
  __v8su maskVE53__ = {0, 0, 0, maskE05__, 0, 0, 0, 0};
  __v8su maskVE54__ = {0, 0, 0, maskE06_1, maskE06_2, 0, 0, 0};
  __v8su maskVE55__ = {0, 0, 0, 0, maskE07__, 0, 0, 0};
  __v8su maskVE56__ = {0, 0, 0, 0, maskE08__, 0, 0, 0};

  __v8su maskVE57__ = {0, 0, 0, 0, 0, maskE01__, 0, 0};
  __v8su maskVE58__ = {0, 0, 0, 0, 0, maskE02__, 0, 0};
  __v8su maskVE59__ = {0, 0, 0, 0, 0, maskE03_1, maskE03_2, 0};
  __v8su maskVE60__ = {0, 0, 0, 0, 0, 0, maskE04__, 0};
  __v8su maskVE61__ = {0, 0, 0, 0, 0, 0, maskE05__, 0};
  __v8su maskVE62__ = {0, 0, 0, 0, 0, 0, maskE06_1, maskE06_2};
  __v8su maskVE63__ = {0, 0, 0, 0, 0, 0, 0, maskE07__};
  __v8su maskVE64__ = {0, 0, 0, 0, 0, 0, 0, maskE08__};
#endif

  // Initialize vector to accumulate the sum
  __m256i sum_vec = _mm256_setzero_si256();

  __v8su mask_sum_B1_V01 = {maskE01__, 0, 0, maskE01__, 0, 0, maskE01__, 0};
  __v8su mask_sum_B2_V01 = {0, maskE01__, 0, 0, maskE01__, 0, 0, maskE01__};
  __v8su mask_sum_B3_V01 = {0, 0, maskE01__, 0, 0, maskE01__, 0, 0};

  __v8su mask_sum_B1_V02 = {maskE02__, 0, 0, maskE02__, 0, 0, maskE02__, 0};
  __v8su mask_sum_B2_V02 = {0, maskE02__, 0, 0, maskE02__, 0, 0, maskE02__};
  __v8su mask_sum_B3_V02 = {0, 0, maskE02__, 0, 0, maskE02__, 0, 0};

  __v8su mask_sum_B1_V03_1 = {maskE03_1, 0, 0, maskE03_1, 0, 0, maskE03_1, 0};
  __v8su mask_sum_B2_V03_1 = {0, maskE03_1, 0, 0, maskE03_1, 0, 0, maskE03_1};
  __v8su mask_sum_B3_V03_1 = {0, 0, maskE03_1, 0, 0, maskE03_1, 0, 0};

  __v8su mask_sum_B1_V03_2 = {0, maskE03_2, 0, 0, maskE03_2, 0, 0, maskE03_2};
  __v8su mask_sum_B2_V03_2 = {0, 0, maskE03_2, 0, 0, maskE03_2, 0, 0};
  __v8su mask_sum_B3_V03_2 = {maskE03_2, 0, 0, maskE03_2, 0, 0, maskE03_2, 0};

  __v8su mask_sum_B1_V04 = {0, maskE04__, 0, 0, maskE04__, 0, 0, maskE04__};
  __v8su mask_sum_B2_V04 = {0, 0, maskE04__, 0, 0, maskE04__, 0, 0};
  __v8su mask_sum_B3_V04 = {maskE04__, 0, 0, maskE04__, 0, 0, maskE04__, 0};

  __v8su mask_sum_B1_V05 = {0, maskE05__, 0, 0, maskE05__, 0, 0, maskE05__};
  __v8su mask_sum_B2_V05 = {0, 0, maskE05__, 0, 0, maskE05__, 0, 0};
  __v8su mask_sum_B3_V05 = {maskE05__, 0, 0, maskE05__, 0, 0, maskE05__, 0};

  __v8su mask_sum_B1_V06_1 = {0, maskE06_1, 0, 0, maskE06_1, 0, 0, maskE06_1};
  __v8su mask_sum_B2_V06_1 = {0, 0, maskE06_1, 0, 0, maskE06_1, 0, 0};
  __v8su mask_sum_B3_V06_1 = {maskE06_1, 0, 0, maskE06_1, 0, 0, maskE06_1, 0};

  __v8su mask_sum_B1_V06_2 = {0, 0, maskE06_2, 0, 0, maskE06_2, 0, 0};
  __v8su mask_sum_B2_V06_2 = {maskE06_2, 0, 0, maskE06_2, 0, 0, maskE06_2, 0};
  __v8su mask_sum_B3_V06_2 = {0, maskE06_2, 0, 0, maskE06_2, 0, 0, maskE06_2};

  __v8su mask_sum_B1_V07 = {0, 0, maskE07__, 0, 0, maskE07__, 0, 0};
  __v8su mask_sum_B2_V07 = {maskE07__, 0, 0, maskE07__, 0, 0, maskE07__, 0};
  __v8su mask_sum_B3_V07 = {0, maskE07__, 0, 0, maskE07__, 0, 0, maskE07__};

  __v8su mask_sum_B1_V08 = {0, 0, maskE08__, 0, 0, maskE08__, 0, 0};
  __v8su mask_sum_B2_V08 = {maskE08__, 0, 0, maskE08__, 0, 0, maskE08__, 0};
  __v8su mask_sum_B3_V08 = {0, maskE08__, 0, 0, maskE08__, 0, 0, maskE08__};

  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    size_t i = 0;
    for (; i < sz_tmp; i += 96) { // Step by 3 bytes per packed12_t

      // Load 32-bit data from arr_tmp. We load 4 bytes at a time, since we're
      // using 32-bit registers.
      __m256i data_W1 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i));
      __m256i data_W2 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 32));
      __m256i data_W3 =
          _mm256_load_si256(reinterpret_cast<__m256i *>(arr_tmp + i + 64));

      __m256i masked_val_V01 = _mm256_or_si256(
          _mm256_or_si256(_mm256_and_si256((__m256i)mask_sum_B1_V01, data_W1),
                          _mm256_and_si256((__m256i)mask_sum_B2_V01, data_W2)),
          _mm256_and_si256((__m256i)mask_sum_B3_V01, data_W3));

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V02 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V02, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V02, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V02, data_W3)),
          12);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V03_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V03_1, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V03_1, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V03_1, data_W3)),
          24);
      __m256i masked_val_V03_2 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V03_2, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V03_2, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V03_2, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V04 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V04, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V04, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V04, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V05 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V05, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V05, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V05, data_W3)),
          16);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V06_1 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V06_1, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V06_1, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V06_1, data_W3)),
          28);
      __m256i masked_val_V06_2 = _mm256_slli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V06_2, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V06_2, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V06_2, data_W3)),
          4);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V07 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V07, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V07, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V07, data_W3)),
          8);

      // Shift the upper bits by 12 (to position them correctly)
      __m256i masked_val_V08 = _mm256_srli_epi64(
          _mm256_or_si256(
              _mm256_or_si256(
                  _mm256_and_si256((__m256i)mask_sum_B1_V08, data_W1),
                  _mm256_and_si256((__m256i)mask_sum_B2_V08, data_W2)),
              _mm256_and_si256((__m256i)mask_sum_B3_V08, data_W3)),
          20);

      // Accumulate the results into sum_vec
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V01);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V02);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V03_1);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V03_2);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V04);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V05);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V06_1);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V06_2);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V07);
      sum_vec = _mm256_add_epi32(sum_vec, masked_val_V08);
    }
    // assert(i == sz_tmp);
  }

  // Perform a horizontal sum to reduce the vector to a scalar
  uint32_t tmp_sum[8];
  _mm256_store_si256(reinterpret_cast<__m256i *>(tmp_sum), sum_vec);

  // Sum the elements in tmp_sum to get the final scalar sum
  for (size_t k = 0; k < 8; k++) {
    sum += tmp_sum[k];
  }

  return sum;
}

size_t __attribute__((noinline))
access_speed_test_16_bit_arr_32bitSum(uint16_t *array16, const size_t size) {
  uint32_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array16[i]);
    }
  }
  return sum;
}

size_t __attribute__((noinline))
access_speed_test_32_bit_arr_32bitSum(uint32_t *array32, const size_t size) {
  uint32_t sum = 0;
  for (size_t j = 0; j < NUM_OF_SCANS; j++) {
    for (size_t i = 0; i < size; i++) {
      sum = (sum + array32[i]);
    }
  }
  return sum;
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
  if (1) {
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
      size_t sum = access_speed_test01(arr_dest, SZ >> 3llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Tim17:%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  if (1) {
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
      size_t sum = access_speed_test02(arr_dest, SZ >> 3llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Tim17:%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  if (1) {
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
      printf("Tim32:%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  if (1) {
    assert((SZ & 7llu) == 0);
    packed9_t *arr_dest =
        static_cast<packed9_t *>(malloc(sizeof(packed9_t) * (SZ >> 3llu)));
    {
      auto start = std::chrono::high_resolution_clock::now();
      memcpy_16_to_9_01(arr, arr_dest);
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
      size_t sum = access_speed_test_16_to_9_01(arr_dest, SZ >> 3llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time9:%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  if (1) {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * SZ));
    {
      auto start = std::chrono::high_resolution_clock::now();
      memcpy_16_to_8_01(arr, arr_dest, SZ);
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
      size_t sum = access_speed_test_16_to_8_01(arr_dest, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time8:%lu, sum = %lu\n", duration.count(), sum);
    }
  }
  {
    assert((SZ & 7llu) == 0);
    packed12_t *arr_dest = static_cast<packed12_t *>(
        aligned_malloc_multiple(sizeof(packed12_t) * (SZ >> 1llu), 32));
    {
      auto start = std::chrono::high_resolution_clock::now();
      memcpy_16_to_12_01(arr, arr_dest);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu\n", duration.count());
    }
    for (size_t i = 0, j = 0; i < SZ; i += 2, j++) {
      assert(arr[i] == arr_dest[j].u.s.E1);
      assert(arr[i + 1] == arr_dest[j].u.s.E2);
    }
    if (1) {
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test(arr, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %lu\n", duration.count(), sum);
    }
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_01(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("1Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_02(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("2Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_03(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("3Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
#if INCORRECT_GPT_SOLUTIONS
    {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_04(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Tim12:%lu, sum = %lu\n", duration.count(), sum);
    }
    {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_05(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Tim12:%lu, sum = %lu\n", duration.count(), sum);
    }
#endif
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_06(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("6Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_07(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("7Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
    if (1) {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      size_t sum = access_speed_test_16_to_12_08(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("8Tm12:%lu, sum = %lu\n", duration.count(), sum);
    }
    {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      uint32_t sum =
          access_speed_test_16_to_12_09_32bitSum(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("9Tm12:%lu, sum = %u\n", duration.count(), sum);
    }
    {
      assert((SZ & 7llu) == 0);
      auto start = std::chrono::high_resolution_clock::now();
      uint32_t sum =
          access_speed_test_16_to_12_10_32bitSum(arr_dest, SZ >> 1llu);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("10T12:%lu, sum = %u\n", duration.count(), sum);
    }
    {
      auto start = std::chrono::high_resolution_clock::now();
      uint32_t sum = access_speed_test_16_bit_arr_32bitSum(arr, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Time :%lu, sum = %u\n", duration.count(), sum);
    }
    {
      uint32_t *arr_dest = static_cast<uint32_t *>(
          aligned_malloc_multiple(sizeof(uint32_t) * SZ, 32));
      {
        auto start = std::chrono::high_resolution_clock::now();
        memcpy_16_to_32_01(arr, arr_dest, SZ);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        printf("Time :%lu\n", duration.count());
      }
      auto start = std::chrono::high_resolution_clock::now();
      uint32_t sum = access_speed_test_32_bit_arr_32bitSum(arr_dest, SZ);
      auto stop = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      printf("Tim32:%lu, sum = %u\n", duration.count(), sum);
    }
  }
  return 0;
}
