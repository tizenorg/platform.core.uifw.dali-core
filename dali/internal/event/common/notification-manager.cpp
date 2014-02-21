//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/

//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/common/notification-manager.h>

// EXTERNAL INCLUDES
#include <boost/thread/mutex.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/common/message-container.h>

using namespace std;

namespace Dali
{

namespace Internal
{

typedef boost::mutex MessageQueueMutex;

struct NotificationManager::Impl
{
  Impl()
  : notificationCount(0)
  {
  }

  ~Impl()
  {
  }

  // Used to skip duplicate operations during Notify()
  unsigned int notificationCount;

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex queueMutex;
  MessageContainer updateQueue;
  MessageContainer eventQueue;
};

NotificationManager::NotificationManager()
{
  mImpl = new Impl();
}

NotificationManager::~NotificationManager()
{
  delete mImpl;
}

void NotificationManager::QueueMessage( MessageBase* message )
{
  DALI_ASSERT_DEBUG( NULL != message );

  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  mImpl->updateQueue.PushBack( message );
}

bool NotificationManager::MessagesToProcess()
{
  // queueMutex must be locked whilst accessing queue
  MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

  return ( false == mImpl->updateQueue.IsEmpty() );
}

void NotificationManager::ProcessMessages()
{
  // Done before messages are processed, for notification count comparisons
  ++mImpl->notificationCount;

  // queueMutex must be locked whilst accessing queue
  {
    MessageQueueMutex::scoped_lock lock( mImpl->queueMutex );

    // Swap the queue, original queue ends up empty, then release the lock
    mImpl->updateQueue.Swap( mImpl->eventQueue );
  }
  // end of scope, lock is released

  MessageContainer::Iterator iter = mImpl->eventQueue.Begin();
  MessageContainer::Iterator end = mImpl->eventQueue.End();
  for( ; iter != end; ++iter )
  {
    (*iter)->Process( 0u/*ignored*/ );
  }

  // release the processed messages from event side queue
  mImpl->eventQueue.Clear();
}

unsigned int NotificationManager::GetNotificationCount() const
{
  return mImpl->notificationCount;
}

void NotificationManager::NotifyPropertyNotification( Internal::PropertyNotification* propertyNotification )
{
  DALI_ASSERT_DEBUG( propertyNotification && "NULL property notification being notified" );
  propertyNotification->EmitSignalNotify();
}

} // namespace Internal

} // namespace Dali
