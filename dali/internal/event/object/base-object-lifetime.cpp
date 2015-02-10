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
#include <dali/internal/event/object/base-object-lifetime.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/integration-api/debug.h>

namespace Dali {
namespace Internal {

BaseObjectLifetime::BaseObjectLifetime()
  : mObjectsCreated(0)
{
}

BaseObjectLifetime::~BaseObjectLifetime()
{

}

void BaseObjectLifetime::CheckBaseObjectLifetimeCount()
{
  if(0 != mObjectsCreated)
  {
    DALI_LOG_WARNING("Dali objects alive at shutdown. Alive count: '%d'\n", mObjectsCreated);
  }

  DALI_ASSERT_ALWAYS(0 == mObjectsCreated &&
                     "Dali Objects have not been cleaned up before Dali core destruction!");

}

void BaseObjectLifetime::NotifyCreation( Dali::BaseObject* object )
{
  mObjectsCreated++;

  if ( !mObjectCreatedSignal.Empty() )
  {
    Dali::BaseHandle handle( object );
    mObjectCreatedSignal.Emit( handle );
  }
}

void BaseObjectLifetime::NotifyDestruction( Dali::BaseObject* object )
{
  if(0 >= mObjectsCreated)
  {
    DALI_LOG_WARNING("A dali object was deleted twice or gave no creation notifictaion! '%d'\n", mObjectsCreated);
  }
  else
  {
    mObjectsCreated--;
    mObjectDestroyedSignal.Emit( object );
  }
}


BaseObject::ObjectCreatedSignalType& BaseObjectLifetime::ObjectCreatedSignal()
{
  return mObjectCreatedSignal;
}

BaseObject::ObjectDestroyedSignalType& BaseObjectLifetime::ObjectDestroyedSignal()
{
  return mObjectDestroyedSignal;
}


}; // namespace Internal
}; // namespace Dali
