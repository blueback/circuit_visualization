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

typedef struct {
  char *arr;
  size_t size;
  size_t capacity;
} EdgeSynArr;

EdgeSynArr esa;

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

#if 0
uint32_t addEdge() {
    if (esa.size < esa.capacity) {
       // size_t index_size = log
    }
}
#endif

#define SZ (1lu << 28)

__attribute__((noinline)) void fill(uint8_t *arr) {
  for (size_t i = 0; i < SZ; i++) {
    arr[i] = i % 256;
  }
}

__attribute__((noinline)) void fill(uint16_t *arr) {
  for (size_t i = 0; i < SZ; i++) {
    arr[i] = i % 256;
  }
}

__attribute__((noinline)) void copyarr1(uint8_t *arr, uint8_t *arr2) {
  memcpy(arr2, arr, SZ);
}

__attribute__((noinline)) void copyarr2(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ / 16; i++) {
    (reinterpret_cast<__uint128_t *>(arr2))[i] =
        (reinterpret_cast<__uint128_t *>(arr))[i];
  }
}

__attribute__((noinline)) void copyarr3(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i++) {
    arr2[i] = arr[i];
  }
}

__attribute__((noinline)) void copyarr4(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 8) {
    for (size_t j = i; j < i + 8; j++) {
      arr2[j] = arr[j];
    }
  }
}

__attribute__((noinline)) void copyarrINC5(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i++) {
    arr2[i * 2] = arr[i];
  }
}

__attribute__((noinline)) void copyarrINC6(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 8) {
    for (size_t j = i; j < i + 8; j++) {
      arr2[2 * j] = arr[j];
    }
  }
}

__attribute__((noinline)) void copyarrINC7(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 4) {
    arr2[2 * i] = arr[i];
    arr2[2 * i + 2] = arr[i + 1];
    arr2[2 * i + 4] = arr[i + 2];
    arr2[2 * i + 6] = arr[i + 3];
  }
}

__attribute__((noinline)) void copyarrINC8(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 8) {
    arr2[2 * i] = arr[i];
    arr2[2 * i + 2] = arr[i + 1];
    arr2[2 * i + 4] = arr[i + 2];
    arr2[2 * i + 6] = arr[i + 3];
    arr2[2 * i + 8] = arr[i + 4];
    arr2[2 * i + 10] = arr[i + 5];
    arr2[2 * i + 12] = arr[i + 6];
    arr2[2 * i + 14] = arr[i + 7];
  }
}

__attribute__((noinline)) void copyarrINC9(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 16) {
    arr2[2 * i] = arr[i];
    arr2[2 * i + 2] = arr[i + 1];
    arr2[2 * i + 4] = arr[i + 2];
    arr2[2 * i + 6] = arr[i + 3];
    arr2[2 * i + 8] = arr[i + 4];
    arr2[2 * i + 10] = arr[i + 5];
    arr2[2 * i + 12] = arr[i + 6];
    arr2[2 * i + 14] = arr[i + 7];
    arr2[2 * i + 16] = arr[i + 8];
    arr2[2 * i + 18] = arr[i + 9];
    arr2[2 * i + 20] = arr[i + 10];
    arr2[2 * i + 22] = arr[i + 11];
    arr2[2 * i + 24] = arr[i + 12];
    arr2[2 * i + 26] = arr[i + 13];
    arr2[2 * i + 28] = arr[i + 14];
    arr2[2 * i + 30] = arr[i + 15];
  }
}

__attribute__((noinline)) void copyarrINC10(uint8_t *arr, uint8_t *arr2) {
  for (size_t i = 0; i < SZ; i += 2) {
    arr2[2 * i] = arr[i];
    arr2[2 * i + 2] = arr[i + 1];
  }
}

__attribute__((noinline)) void
fast_memcpy_uint8_to_uint16(const uint8_t *src, uint16_t *dst, size_t count) {
  size_t i = 0;

  // Process unaligned prefix
  while (((uintptr_t)(src + i) % 32 != 0 || (uintptr_t)(dst + i) % 32 != 0) &&
         i < count) {
    dst[i] = (uint16_t)src[i];
    i++;
  }

  // Process aligned SIMD blocks (32 uint8_t -> 16 uint16_t per iteration)
  for (; i + 32 <= count; i += 32) {
    // Load 32 bytes from source (aligned)
    __m256i data = _mm256_load_si256((const __m256i *)(src + i));

    // Zero-extend to 16-bit
    __m256i lower = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 0));
    __m256i upper = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 1));

    // Store 16 uint16_t elements to destination (aligned)
    _mm256_store_si256((__m256i *)(dst + i), lower);
    _mm256_store_si256((__m256i *)(dst + i + 16), upper);
  }

  // Process remaining elements (suffix)
  for (; i < count; i++) {
    dst[i] = (uint16_t)src[i];
  }
}

