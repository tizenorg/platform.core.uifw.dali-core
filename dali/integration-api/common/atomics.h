#ifndef __DALI_INTEGRATION_ATOMICS_H_
#define __DALI_INTEGRATION_ATOMICS_H_

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * atomics.h
 *
 * Atomic memory operations for Dali.
 */

// EXTERNAL INCLUDES
#include <stdint.h>
#include <stddef.h>


namespace Dali
{

namespace Integration
{

/**
 * @brief Atomic increment of a value at an aligned memory location in normal cacheable memory.
 *
 * @param address A pointer to a location in a cacheable memory region that is
 *        aligned to a 4 byte boundary.
 */
inline uint32_t AtomicIncrementAtCacheableAlignedAddress( volatile int32_t * const address )
{
  DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );

#ifdef __aarch64__
#pragma message "Compiling with fast inline assembly for atomic add on ARM AArch64"

  // The output register:
  int32_t newValue;
  // Temporary value to hold success or failure of an atomic transaction:
  uint32_t success;

  asm volatile("top:\n\t"
    "ldxr  %[newValue], %[address] \n\t"
    "add   %[newValue], %[one] \n\t"
    "stxr  %[success],  %[newValue], %[address] \n\t"
    "cbnz  %[success],  topb \n\t"
    // Outputs:
    : [newValue] "=&r" (newValue),
      // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
      [address] "+Q" (*address),
    // Temporaries represented as outputs to let gcc allocate any free register:
      [success] "=&r" (success),
    // Inputs:
    : [one] "Ir" (1)
    // Clobbered: I.e., stuff that is modified. A "memory" clobber should inject a
    // data memory barrier. Indeed, the Linux smp_mb() barrier expands to an empty
    // asm statement with nothing but a memory clobber.
    : "memory"
  );

  return newValue;

#else
  // gcc > 4.1 builtin atomic add and fetch (++mCount; return mCount)
  return __sync_add_and_fetch( address, 1 );
#endif
}


/**
 * @brief Atomic decrement of a value at an aligned memory location in normal cacheable memory.
 *
 * @param address A pointer to a location in a cacheable memory region that is
 *        aligned to a 4 byte boundary.
 */
inline int32_t AtomicDecrementAtCacheableAlignedAddress( volatile int32_t * const address )
{
  DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );
#ifdef __aarch64__
#pragma message "Compiling with fast atomic inline assembly for atomic subtract on ARM AArch64"

  // The output register:
  int32_t newValue;
  // Temporary value to hold success or failure of an atomic transaction:
  uint32_t success;

  asm volatile("top:\n\t"
    "ldxr  %[newValue], %[address] \n\t"
    "sub   %[newValue], %[one] \n\t"
    "stxr  %[success],  %[newValue], %[address] \n\t"
    "cbnz  %[success],  topb \n\t"
    // Outputs:
    : [newValue] "=&r" (newValue),
      // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
      [address] "+Q" (*address),
    // Temporaries represented as outputs to let gcc allocate any free register:
      [success] "=&r" (success),
    // Inputs:
    : [one] "Ir" (1)
    // Clobbered: I.e., stuff that is modified. A "memory" clobber should inject a
    // data memory barrier. Indeed, the Linux smp_mb() barrier expands to an empty
    // asm statement with nothing but a memory clobber.
    : "memory"
  );

  return newValue;

#else
  // gcc > 4.1 builtin atomic subtract and fetch:
  return __sync_sub_and_fetch( address, 1 );
#endif
}

/**
 * @brief Atomic write to an aligned memory location in cacheable memory.
 *
 * For common platforms with coherent caches such as ARM mpcore and Intel CPUs,
 * a cacheline can be in a writeable state in the L1 cache of exactly one core
 * at a time. Therefore, a write to a location that does not require a read /
 * modify / write cycle or cross a cacheline boundary is  automatically
 * atomic.
 *
 * @param address A pointer to a location in a cacheable memory region that is
 *        aligned to a 4 byte boundary.
 * @param value A value to assign to the memory location in address.
 *
 * @note This is atomic with respect to other calls to itself and to
 * AtomicReadFromCacheableAlignedAddress, but not with respect to locations
 * accessed via AtomicAddAtCacheableAlignedAddress and
 * AtomicDecrementAtCacheableAlignedAddress.
 */
inline void AtomicWriteToCacheableAlignedAddress( volatile uint32_t * const address, uint32_t value )
{
  DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );
  *address = value;
}

/**
 * @brief Atomic read from an aligned memory location in cacheable memory.
 *
 * For common platforms with coherent caches such as ARM mpcore and Intel CPUs,
 * a cacheline can be in a writeable state in the L1 cache of exactly one core
 * at a time. Therefore, a read a location that does not cross a cacheline
 * boundary is automatically atomic.
 *
 * @param  address A pointer to a location in a cacheable memory region that is
 *         aligned to a 4 byte boundary.
 * @return The value stored at the memory location in address.
 *
 * @note This is atomic with respect to other calls to itself and to
 * AtomicWriteToCacheableAlignedAddress, but not with respect to access via
 * AtomicAddAtCacheableAlignedAddress and
 * AtomicDecrementAtCacheableAlignedAddress to the same location.
 */
inline uint32_t AtomicReadFromCacheableAlignedAddress( const volatile uint32_t * const address )
{
  DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );
  return *address;
}

} /* namespace Integration */
} /* namespace Dali */

#endif /* __DALI_INTEGRATION_ATOMICS_H_ */
