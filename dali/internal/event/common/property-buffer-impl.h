#ifndef __DALI_INTERNAL_PROPERTY_BUFFER_H__
#define __DALI_INTERNAL_PROPERTY_BUFFER_H__

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
#include <dali/public-api/object/property-buffer.h>

#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

namespace Internal
{

/**
 * A class that implements a buffer of properties
 * The concrete derived class is responsible for implementing the property system methods.
 * Classes may derive from Dali::BaseObject, until any properties are required.
 */
class PropertyBuffer : public ProxyObject
{
public:


protected:

  /**
   * Default constructor.
   */
  PropertyBuffer()
  {
  }

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PropertyBuffer()
  {
  }

private:

  // Not implemented
  PropertyBuffer(const PropertyBuffer& rhs);

  // Not implemented
  PropertyBuffer& operator=(const PropertyBuffer& rhs);
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Object& GetImplementation(Dali::PropertyBuffer& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::Object&>(handle);
}

inline const Internal::Object& GetImplementation(const Dali::PropertyBuffer& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::Object&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_BUFFER_H__

