/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

struct WeakHandle::Impl : public Internal::Object::Observer
{
  // Construction
  Impl()
  : mObject( NULL )
  {
  }

  // Construction
  Impl( Handle& handle )
  : mObject( NULL )
  {
    if(handle)
    {
      mObject = dynamic_cast<Internal::Object*>( handle.GetObjectPtr() );
      if(mObject)
      {
        mObject->AddObserver( *this );
      }
    }
  }

  // Destruction
  ~Impl()
  {
    if( mObject )
    {
      mObject->RemoveObserver( *this );
    }
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectAdded( Internal::Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectRemoved( Internal::Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void ObjectDestroyed( Internal::Object& object )
  {
    mObject = NULL;
  }

  // Data
  Internal::Object* mObject;
};

WeakHandle::WeakHandle()
: mImpl( new Impl() )
{
}

WeakHandle::WeakHandle( Handle& handle )
: mImpl( new Impl( handle ) )
{
}

WeakHandle::~WeakHandle()
{
  delete mImpl;
  mImpl = NULL;
}

WeakHandle::WeakHandle(const WeakHandle& handle)
: mImpl( NULL )
{
  Handle object = handle.GetHandle();
  mImpl = new Impl(object);
}

WeakHandle& WeakHandle::operator=( const WeakHandle& rhs )
{
  if( this != &rhs )
  {
    delete mImpl;

    Handle handle = rhs.GetHandle();
    mImpl = new Impl(handle);
  }

  return *this;
}

bool WeakHandle::operator==( const WeakHandle& rhs ) const
{
  return this->mImpl->mObject == rhs.mImpl->mObject;
}

bool WeakHandle::operator!=( const WeakHandle& rhs ) const
{
  return !( *this == rhs );
}

Handle WeakHandle::GetHandle() const
{
  return Handle( mImpl->mObject );
}

} // Dali
