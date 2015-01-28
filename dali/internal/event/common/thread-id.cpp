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
 */

#include "thread-id.h"
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{
namespace Internal
{

ThreadId::ThreadId()
{
  mThreadId = pthread_self();
}

bool ThreadId::CheckIdentity(const char* identifier) const
{
  bool isSameThread = pthread_equal( mThreadId, pthread_self() );

  if( !isSameThread )
  {
    DALI_LOG_ERROR( "%s called from the wrong thread\n", identifier);

#if defined(ASSERT_ON_BAD_THREAD_ENABLED)
    DALI_ASSERT_ALWAYS( 0 && "API called from wrong thread\n" );
#endif
  }

  return isSameThread;
}

bool ThreadId::CheckIsEventThread(const char* identifier)
{
  bool isEventThread = ThreadLocalStorage::Created();

  if( !isEventThread )
  {
    DALI_LOG_ERROR( "%s called from the wrong thread\n", identifier);

#if defined(ASSERT_ON_BAD_THREAD_ENABLED)
    DALI_ASSERT_ALWAYS( 0 && "API called from wrong thread\n" );
#endif
  }

  return isEventThread;
}

} // namespace Internal
} // namespace Dali
