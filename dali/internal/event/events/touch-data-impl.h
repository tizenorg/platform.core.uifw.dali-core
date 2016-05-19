#ifndef __DALI_INTERNAL_TOUCH_DATA_H__
#define __DALI_INTERNAL_TOUCH_DATA_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/point.h>
#include <dali/public-api/events/point-state.h>

namespace Dali
{

class Actor;
struct Vector2;

namespace Internal
{

/**
 * @copydoc Dali::TouchData
 */
class TouchData
{
public:

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  TouchData();

  /**
   * @brief Constructor
   * @param[in]  time  The time the event occurred
   */
  TouchData( unsigned long time );

  /**
   * @brief Copy constructor
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchData( const TouchData& other );

  /**
   * @brief Destructor
   */
  ~TouchData();

  // Operators

  /**
   * @brief Assignment Operator
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchData& operator=( const TouchData& other );

  // Getters

  /**
   * @copydoc Dali::TouchData::GetTime()
   */
  unsigned long GetTime() const;

  /**
   * @copydoc Dali::TouchData::GetPointCount()
   */
  unsigned int GetPointCount() const;

  /**
   * @copydoc Dali::TouchData::GetDeviceId()
   */
  int32_t GetDeviceId( unsigned int point ) const;

  /**
   * @copydoc Dali::TouchData::GetGetState()
   */
  PointState::Type GetState( unsigned int point ) const;

  /**
   * @copydoc Dali::TouchData::GetHitActor()
   */
  Dali::Actor GetHitActor( unsigned int point ) const;

  /**
   * @copydoc Dali::TouchData::GetLocal()
   */
  const Vector2& GetLocal( unsigned int point ) const;

  /**
   * @copydoc Dali::TouchData::GetScreen()
   */
  const Vector2& GetScreen( unsigned int point ) const;

  /**
   * @brief Returns a reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @SINCE_1_1.36
   * @param[in]  point  The index of the required Point.
   * @return A reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  Point& GetPoint( unsigned int point );

  // Setters

  /**
   * @brief Sets the time (in ms) that the touch event occurred.
   * @param[in]  time  The time (in ms) that the touch event occurred.
   */
  void SetTime( unsigned long time );

  /**
   * @brief Adds a point to this touch event handler.
   * @param[in]  point  The point to add to the touch event handler.
   */
  void AddPoint( const Point& point );

//private:

  typedef std::vector< Point > PointContainer;
  PointContainer   mPoints;   ///< Container of the points for this touch event.
  unsigned long    mTime;     ///< The time (in ms) that the touch event occurred.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TOUCH_DATA_H__
