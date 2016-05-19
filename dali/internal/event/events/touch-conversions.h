#ifndef __DALI_INTERNAL_POINT_CONVERSION_H__
#define __DALI_INTERNAL_POINT_CONVERSION_H__

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

namespace Dali
{

struct TouchEvent;
class TouchData;
struct TouchPoint;
class Point;

namespace Internal
{

/**
 * @brief Converts TouchPoint to a Point.
 *
 * @param[in]  touchPoint  The TouchPoint to copy from.
 * @param[in]  point       The Point to populate.
 */
void ConvertTouchPointToPoint( const TouchPoint& touchPoint, Point& point );

/**
 * @brief Converts TouchEvent to a TouchEventHandle
 *
 * @param[in]  touchEvent        The TouchEvent to copy from.
 * @param[in]  touchEventHandle  The TouchEventHandle to populate.
 * @note Assumes touchEventHandle does not have any points.
 */
void ConvertTouchEventToTouchEventHandle( const TouchEvent& touchEvent, Dali::TouchData& touchEventHandle );

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_OBSERVER_H__

