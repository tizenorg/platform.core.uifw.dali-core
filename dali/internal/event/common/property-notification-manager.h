#ifndef __DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H__
#define __DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H__

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

// EXTERNAL INCLUDES
#include <set>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/property-notifier.h>

namespace Dali
{

namespace Internal
{

class PropertyNotification;

/**
 * PropertyNotificationManager issues notifications to applications
 * a condition of a property being met.
 * It also monitors the lifetime of PropertyNotification objects and will
 * only emit signals for PropertyNotification objects which are still valid.
 */
class PropertyNotificationManager : public PropertyNotifier
{
public:

  /**
   * Create an PropertyNotificationManager.
   * @return A newly allocated object.
   */
  static PropertyNotificationManager* New();

  /**
   * Virtual destructor.
   */
  virtual ~PropertyNotificationManager();

  /**
   * Called when a PropertyNotification is constructed.
   */
  void PropertyNotificationCreated( PropertyNotification& propertyNotification );

  /**
   * Called when a PropertyNotification is destroyed.
   */
  void PropertyNotificationDestroyed( PropertyNotification& propertyNotification );

  /**
   * From PropertyNotifier; emit "Notify" signal for a changed property.
   * This method should be called in the event-thread; the update-thread must use PropertyChangedMessage.
   * @param[in] propertyNotification A pointer to the SceneGraph::PropertyNotification that has been mnodified.
   */
  virtual void NotifyProperty( SceneGraph::PropertyNotification* propertyNotification );

private:

  /**
   * Default constructor.
   */
  PropertyNotificationManager();

  // Undefined
  PropertyNotificationManager(const PropertyNotificationManager&);

  // Undefined
  PropertyNotificationManager& operator=(const PropertyNotificationManager& rhs);

private:

  std::set< PropertyNotification* > mPropertyNotifications; ///< All existing PropertyNotifications (not referenced)

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_NOTIFICATION_MANAGER_H__

