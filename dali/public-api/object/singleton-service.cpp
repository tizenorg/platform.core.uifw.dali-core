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
#include <dali/public-api/object/singleton-service.h>

// INTERNAL INCLUDES
#include <dali/internal/event/object/singleton-service-impl.h>

namespace Dali
{

const char* const SingletonService::SIGNAL_OBJECT_CREATED = "object-created";
const char* const SingletonService::SIGNAL_OBJECT_DESTROYED = "object-destroyed";

SingletonService::SingletonService()
{
}

SingletonService SingletonService::Get()
{
  return Internal::SingletonService::Get();
}

void SingletonService::Release()
{
  GetImplementation( *this ).Release();
}

SingletonService::~SingletonService()
{
}

void SingletonService::Register( const std::type_info& info, BaseHandle singleton )
{
  GetImplementation( *this ).Register( info, singleton );
}

void SingletonService::UnregisterAll()
{
  GetImplementation( *this ).UnregisterAll();
}

BaseHandle SingletonService::GetSingleton( const std::type_info& info ) const
{
  return GetImplementation( *this ).GetSingleton( info );
}

SingletonService::SingletonService( Internal::SingletonService* singletonService )
: BaseHandle( singletonService )
{
}

void SingletonService::NotifyObjectCreate( Dali::BaseObject* object )
{
  GetImplementation(*this).NotifyObjectCreate( object );
}

void SingletonService::NotifyObjectDestroy( Dali::BaseObject* object )
{
  GetImplementation(*this).NotifyObjectDestroy( object );
}

SingletonService::ObjectCreatedSignalV2& SingletonService::ObjectCreatedSignal()
{
  return GetImplementation(*this).ObjectCreatedSignal();
}

SingletonService::ObjectDestroyedSignalV2& SingletonService::ObjectDestroyedSignal()
{
  return GetImplementation(*this).ObjectDestroyedSignal();
}

} // namespace Dali
