#ifndef __DALI_BASE_OBJECT_H__
#define __DALI_BASE_OBJECT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/functor-delegate.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

/**
 * @brief A base class for objects.
 */
class DALI_IMPORT_API BaseObject : public Dali::RefObject
{
public:

  /**
   * @brief Connects a void() functor to a specified signal.
   *
   * @pre The signal must be available in this object.
   * @param [in] connectionTracker A connection tracker which can be used to disconnect.
   * @param [in] signalName Name of the signal to connect to.
   * @param [in] functor The functor to copy.
   * @return True if the signal was available.
   */
  template <class T>
  bool ConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, const T& functor )
  {
    return DoConnectSignal( connectionTracker, signalName, FunctorDelegate::New( functor ) );
  }

  /**
   * @copydoc Dali::BaseHandle::DoAction
   */
  bool DoAction(const std::string& actionName, const std::vector<Property::Value>& attributes);

  /**
   * @copydoc Dali::BaseHandle::GetTypeName
   */
  const std::string& GetTypeName() const;

  /**
   * @copydoc Dali::BaseHandle::GetTypeInfo
   */
  bool GetTypeInfo(Dali::TypeInfo& info) const;

public: // Not intended for application developers

  /**
   * @copydoc Dali::BaseHandle::DoConnectSignal
   */
  bool DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functorDelegate );

protected:

  /**
   * @brief Default constructor.
   */
  BaseObject();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~BaseObject();

  /**
   * @brief Notify the object as created.
   *
   * Some derived objects, eg Actors, Images will notify their creation with this function
   */
  void NotifyCreation();

  /**
   * @brief Notify the object as destroyed.
   */
  void NotifyDestruction();

public:
  /**
   * @brief Object created signal
   */
  typedef Signal<  void (BaseHandle) > ObjectCreatedSignalType;

  /**
   * @brief Object destroyed signal
   */
  typedef Signal<  void (const Dali::RefObject*) > ObjectDestroyedSignalType;

  /**
   * @brief This signal is emitted when (some) derived objects are created.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(BaseHandle object);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  static ObjectCreatedSignalType& ObjectCreatedSignal();

  /**
   * @brief This signal is emitted when (some) derived objects are destroyed.
   *
   * WARNING: Since this signal is emitted when the object is
   * in the process of being destroyed, the RefObject pointer
   * passed in the signal should not be modified in anyways.
   * And should NOT be used to create an handle. which will
   * affect the life time of this destroyed object and leads to
   * undefined behaviour.
   *
   * The only intended use is for Toolkit controls which want to
   * keep track of objects being created and destroyed for internal
   * bookkeeping.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const Dali::RefObject* objectPointer);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  static ObjectDestroyedSignalType& ObjectDestroyedSignal();

private:

  // Not implemented
  DALI_INTERNAL BaseObject(const BaseObject& rhs);

  // Not implemented
  DALI_INTERNAL BaseObject& operator=(const BaseObject& rhs);
};

// Helpers for public-api forwarding methods

/**
 * @brief Get the implementation of a handle.
 *
 * @param[in] handle The handle
 * @return A reference to the object the handle points at.
 */
inline BaseObject& GetImplementation(Dali::BaseHandle& handle)
{
  DALI_ASSERT_ALWAYS( handle && "BaseObject handle is empty" );

  return handle.GetBaseObject();
}

/**
 * @brief Get the implementation of a handle.
 *
 * @param[in] handle The handle
 * @return A reference to the object the handle points at.
 */
inline const BaseObject& GetImplementation(const Dali::BaseHandle& handle)
{
  DALI_ASSERT_ALWAYS( handle && "BaseObject handle is empty" );

  return handle.GetBaseObject();
}

} // namespace Dali

# endif // __DALI_BASE_OBJECT_H__
