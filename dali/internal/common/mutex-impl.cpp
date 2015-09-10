/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali/internal/common/mutex-impl.h>

// EXTERNAL INCLUDES
#ifdef LOCK_BACKTRACE_ENABLED
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <execinfo.h>
#include <cxxabi.h>
#endif // LOCK_BACKTRACE_ENABLED

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
#ifdef LOCK_BACKTRACE_ENABLED
extern std::string Demangle( const char* symbol );
#endif // LOCK_BACKTRACE_ENABLED

namespace Internal
{

namespace Mutex
{

namespace
{
#ifdef LOCK_BACKTRACE_ENABLED

// Constants
const unsigned int MAX_NUM_STACK_FRAMES = 5;
const unsigned int MAX_LOCK_SUPPORT = 5;
const unsigned int TYPICAL_BACKTRACE_STRING_SIZE = 1000;
const unsigned int BACKTRACE_STRING_SIZE = MAX_LOCK_SUPPORT * TYPICAL_BACKTRACE_STRING_SIZE;

// Thread Local Storage
__thread char gBackTraceString[ BACKTRACE_STRING_SIZE ] = { 0, };
__thread char * gBackTraceStringPtr = NULL;
__thread unsigned int gLastBackTraceCount[ MAX_LOCK_SUPPORT ];

#endif // LOCK_BACKTRACE_ENABLED

__thread unsigned int gThreadLockCount = 0;
} // unnamed namespace

void Lock( pthread_mutex_t* mutex )
{
  ++gThreadLockCount;

#ifdef LOCK_BACKTRACE_ENABLED

  DALI_ASSERT_ALWAYS( gThreadLockCount <= MAX_LOCK_SUPPORT && "Too many locks held for back trace" );

  // Reset to start of string, no need to set string to NULL as that's done in Unlock
  if( gThreadLockCount == 1 )
  {
    gBackTraceStringPtr = gBackTraceString;
  }

  int backTraceIndex = gThreadLockCount - 1;

  gLastBackTraceCount[ backTraceIndex ] = sprintf( gBackTraceStringPtr, "\n[Lock %d]\n", gThreadLockCount );
  gBackTraceStringPtr += gLastBackTraceCount[ backTraceIndex ];

  void* frameArray[ MAX_NUM_STACK_FRAMES ];
  int nSize = backtrace( frameArray, MAX_NUM_STACK_FRAMES );
  char** symbols = backtrace_symbols( frameArray, nSize );
  for( int i = 1; i < nSize; i++ )
  {
    std::string demangled_symbol = Demangle( symbols[ i ] );
    int added = sprintf( gBackTraceStringPtr, "  [%02d] %s\n", i, demangled_symbol.c_str() );
    gBackTraceStringPtr += added;
    gLastBackTraceCount[ backTraceIndex ] += added;
  }
  free(symbols);

  // If we've got more than one lock, then show a warning with a backtrace for all locks that we currently hold
  if( gThreadLockCount > 1 )
  {
    DALI_LOG_WARNING( "%s====================================\n", gBackTraceString );
  }

#else

  // TODO: Uncomment when locks held per thread at any given time are 1
  //DALI_ASSERT_DEBUG( gThreadLockCount == 1 );

#endif // LOCK_BACKTRACE_ENABLED

  pthread_mutex_lock( mutex );
}

void Unlock( pthread_mutex_t* mutex )
{
  pthread_mutex_unlock( mutex );
  --gThreadLockCount;

#ifdef LOCK_BACKTRACE_ENABLED
  // Update our location within the string
  gBackTraceStringPtr -= gLastBackTraceCount[ gThreadLockCount ];

  // Ensure that the data in the array that we are no longer interested in is all set to NULL
  memset( gBackTraceStringPtr, 0, gLastBackTraceCount[ gThreadLockCount ] );
#endif // USE_BACKTRACE
}

} // namespace Mutex

} // namespace Internal

} // namespace Dali
