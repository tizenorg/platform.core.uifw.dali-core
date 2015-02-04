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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/atomics.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

// Simple single-threaded tests:

int UtcDaliAtomicIncrementAtCacheableAlignedAddressSingleThreadedLoops(void)
{
  const int32_t target = 7907 * 7919;
  int32_t counter = 0;

  for( int i = 0; i < target; ++i )
  {
    Dali::Integration::AtomicIncrementAtCacheableAlignedAddress( &counter );
  }
  DALI_TEST_EQUALS( counter, target, TEST_LOCATION );

  for( int i = 0; i < target; ++i )
  {
    Dali::Integration::AtomicDecrementAtCacheableAlignedAddress( &counter );
  }
  DALI_TEST_EQUALS( counter, 0, TEST_LOCATION );

  END_TEST;
}

// Todo: Spawn a lot of threads and spin hard over these atomics.
// Todo: Do both incs and decs from a lot of threads and spin hard over these atomics.
// Todo: Do both incs and decs from a lot of threads and spin hard over these atomics, accessing an array of variables rather than just one (to induce false-sharing behaviour on the memory monitors which typically alias 2048 byte blocks of memory on 64 bit ARM).
