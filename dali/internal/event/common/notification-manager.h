#ifndef __DALI_INTERNAL_NOTIFICATION_MANAGER_H__
#define __DALI_INTERNAL_NOTIFICATION_MANAGER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>

namespace Dali
{

namespace Internal
{

class PropertyNotification;

/**
 * Provides notifications to the event-thread regarding the changes in previous update(s).
 * For example after an animation finished, or after resources were loaded.
 */
class NotificationManager
{
public:

  /**
   * Create an NotificationManager.
   */
  NotificationManager();

  /**
   * Virtual destructor.
   */
  virtual ~NotificationManager();

  /**
   * Queue a scene message. This method is thread-safe.
   * @param[in] message A newly allocated message; NotificationManager takes ownership.
   */
  void QueueMessage( MessageBase* message );

  /**
   * Query whether the NotificationManager has messages to process.
   * @return True if there are messages to process.
   */
  bool MessagesToProcess();

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessMessages();

  /**
   * Retrieve the notification count; this is incremented when Notify() is called.
   */
  unsigned int GetNotificationCount() const;

public: // Notifications

  /**
   * Emit "Notify" signal for a property notification whose conditio has just been met (valid),
   * or no longer met (invalid).
   * This should be called indirectly via queued messages.
   */
  void NotifyPropertyNotification( Internal::PropertyNotification* propertyNotification );

private:

  struct Impl;
  Impl* mImpl;
};

/**
 * A functor for querying the notification count.
 * This is useful for skipping duplicate operations during NotificationManager::Notify()
 */
struct NotificationCountQuery
{
  NotificationCountQuery(const NotificationManager& manager)
  : mNotificationManager(manager)
  {
  }

  unsigned int operator()() const
  {
    return mNotificationManager.GetNotificationCount();
  }

private:

  const NotificationManager& mNotificationManager;
};

/**
 * Notification message for property notification conditions have been met (valid)
 * @param[in] manager The notification manager which will notify applications.
 * @param[in] propertyNotification The property notification.
 */
inline MessageBase* PropertyNotificationMessage( NotificationManager& manager, Internal::PropertyNotification& propertyNotification )
{
  return new MessageValue1< NotificationManager, Internal::PropertyNotification* >(
      &manager, &NotificationManager::NotifyPropertyNotification, &propertyNotification );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_NOTIFICATION_MANAGER_H__

