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

namespace Dali
{

namespace Internal
{

TouchData::TouchData()
: mPoints(),
  mTime( 0 )
{
}

TouchData::TouchData( unsigned long time )
: mPoints(),
  mTime( time )
{
}

TouchData::~TouchData()
{
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
  return mPoints[ point ].deviceId;
}

PointState::Type TouchData::GetState( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return static_cast< PointState::Type >( mPoints[ point ].state );
}

Dali::Actor TouchData::GetHitActor( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].hitActor;
}

const Vector2& TouchData::GetLocal( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].local;
}

const Vector2& TouchData::GetScreen( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mPoints.size() && "No point at index" );
  return mPoints[ point ].screen;
}

TouchPoint& TouchData::GetPoint( unsigned int point )
{
  DALI_ASSERT_DEBUG( point < mPoints.size() && "No point at index" );
  return mPoints[ point ];
}

void TouchData::SetTime( unsigned long time )
{
  mTime = time;
}

void TouchData::AddPoint( const TouchPoint& point )
{
  mPoints.push_back( point );
}

void TouchData::SetPoints( const TouchPointContainer& points )
{
  mPoints = points;
}

} // namsespace Internal

} // namespace Dali
