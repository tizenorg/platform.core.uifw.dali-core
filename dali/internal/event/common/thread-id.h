#ifndef DALI_INTERNAL_THREAD_ID_H
#define DALI_INTERNAL_THREAD_ID_H

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

#include <pthread.h>

namespace Dali
{
namespace Internal
{

/**
 * Class to store the current thread id. Provides a method to check against the calling
 * thread.
 */
class ThreadId
{
public:
  /**
   * Constructor
   */
  ThreadId();

  /**
   * Returns true if the calling thread is the same as the stored thread identity.
   */
  bool CheckIdentity() const;

private:
  pthread_t mThreadId;
};


}
}

#endif // DALI_INTERNAL_THREAD_ID_H
