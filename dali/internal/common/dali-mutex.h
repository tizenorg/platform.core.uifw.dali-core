#ifndef __DALI_MUTEX_H__
#define __DALI_MUTEX_H__

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

#include <dali/public-api/common/dali-common.h>

#if defined(MUTEX_LOCKS_ENABLED)

#include <boost/thread/mutex.hpp>

namespace Dali
{
namespace Internal
{

typedef boost::mutex MessageQueueMutex;

} // namespace Internal
} // namespace Dali

#else

namespace Dali
{
namespace Internal
{

struct MessageQueueMutex
{
  struct scoped_lock
  {
    scoped_lock(MessageQueueMutex& /* mutex */)
    {
    }
  };
};

} // namespace Internal
} // namespace Dali

#endif


#endif // header
