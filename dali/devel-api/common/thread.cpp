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

// CLASS HEADER
#include <dali/devel-api/common/thread.h>

// EXTERNAL INCLUDES
#include <pthread.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

struct Thread::ThreadImpl
{
  pthread_t thread;
  Thread::State state;
};

Thread::Thread()
: mImpl( new ThreadImpl )
{
  mImpl->state = Thread::STATE_NONE;
}

Thread::~Thread()
{
  delete mImpl;
}

void Thread::Start()
{
  DALI_ASSERT_DEBUG( mImpl->state == STATE_NONE  );

  int error = pthread_create( &(mImpl->thread), NULL, InternalThreadEntryFunc, this );
  DALI_ASSERT_ALWAYS( !error && "Failed to create a new thread" );

  mImpl->state = STATE_STARTED;
}

void Thread::Join()
{
  DALI_ASSERT_DEBUG( mImpl->state == STATE_STARTED  );

  pthread_join( mImpl->thread, NULL );
  mImpl->state = STATE_JOINED;
}

Thread::State Thread::GetCurrentState() const
{
  return mImpl->state;
}

} // namespace Dali
