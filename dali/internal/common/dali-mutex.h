#ifndef DALI_MUTEX_H
#define DALI_MUTEX_H

#include <dali/public-api/common/dali-common.h>

#if !defined(EMSCRIPTEN)

#include <boost/thread/mutex.hpp>

namespace Dali
{
namespace Internal
{
typedef boost::mutex MessageQueueMutex;
}
}

#else

namespace Dali
{
namespace Internal
{
 struct MessageQueueMutex;
  struct MessageQueueMutex
  {
    struct scoped_lock
    {
      scoped_lock(MessageQueueMutex& /* mutex */)
      {
      }
    };
  };
}
}

#endif


#endif // header
