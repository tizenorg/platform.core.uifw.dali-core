#ifndef __DALI_TOUCH_EVENT_HANDLE_H__
#define __DALI_TOUCH_EVENT_HANDLE_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

class Point;

/**
 * @brief Touch events are a collection of points at a specific moment in time.
 *
 * When a multi-touch event occurs, each point represents the points that are currently being
 * touched or the points where a touch has stopped.
 * @SINCE_1_1.36
 */
struct DALI_IMPORT_API TouchEventHandle
{
  // Construction & Destruction

  /**
   * @brief Default constructor
   *
   * @SINCE_1_1.36
   */
  TouchEventHandle();

  /**
   * @brief Constructor
   *
   * @SINCE_1_1.36
   * @param[in]  time  The time the event occurred
   */
  TouchEventHandle( unsigned long time );

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_1.36
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchEventHandle( const TouchEventHandle& other );

  /**
   * @brief Destructor
   *
   * @SINCE_1_1.36
   */
  ~TouchEventHandle();

  // Operators

  /**
   * @brief Assignment Operator
   *
   * @SINCE_1_1.36
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchEventHandle& operator=( const TouchEventHandle& other );

  // Getters

  /**
   * @brief Returns the time (in ms) that the touch event occurred.
   *
   * @SINCE_1_1.36
   * @return The time (in ms) that the touch event occurred.
   */
  unsigned long GetTime() const;

  /**
   * @brief Returns the total number of points in this TouchEventHandle.
   *
   * @SINCE_1_1.36
   * @return Total number of Points.
   */
  unsigned int GetPointCount() const;

  /**
   * @brief Returns a const reference to a point at the index requested.
   *
   * The first point in the set is always the primary point (i.e. the first point touched in a multi-touch event).
   *
   * @SINCE_1_1.36
   * @param[in]  point  The index of the required Point.
   * @return A const reference to the Point at the position requested
   * @note point should be less than the value returned by GetPointCount(). Will assert if out of range.
   */
  const Point& GetPoint( unsigned int point ) const;

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
   *
   * @SINCE_1_1.36
   * @param[in]  time  The time (in ms) that the touch event occurred.
   */
  void SetTime( unsigned long time );

  /**
   * @brief Adds a point to this touch event handler.
   *
   * @SINCE_1_1.36
   * @param[in]  point  The point to add to the touch event handler.
   */
  void AddPoint( const Point& point );

private:

  struct Impl;
  Impl* mImpl;
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_TOUCH_EVENT_HANDLE_H__
