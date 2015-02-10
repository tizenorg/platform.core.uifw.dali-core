#ifndef __DALI_INTERNAL_BASE_OBJECT_LIFETIME_HPP__
#define __DALI_INTERNAL_BASE_OBJECT_LIFETIME_HPP__

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Internal
{

class BaseObjectLifetime : public Dali::RefObject
{
public:
  /**
  * Constructor
  */
  BaseObjectLifetime();

  /**
   * Notify object creation
   */
  void NotifyCreation( Dali::BaseObject* object );

  /**
   * Notify object destruction
   */
  void NotifyDestruction( Dali::BaseObject* object );

  /*
   * @copydoc BaseObject::ObjectCreatedSignal
   */
  BaseObject::ObjectCreatedSignalType& ObjectCreatedSignal();

  /*
   * @copydoc BaseObject::ObjectDestroyedSignal
   */
  BaseObject::ObjectDestroyedSignalType& ObjectDestroyedSignal();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BaseObjectLifetime();

private:
  Dali::BaseObject::ObjectCreatedSignalType mObjectCreatedSignal;
  Dali::BaseObject::ObjectDestroyedSignalType mObjectDestroyedSignal;
  std::size_t mObjectsCreated;
};

typedef Dali::IntrusivePtr<BaseObjectLifetime> BaseObjectLifetimePtr;

} // namespace Internal

} // namespace Dali


#endif // header