__attribute__((noinline)) void
fast_memcpy_uint8_to_uint16_alligned(const uint8_t *src, uint16_t *dst,
                                     size_t count) {
  size_t i = 0;

  // Process 32 bytes at a time (32 uint8_t -> 16 uint16_t) using AVX2
  for (; i + 32 <= count; i += 32) {
    // Load 32 uint8_t elements from source
    __m256i data = _mm256_loadu_si256((const __m256i *)(src + i));

    // Zero-extend to 16-bit
    __m256i lower = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 0));
    __m256i upper = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 1));

    // Store 16 uint16_t elements to destination
    _mm256_storeu_si256((__m256i *)(dst + i), lower);
    _mm256_storeu_si256((__m256i *)(dst + i + 16), upper);
  }

  // Process remaining elements
  for (; i < count; i++) {
    dst[i] = (uint16_t)src[i];
  }
}

__attribute__((noinline)) void
fast_memcpy_uint8_to_uint16_alligned_multiple(const uint8_t *src, uint16_t *dst,
                                              size_t count) {
  size_t i = 0;

  // Process 32 bytes at a time (32 uint8_t -> 16 uint16_t) using AVX2
  for (; i + 32 <= count; i += 32) {
    // Load 32 uint8_t elements from source
    __m256i data = _mm256_load_si256((const __m256i *)(src + i));

    // Zero-extend to 16-bit
    __m256i lower = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 0));
    __m256i upper = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(data, 1));

    // Store 16 uint16_t elements to destination
    _mm256_store_si256((__m256i *)(dst + i), lower);
    _mm256_store_si256((__m256i *)(dst + i + 16), upper);
  }
}

__attribute__((noinline)) void memcpy_16_to_32_01(const uint16_t *src,
                                                  uint32_t *dst, size_t count) {
  for (size_t i = 0; i < count; i++) {
    dst[i] = src[i];
  }
}

__attribute__((noinline)) void memcpy_16_to_32_02(const uint16_t *src,
                                                  uint32_t *dst, size_t count) {
  size_t i = 0;

  // Process 32 bytes at a time (16 uint16_t -> 8 uint32_t) using AVX2
  for (; i + 16 <= count; i += 16) {
    // Load 32 uint8_t elements from source
    __m256i data = _mm256_load_si256((const __m256i *)(src + i));

    // Zero-extend to 16-bit
    __m256i lower = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(data, 0));
    __m256i upper = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(data, 1));

    // Store 16 uint16_t elements to destination
    _mm256_store_si256((__m256i *)(dst + i), lower);
    _mm256_store_si256((__m256i *)(dst + i + 8), upper);
  }
}

__attribute__((noinline)) void memcpy_16_to_32_03(const uint16_t *src,
                                                  uint32_t *dst, size_t count) {
  for (size_t i = 0; i < count; i += 2) {
    dst[i] = src[i];
    dst[i + 1] = src[i + 1];
  }
}

__attribute__((noinline)) void memcpy_16_to_32_04(const uint16_t *src,
                                                  uint32_t *dst, size_t count) {
  for (size_t i = 0; i < count; i += 4) {
    dst[i] = src[i];
    dst[i + 1] = src[i + 1];
    dst[i + 2] = src[i + 2];
    dst[i + 3] = src[i + 3];
  }
}

