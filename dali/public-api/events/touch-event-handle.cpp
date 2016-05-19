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

struct TouchEventHandle::Impl
{
  PointContainer   points;   ///< Container of the points for this touch event.
  unsigned long    time;     ///< The time (in ms) that the touch event occurred.
};

TouchEventHandle::TouchEventHandle()
: mImpl( new Impl )
{
  mImpl->time = 0;
}

TouchEventHandle::TouchEventHandle( unsigned long time )
: mImpl( new Impl )
{
  mImpl->time = time;
}

TouchEventHandle::TouchEventHandle( const TouchEventHandle& other )
: mImpl( new Impl )
{
  mImpl = other.mImpl;
}

TouchEventHandle::~TouchEventHandle()
{
  delete mImpl;
}

TouchEventHandle& TouchEventHandle::operator=( const TouchEventHandle& other )
{
  if( this != &other )
  {
    mImpl = other.mImpl;
  }

  return *this;
}

unsigned long TouchEventHandle::GetTime() const
{
  return mImpl->time;
}

unsigned int TouchEventHandle::GetPointCount() const
{
  return mImpl->points.size();
}

const Point& TouchEventHandle::GetPoint( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS( point < mImpl->points.size() && "No point at index" );
  return mImpl->points[ point ];
}

Point& TouchEventHandle::GetPoint( unsigned int point )
{
  DALI_ASSERT_ALWAYS( point < mImpl->points.size() && "No point at index" );
  return mImpl->points[ point ];
}

void TouchEventHandle::SetTime( unsigned long time )
{
  mImpl->time = time;
}

void TouchEventHandle::AddPoint( const Point& point )
{
  mImpl->points.push_back( point );
}

} // namespace Dali
