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

// CLASS HEADER
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <string>
#include <cstdio>
//#include <execinfo.h>
#include <platform-abstractions/android/nativeLogging.h>

#ifndef EMSCRIPTEN // cxxabi not supported
# include <cxxabi.h>
#endif

#include <cstring>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{
const int MAX_NUM_STACK_FRAMES = 25;
const size_t C_SYMBOL_LENGTH = 4096;
}

namespace Dali
{

#ifndef EMSCRIPTEN

std::string Demangle(const char* symbol)
{
  return symbol;
}

#endif // EMSCRIPTEN

#ifndef EMSCRIPTEN

DALI_EXPORT_API DaliException::DaliException(const char *location, const char* condition)
: mLocation(location), mCondition(condition)
{
  // Note, if a memory error has occured, then the backtrace won't work - backtrace_symbols relies on
  // allocating memory.

  // Initial dlog error message is output in DALI_ASSERT_ALWAYS macro
  // Also output on stderr
  LOGE("Exception: \n%s\n thrown at %s\nSee dlog for backtrace\n", mCondition.c_str(), mLocation.c_str());
  LOGE("Backtrace:\n");
}

#else

DALI_EXPORT_API DaliException::DaliException(const char *location, const char* condition)
: mLocation(location), mCondition(condition)
{
  printf("Exception: \n%s\n thrown at %s\nSee dlog for backtrace\n", mCondition.c_str(), mLocation.c_str());
}

#endif // EMSCRIPTEN

DALI_EXPORT_API void DaliAssertMessage(const char* condition, const char* file, int line)
{
  LOGE( "Assertion (%s) failed in: %s:%d\n", condition, file, line );
}

} // Dali