int main() {
  uint8_t *arr = static_cast<uint8_t *>(malloc(SZ));
  fill(arr);
  // for (size_t i = 120; i < 150; i++) {
  //   printf("arr[%lu] = %hhu\n", i, arr[i]);
  // }
#if 0
  {
#define ALLIGNER (8)
    size_t arr_ptr = reinterpret_cast<size_t>(arr);

    uint8_t *arr_end = arr + SZ;

    if (arr + 1 % 8 == 0) {
      arr_start = arr_start + 1;
      arr_end = arr + SZ - 7;
    } else if (arr + 2 % 8 == 0) {
      arr_start = arr_start + 2;
      arr_end = arr + SZ - 6;
    }
  }
#endif
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarr1(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    assert(memcmp(arr, arr_dest, SZ) == 0);
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarr2(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    assert(memcmp(arr, arr_dest, SZ) == 0);
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarr3(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    assert(memcmp(arr, arr_dest, SZ) == 0);
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarr4(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    assert(memcmp(arr, arr_dest, SZ) == 0);
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC5(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC6(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC7(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC8(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC9(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint8_t *arr_dest = static_cast<uint8_t *>(malloc(2 * SZ));
    auto start = std::chrono::high_resolution_clock::now();
    copyarrINC10(arr, arr_dest);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("Time :%lu\n", duration.count());
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[2 * i]);
    }
  }
  {
    uint16_t *arr_dest = static_cast<uint16_t *>(malloc(2 * SZ));
    uint8_t *arr_dest_uint8_t = reinterpret_cast<uint8_t *>(arr_dest);
    auto start = std::chrono::high_resolution_clock::now();
    fast_memcpy_uint8_to_uint16(arr, arr_dest, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("fast_memcpy_uint8_to_uint16 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("arr[%lu] = %hhu, arr_dest[%lu] = %hu\n", i, arr[i], i,
    //          arr_dest[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(arr[i] == arr_dest_uint8_t[2 * i]);
    }
  }
  {
    uint8_t *src =
        static_cast<uint8_t *>(aligned_malloc(sizeof(uint8_t) * SZ, 32));
    uint16_t *dst =
        static_cast<uint16_t *>(aligned_malloc(sizeof(uint16_t) * SZ, 32));
    uint8_t *dst_uint8_t = reinterpret_cast<uint8_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    fast_memcpy_uint8_to_uint16_alligned(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("fast_memcpy_uint8_to_uint16_alligned Time :%lu\n",
           duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint8_t[2 * i]);
    }
  }
  {
    uint8_t *src = static_cast<uint8_t *>(
        aligned_malloc_multiple(sizeof(uint8_t) * SZ, 32));
    uint16_t *dst = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint8_t *dst_uint8_t = reinterpret_cast<uint8_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    fast_memcpy_uint8_to_uint16_alligned_multiple(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("fast_memcpy_uint8_to_uint16_alligned_multiple Time :%lu\n",
           duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint8_t[2 * i]);
    }
  }
  {
    uint16_t *src = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint32_t *dst = static_cast<uint32_t *>(
        aligned_malloc_multiple(sizeof(uint32_t) * SZ, 32));
    uint16_t *dst_uint16_t = reinterpret_cast<uint16_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    memcpy_16_to_32_01(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("memcpy_16_to_32_01 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint16_t[2 * i]);
    }
  }
  {
    uint16_t *src = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint32_t *dst = static_cast<uint32_t *>(
        aligned_malloc_multiple(sizeof(uint32_t) * SZ, 32));
    uint16_t *dst_uint16_t = reinterpret_cast<uint16_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    memcpy_16_to_32_02(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("memcpy_16_to_32_02 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint16_t[2 * i]);
    }
  }
  {
    uint16_t *src = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint32_t *dst = static_cast<uint32_t *>(
        aligned_malloc_multiple(sizeof(uint32_t) * SZ, 32));
    uint16_t *dst_uint16_t = reinterpret_cast<uint16_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    memcpy_16_to_32_03(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("memcpy_16_to_32_03 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint16_t[2 * i]);
    }
  }
  {
    uint16_t *src = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint16_t *dst = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    memcpy(dst, src, SZ * sizeof(uint16_t));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("memcpy_16_to_16_01 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
  }
  {
    uint16_t *src = static_cast<uint16_t *>(
        aligned_malloc_multiple(sizeof(uint16_t) * SZ, 32));
    uint32_t *dst = static_cast<uint32_t *>(
        aligned_malloc_multiple(sizeof(uint32_t) * SZ, 32));
    uint16_t *dst_uint16_t = reinterpret_cast<uint16_t *>(dst);
    fill(src);
    auto start = std::chrono::high_resolution_clock::now();
    memcpy_16_to_32_04(src, dst, SZ);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("memcpy_16_to_32_04 Time :%lu\n", duration.count());
    // for (size_t i = 0; i < 512; i++) {
    //   printf("src[%lu] = %hhu, dst[%lu] = %hu\n", i, src[i], i, dst[i]);
    // }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst[i]);
    }
    for (size_t i = 0; i < SZ; i++) {
      assert(src[i] == dst_uint16_t[2 * i]);
    }
  }
  return 0;
}
