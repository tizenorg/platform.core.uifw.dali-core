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

#define DALI_ATOMIC_INCREMENT( ADDRESS, OUT_NEW_VALUE ) \
{ \
  volatile int32_t * const address = (ADDRESS);  \
  int32_t newValue;  \
  uint32_t success;  \
  \
  asm volatile(  \
"1:\tldxr  %w[newValue], %[address] \n\t"  \
      "add   %w[newValue], %w[newValue], %[one] \n\t"  \
      "stxr  %w[success],  %w[newValue], %[address] \n\t"  \
      "cbnz  %w[success],  1b \n\t"  \
    : [newValue] "=&r" (newValue),  \
      [address] "+Q" (*address),  \
      [success] "=&r" (success)  \
    : [one] "Ir" (1)  \
  );  \
  \
  (OUT_NEW_VALUE) = newValue;  \
}

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
__attribute__((always_inline))
inline int32_t AtomicIncrementAtCacheableAlignedAddress( volatile int32_t * const address )
{
  ///////////////////// TEMP DISABLE DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );

///@ToDo: expand this to be used on AArch32 and ARMv7
#ifdef __aarch64__
  // The output register:
  int32_t newValue;
  // Temporary value to hold success or failure of an atomic transaction:
  uint32_t success;

  asm volatile(
"1:\tldxr  %w[newValue], %[address] \n\t"
      "add   %w[newValue], %w[newValue], %[one] \n\t"
      "stxr  %w[success],  %w[newValue], %[address] \n\t"
      "cbnz  %w[success],  1b \n\t"
    // Outputs:
    : [newValue] "=&r" (newValue),
      // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
      [address] "+Q" (*address),
    // Temporaries represented as outputs to let gcc allocate any free register:
      [success] "=&r" (success)
    // Inputs:
    : [one] "Ir" (1)
    // Clobbered: I.e., stuff that is modified. A "memory" clobber should inject a
    // data memory barrier. Indeed, the Linux smp_mb() barrier expands to an empty
    // asm statement with nothing but a memory clobber.
    : //> Temp --- : "memory"
  );

  return newValue;

#else
  // gcc > 4.1 builtin atomic add and fetch:
  return __sync_add_and_fetch( address, 1 );
#endif
}

__attribute__((always_inline))
// No return value, and hard 2 registers used for temporaries.
inline void AtomicIncrementAtCacheableAlignedAddress2( volatile int32_t * const address )
{
  ///////////////////// TEMP DISABLE DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );

///@ToDo: expand this to be used on AArch32 and ARMv7
#ifdef __aarch64__

  asm volatile(
    "1:\tldxr  w1, %[address] \n\t"
        "add   w1, w1, %[one] \n\t"
        "stxr  w2,  w1, %[address] \n\t"
        "cbnz  w2,  1b \n\t"
      // Outputs:
      : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
        [address] "+Q" (*address)
      // Inputs:
      : [one] "Ir" (1)
      // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
      : "w1", "w2"
    );

#else
  // gcc > 4.1 builtin atomic add and fetch:
  __sync_add_and_fetch( address, 1 );
#endif
}

/**
 * @brief Atomic decrement of a value at an aligned memory location in normal cacheable memory.
 *
 * @param address A pointer to a location in a cacheable memory region that is
 *        aligned to a 4 byte boundary.
 */
