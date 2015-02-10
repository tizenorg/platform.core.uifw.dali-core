#ifndef __DALI_INTERNAL_SINGLETON_SERVICE_H__
#define __DALI_INTERNAL_SINGLETON_SERVICE_H__

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

// EXTERNAL INCLUDES
#include <set>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/singleton-service.h>

namespace Dali
{

namespace Internal
{

class SingletonService : public Dali::BaseObject
{
public:

  /**
   * Private Constructor
   * @see SingletonService::New()
   */
  SingletonService();

  /**
   * Virtual Destructor
   */
  virtual ~SingletonService();

  /**
   * @copydoc Dali::SingletonService::Get()
   */
  static Dali::SingletonService Get();

  /**
   * Reset/Destory the singleton service
   */
  void Release();

  /**
   * @copydoc Dali::SingletonService::Register()
   */
  void Register( const std::type_info& info, BaseHandle singleton );

  /**
   * @copydoc Dali::SingletonService::UnregisterAll()
   */
  void UnregisterAll();

  /**
   * @copydoc Dali::SingletonService::GetSingleton()
   */
  BaseHandle GetSingleton( const std::type_info& info ) const;

  /**
   * @copydoc Dali::SingletonService::ObjectCreatedSignal()
   */
  Dali::SingletonService::ObjectCreatedSignalType& ObjectCreatedSignal();

  /**
   * @copydoc Dali::SingletonService::ObjectDestroyedSignal()
   */
  Dali::SingletonService::ObjectDestroyedSignalType& ObjectDestroyedSignal();

  /**
   * @copydoc Dali::SingletonService::NotifyObjectCreate()
   */
  void NotifyObjectCreate( Dali::BaseObject* object );

  /**
   * @copydoc Dali::SingletonService::NotifyObjectDestroy()
   */
  void NotifyObjectDestroy( Dali::BaseObject* object );

private:

  // Undefined
  SingletonService( const SingletonService& );
  SingletonService& operator=( SingletonService& );

private:
  Dali::SingletonService::ObjectCreatedSignalType mObjectCreatedSignal;
  Dali::SingletonService::ObjectDestroyedSignalType mObjectDestroyedSignal;

private:
  std::size_t mObjectsCreated;

  typedef std::pair<std::string, BaseHandle> SingletonPair;
  typedef std::map<std::string, BaseHandle>  SingletonContainer;
  typedef SingletonContainer::const_iterator SingletonConstIter;

  SingletonContainer mSingletonContainer; ///< The container to look up singleton by its type name

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::SingletonService& GetImplementation(Dali::SingletonService& service)
{
  DALI_ASSERT_ALWAYS( service && "SingletonService handle is empty" );

  BaseObject& handle = service.GetBaseObject();

  return static_cast<Internal::SingletonService&>(handle);
}

inline const Internal::SingletonService& GetImplementation(const Dali::SingletonService& service)
{
  DALI_ASSERT_ALWAYS( service && "SingletonService handle is empty" );

  const BaseObject& handle = service.GetBaseObject();

  return static_cast<const Internal::SingletonService&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_SINGLETON_SERVICE_H__
