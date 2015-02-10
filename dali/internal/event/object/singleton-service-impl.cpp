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
#include <dali/internal/event/object/singleton-service-impl.h>

// EXTERNAL INCLUDES
#include <boost/thread/tss.hpp>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

#if 0 // defined(DEBUG_ENABLED)
//#include <slp-logging.h>
Debug::Filter* gSingletonServiceLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_SINGLETON_SERVICE" );

// Need to define own macro as the log function is not installed when this object is created so no logging is shown with DALI_LOG_INFO at construction and destruction
#define DALI_LOG_SINGLETON_SERVICE_DIRECT(level, message)                        \
    if(gSingletonServiceLogFilter && gSingletonServiceLogFilter->IsEnabledFor(level)) { std::string string(message); Dali::SlpPlatform::LogMessage( Debug::DebugInfo, string );  }

#define DALI_LOG_SINGLETON_SERVICE(level, format, args...) DALI_LOG_INFO(gSingletonServiceLogFilter, level, format, ## args )

#else

#define DALI_LOG_SINGLETON_SERVICE_DIRECT(level, message)
#define DALI_LOG_SINGLETON_SERVICE(level, format, args...)

#endif

namespace Dali
{

namespace Internal
{

namespace
{
typedef IntrusivePtr<SingletonService> SingletonServicePtr;
boost::thread_specific_ptr< SingletonServicePtr > gSingletonService;
} // unnamed namespace

Dali::SingletonService SingletonService::Get()
{
  SingletonServicePtr* ptr = gSingletonService.get();

  if ( NULL == gSingletonService.get() )
  {
    ptr = new SingletonServicePtr(new SingletonService());
    gSingletonService.reset(ptr);
  }

  return Dali::SingletonService(ptr->Get());
}

void SingletonService::Release()
{
  mSingletonContainer.clear();

}

void SingletonService::Register( const std::type_info& info, BaseHandle singleton )
{
  if( singleton )
  {
    DALI_LOG_SINGLETON_SERVICE( Debug::General, "Singleton Added: %s\n", info.name() );
    mSingletonContainer.insert( SingletonPair( info.name(), singleton ) );
  }
}

void SingletonService::UnregisterAll( )
{
  mSingletonContainer.clear();
}

BaseHandle SingletonService::GetSingleton( const std::type_info& info ) const
{
  BaseHandle object;

  SingletonConstIter iter = mSingletonContainer.find(info.name());
  if( iter != mSingletonContainer.end() )
  {
    object = ( *iter ).second;
  }

  return object;
}

SingletonService::SingletonService()
  : mSingletonContainer()
{
  DALI_LOG_SINGLETON_SERVICE_DIRECT( Debug::Concise, "SingletonService Created\n" );
}

SingletonService::~SingletonService()
{
  DALI_LOG_SINGLETON_SERVICE_DIRECT( Debug::Concise, "SingletonService Destroyed\n" );

  DALI_ASSERT_ALWAYS(0 == mSingletonContainer.size() &&
                     "Singletons should be released before singleton service destruction");
}

} // namespace Internal

} // namespace Dali
