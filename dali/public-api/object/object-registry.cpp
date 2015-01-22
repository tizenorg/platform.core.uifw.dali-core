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

// CLASS HEADER
#include <dali/public-api/object/object-registry.h>

#include <dali/public-api/object/singleton-service.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-registry-impl.h>

namespace Dali
{

const char* const ObjectRegistry::SIGNAL_OBJECT_CREATED = "object-created";
const char* const ObjectRegistry::SIGNAL_OBJECT_DESTROYED = "object-destroyed";

ObjectRegistry::ObjectRegistry()
{
}

ObjectRegistry::~ObjectRegistry()
{
}

ObjectRegistry::ObjectRegistry(const ObjectRegistry& copy)
: BaseHandle(copy)
{
}

ObjectRegistry ObjectRegistry::Get()
{
  ObjectRegistry registry;

  SingletonService singletonService( SingletonService::Get() );
  if( singletonService )
  {
    Dali::BaseHandle handle = singletonService.GetSingleton( typeid(ObjectRegistry) );
    if( handle )
    {
      // If so, downcast the handle of singleton to ObjectRegistry
      DALI_ASSERT_DEBUG( dynamic_cast<Internal::ObjectRegistry*>(handle.GetObjectPtr()) );
      registry = ObjectRegistry(reinterpret_cast<Internal::ObjectRegistry*>(handle.GetObjectPtr()));
    }

    if( !registry )
    {
      // If not, create the ObjectRegistry and register it as a singleton
      registry = ObjectRegistry(new Internal::ObjectRegistry());
      singletonService.Register( typeid(registry), registry );
    }
  }

  return registry;
}


ObjectRegistry& ObjectRegistry::operator=(const ObjectRegistry& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

ObjectRegistry::ObjectCreatedSignalV2& ObjectRegistry::ObjectCreatedSignal()
{
  return GetImplementation(*this).ObjectCreatedSignal();
}

ObjectRegistry::ObjectDestroyedSignalV2& ObjectRegistry::ObjectDestroyedSignal()
{
  return GetImplementation(*this).ObjectDestroyedSignal();
}

ObjectRegistry::ObjectRegistry(Internal::ObjectRegistry* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
