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
#include <dali/public-api/events/touch-event-handle.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/point.h>

namespace Dali
{

namespace
{
typedef std::vector< Point > PointContainer;
} // unnamed namespace

namespace Internal
{
struct TouchData
{
  PointContainer   points;   ///< Container of the points for this touch event.
  unsigned long    time;     ///< The time (in ms) that the touch event occurred.
};
}

TouchData::TouchData()
: mObjectPointer( new Internal::TouchData )
{
  mObjectPointer->time = 0;
}

TouchData::TouchData( unsigned long time )
: mObjectPointer( new Internal::TouchData )
{
  mObjectPointer->time = time;
}

TouchData::TouchData( const TouchData& other )
: mObjectPointer( new Internal::TouchData )
{
  mObjectPointer = other.mObjectPointer;
}

TouchData::~TouchData()
{
  delete mObjectPointer;
}

TouchData& TouchData::operator=( const TouchData& other )
{
  if( this != &other )
  {
    mObjectPointer = other.mObjectPointer;
  }

  return *this;
}

unsigned long TouchData::GetTime() const
{
  return mObjectPointer->time;
}

unsigned int TouchData::GetPointCount() const
{
  return mObjectPointer->points.size();
}

const Point& TouchData::GetPoint( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mObjectPointer->points.size() && "No point at index" );
  return mObjectPointer->points[ point ];
}

Point& TouchData::GetPoint( unsigned int point )
{
  DALI_ASSERT_ALWAYS( point < mObjectPointer->points.size() && "No point at index" );
  return mObjectPointer->points[ point ];
}

void TouchData::SetTime( unsigned long time )
{
  mObjectPointer->time = time;
}

void TouchData::AddPoint( const Point& point )
{
  mObjectPointer->points.push_back( point );
}

} // namespace Dali
