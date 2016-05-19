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
#include <dali/public-api/events/touch-data.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/events/touch-data-impl.h>

#include <cstdio>

namespace Dali
{

TouchData::TouchData()
: mObjectPointer( new Internal::TouchData )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
}

TouchData::TouchData( const TouchData& other )
: mObjectPointer( new Internal::TouchData )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
  *mObjectPointer = *other.mObjectPointer;
}

TouchData::~TouchData()
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
  delete mObjectPointer;
}

TouchData& TouchData::operator=( const TouchData& other )
{
  if( this != &other )
  {
    *mObjectPointer = *other.mObjectPointer;
  }

  return *this;
}

unsigned long TouchData::GetTime() const
{
  return mObjectPointer->GetTime();
}

unsigned int TouchData::GetPointCount() const
{
  return mObjectPointer->GetPointCount();
}

int32_t TouchData::GetDeviceId( unsigned int point ) const
{
  return mObjectPointer->GetDeviceId( point );
}

PointState::Type TouchData::GetState( unsigned int point ) const
{
  return mObjectPointer->GetState( point );
}

Actor TouchData::GetHitActor( unsigned int point ) const
{
  return mObjectPointer->GetHitActor( point );
}

const Vector2& TouchData::GetLocal( unsigned int point ) const
{
  return mObjectPointer->GetLocal( point );
}

const Vector2& TouchData::GetScreen( unsigned int point ) const
{
  return mObjectPointer->GetScreen( point );
}

TouchData::TouchData( const Internal::TouchData& impl )
: mObjectPointer( new Internal::TouchData )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
  *mObjectPointer = impl;
}

Internal::TouchData& TouchData::GetObject()
{
  return *mObjectPointer;
}

} // namespace Dali
