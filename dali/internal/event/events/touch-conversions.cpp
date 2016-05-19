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
#include <dali/internal/event/events/touch-conversions.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/touch-event-handle.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/public-api/events/point.h>

namespace Dali
{

namespace Internal
{

void ConvertTouchPointToPoint( const TouchPoint& touchPoint, Point& point )
{
  point.SetHitActor( touchPoint.hitActor );
  point.SetScreen( touchPoint.screen );
  point.SetLocal( touchPoint.local );
  point.SetState( static_cast< Point::State >( touchPoint.state ) );
  point.SetDeviceId( touchPoint.deviceId );
}

void ConvertTouchEventToTouchEventHandle( const TouchEvent& touchEvent, Dali::TouchData& touchEventHandle )
{
  touchEventHandle.SetTime( touchEvent.time );

  for ( TouchPointContainerConstIterator iter = touchEvent.points.begin(), endIter = touchEvent.points.end(); iter != endIter; ++iter )
  {
    Point point;
    ConvertTouchPointToPoint( *iter, point );
    touchEventHandle.AddPoint( point );
  }
}

} // namespace Internal

} // namespace Dali

