/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/point.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

struct Point::Impl
{
  Actor     hitActor;     ///< The actor that was underneath the point.
  Vector2   screen;       ///< The co-ordinates relative to the top-left of the screen.
  Vector2   local;        ///< The co-ordinates relative to the top-left of the hit-actor.
  State     state;        ///< State of the point.
  int32_t   deviceId;     ///< Each point has a unique device ID which specifies the device used for that point.
};

Point::Point()
: mImpl( new Impl )
{
  mImpl->state = STARTED;
  mImpl->deviceId = 0;
}

Point::Point( int32_t deviceId, State state, const Vector2& screen )
: mImpl( new Impl )
{
  mImpl->screen = mImpl->local = screen;
  mImpl->state = state;
  mImpl->deviceId = deviceId;
}

Point::Point( int32_t deviceId, State state, const Vector2& screen, const Vector2& local )
: mImpl( new Impl )
{
  mImpl->screen = screen;
  mImpl->local = local;
  mImpl->state = state;
  mImpl->deviceId = deviceId;
}

Point::Point( const Point& other )
: mImpl( new Impl )
{
  *mImpl = *other.mImpl;
}

Point::~Point()
{
  delete mImpl;
}

Point& Point::operator=( const Point& other )
{
  if( this != &other )
  {
    mImpl = other.mImpl;
  }

  return *this;
}

int32_t Point::GetDeviceId() const
{
  return mImpl->deviceId;
}

Point::State Point::GetState() const
{
  return mImpl->state;
}


Actor Point::GetHitActor() const
{
  return mImpl->hitActor;
}

const Vector2& Point::GetLocal() const
{
  return mImpl->local;
}

const Vector2& Point::GetScreen() const
{
  return mImpl->screen;
}

void Point::SetDeviceId( int32_t deviceId )
{
  mImpl->deviceId = deviceId;
}

void Point::SetState( State state )
{
  mImpl->state = state;
}

void Point::SetHitActor( Actor actor )
{
  mImpl->hitActor = actor;
}

void Point::SetLocal( const Vector2& local )
{
  mImpl->local = local;
}

void Point::SetScreen( const Vector2& screen )
{
  mImpl->screen = screen;
}

} // namespace Dali
