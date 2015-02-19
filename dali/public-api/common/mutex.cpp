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
#include <dali/public-api/common/mutex.h>

// EXTERNAL INCLUDES
#include <pthread.h>

namespace Dali
{

struct Mutex::MutexImpl
{
  pthread_mutex_t mutex;
  bool locked;
};

Mutex::Mutex()
: mImpl( new MutexImpl )
{
  pthread_mutex_init( &mImpl->mutex, NULL );
  mImpl->locked = false;
}

Mutex::~Mutex()
{
  Unlock();
  delete mImpl;
}

void Mutex::Lock()
{
  pthread_mutex_lock( &mImpl->mutex );
  mImpl->locked = true;
}

void Mutex::Unlock()
{
  pthread_mutex_unlock( &mImpl->mutex );
  mImpl->locked = false;
}

bool Mutex::IsLocked()
{
  return mImpl->locked;
}

} // namespace Dali
