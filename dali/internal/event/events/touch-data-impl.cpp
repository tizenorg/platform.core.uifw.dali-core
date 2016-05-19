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
#include <dali/internal/event/events/touch-data-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

#include <cstdio>

namespace Dali
{

namespace Internal
{

TouchData::TouchData()
: mPoints(),
  mTime( 0 )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
}

TouchData::TouchData( unsigned long time )
: mPoints(),
  mTime( time )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
}

TouchData::TouchData( const TouchData& other )
: mPoints( other.mPoints ),
  mTime( other.mTime )
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
}

TouchData::~TouchData()
{
  printf( "%s: %p\n", __PRETTY_FUNCTION__, this );
}

TouchData& TouchData::operator=( const TouchData& other )
{
  if( this != &other )
  {
    mPoints = other.mPoints;
    mTime = other.mTime;
  }

  return *this;
}

unsigned long TouchData::GetTime() const
{
  return mTime;
}

unsigned int TouchData::GetPointCount() const
{
  return mPoints.size();
}

int32_t TouchData::GetDeviceId( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].GetDeviceId();
}

PointState::Type TouchData::GetState( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return static_cast< PointState::Type >( mPoints[ point ].GetState() );
}

Dali::Actor TouchData::GetHitActor( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].GetHitActor();
}

const Vector2& TouchData::GetLocal( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].GetLocal();
}

const Vector2& TouchData::GetScreen( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].GetScreen();
}

Point& TouchData::GetPoint( unsigned int point )
{
  DALI_ASSERT_DEBUG( point < mPoints.size() && "No point at index" );
  return mPoints[ point ];
}

void TouchData::SetTime( unsigned long time )
{
  mTime = time;
}

void TouchData::AddPoint( const Point& point )
{
  mPoints.push_back( point );
}

} // namsespace Internal

} // namespace Dali