__attribute__((always_inline))
inline int32_t AtomicDecrementAtCacheableAlignedAddress( volatile int32_t * const address )
{
  DALI_ASSERT_DEBUG( ptrdiff_t(address) % 4 == 0 && "Address must be 32 bit aligned" );
#ifdef __aarch64__
  // The output register:
  int32_t newValue;
  // Temporary value to hold success or failure of an atomic transaction:
  uint32_t success;

  asm volatile(
  "1:\tldxr  %w[newValue], %[address] \n\t"
        "sub   %w[newValue], %w[newValue], %[one] \n\t"
        "stxr  %w[success],  %w[newValue], %[address] \n\t"
        "cbnz  %w[success],  1b \n\t"
      // Outputs:
      : [newValue] "=&r" (newValue),
        // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
        [address] "+Q" (*address),
      // Temporaries represented as outputs to let gcc allocate any free register:
        [success] "=&r" (success)
      // Inputs:
      : [one] "Ir" (1)
      // Clobbered: I.e., stuff that is modified. A "memory" clobber should inject a
      // data memory barrier. Indeed, the Linux smp_mb() barrier expands to an empty
      // asm statement with nothing but a memory clobber.
      :  //> Temp --- : : "memory"
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

/**
 * @defgroup AtomicsTiming Timing tests for atomics.
 *
 * define DALI_INTEGRATION_ATOMICS_TIMING_TEST to enable some timing tests for you to call.
 * @{
 */
#ifdef DALI_INTEGRATION_ATOMICS_TIMING_TEST
#include <time.h>

namespace DaliAtomicsTimingTests
{

extern volatile int32_t counter;
volatile int32_t counter = 0;

inline void InitTimer( timespec& timer )
{
  clock_gettime( CLOCK_MONOTONIC, &timer );
}

inline double ToNanos( timespec& timer )
{
  const double nanos = timer.tv_sec * 1000000000.0 + timer.tv_nsec;
  return nanos;
}

inline double ElapsedNanos( timespec& base )
{
  timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );
  return ToNanos( now ) - ToNanos( base );
}

inline void Incs( unsigned int numIncs, const char * const log )
{
  DALI_ASSERT_DEBUG( log );

  FILE* logfile = fopen( log, "a");
  if( !logfile )
  {
    return;
  }

  const unsigned int numIters = numIncs / 20;
  const unsigned int numTests = numIters * 20;

  if( ptrdiff_t(&counter) % 4 != 0 )
  {
    fprintf( logfile, "FAILED: unaligned counter at " __FILE__ ", line (%u)\n", __LINE__ );
    fclose( logfile );
  }

  // Get a baseline after a warmup run:
  double baselineNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      // 20 incs to amortise loop overhead:
      ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter;
      ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter; ++counter;
    }
    baselineNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, ++, %f, %u, %d, %f\n", baselineNanos, numTests, counter, baselineNanos / numTests );

  // Get results for gcc builtin after a warmup run:
  double gccNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      // 20 incs to amortise loop overhead:
      __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 );
      __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 );
      __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 );
      __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 );
      __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 ); __sync_add_and_fetch( &counter, 1 );
    }
    gccNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, gcc, %f, %u, %d, %f\n", gccNanos, numTests, counter, gccNanos / numTests );

  // Get results for our implementation after a warmup run:
  double daliNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      // 20 incs to amortise loop overhead:
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
    }
    daliNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, inline, %f, %u, %d, %f\n", daliNanos, numTests, counter, daliNanos / numTests );


  // Get results for our second implementation after a warmup run:
  double daliHardRegsNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      // 20 incs to amortise loop overhead:
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
      Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter ); Dali::Integration::AtomicIncrementAtCacheableAlignedAddress2( &counter );
    }
    daliHardRegsNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, inline-hard-regs, %f, %u, %d, %f\n", daliHardRegsNanos, numTests, counter, daliHardRegsNanos / numTests );

  // Get results for our implementation done as a macro after a warmup run:
  double macroNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      int32_t newValue;
      // 20 incs to amortise loop overhead:
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
      DALI_ATOMIC_INCREMENT( &counter, newValue );
    }
    macroNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, macro, %f, %u, %d, %f\n", macroNanos, numTests, counter, macroNanos / numTests );


  // Get results for our implementation done as a load of pasted assembly a warmup run:
  double pastedNanos = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      int32_t newValue;
      int32_t success;
      // 20 incs to amortise loop overhead:
      asm volatile(
      "1:\tldxr  %w[newValue], %[address] \n\t"
          "add   %w[newValue], %w[newValue], %[one] \n\t"
          "stxr  %w[success],  %w[newValue], %[address] \n\t"
          "cbnz  %w[success],  1b \n\t"
        // Outputs:
        : [newValue] "=&r" (newValue),
          // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
          [address] "+Q" (counter),
        // Temporaries represented as outputs to let gcc allocate any free register:
          [success] "=&r" (success)
        // Inputs:
        : [one] "Ir" (1)
      );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
      asm volatile(
            "1:\tldxr  %w[newValue], %[address] \n\t"
                "add   %w[newValue], %w[newValue], %[one] \n\t"
                "stxr  %w[success],  %w[newValue], %[address] \n\t"
                "cbnz  %w[success],  1b \n\t"
              // Outputs:
              : [newValue] "=&r" (newValue),
                // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter),
              // Temporaries represented as outputs to let gcc allocate any free register:
                [success] "=&r" (success)
              // Inputs:
              : [one] "Ir" (1)
            );
    }
    pastedNanos = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, pasted, %f, %u, %d, %f\n", pastedNanos, numTests, counter, pastedNanos / numTests );


  // Get results for our implementation done as a load of pasted assembly a warmup run:
  // This assembly commits to particular registers and forces gcc to allocate variables to avoid them in C++ code.
  double pastedNanos2 = 0;
  for( int runs = 0; runs < 2; ++runs )
  {
    counter = 0;
    timespec start;
    InitTimer( start );
    for( unsigned int i = 0; i < numIters; ++i )
    {
      // 20 incs to amortise loop overhead:
      asm volatile(
      "1:\tldxr  w1, %[address] \n\t"
          "add   w1, w1, %[one] \n\t"
          "stxr  w2,  w1, %[address] \n\t"
          "cbnz  w2,  1b \n\t"
        // Outputs:
        : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
          [address] "+Q" (counter)
        // Inputs:
        : [one] "Ir" (1)
        // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
        : "w1", "w2"
      );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
      asm volatile(
            "1:\tldxr  w1, %[address] \n\t"
                "add   w1, w1, %[one] \n\t"
                "stxr  w2,  w1, %[address] \n\t"
                "cbnz  w2,  1b \n\t"
              // Outputs:
              : // Q = A memory address with no offset ( https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints )
                [address] "+Q" (counter)
              // Inputs:
              : [one] "Ir" (1)
              // Clobbers: (explicitly clobber w1 register to hold the loaded value and register w2 to hold success/fail that we ignore):
              : "w1", "w2"
            );
    }
    pastedNanos2 = ElapsedNanos( start );
  }

  /// File format is csv: "operation", "method", "total nanos", "tests", "counter",  "nanos"
  fprintf( logfile, "inc, pasted2, %f, %u, %d, %f\n", pastedNanos2, numTests, counter, pastedNanos2 / numTests );

  ///@ToDo: Try pasting the assembly 20 times:
  ///@ToDo: Try pasting the assembly 20 times and make new value a clobber, not an output:







  fflush( logfile );
  fclose( logfile );
}

inline void DoAll( unsigned int numIters, const char * const log )
{
  Incs( numIters, log );
}

}

#endif /// @} DALI_INTEGRATION_ATOMICSDALI_INTEGRATION_ATOMICS_TIMING_TEST

#endif /* __DALI_INTEGRATION_ATOMICS_H_ */
