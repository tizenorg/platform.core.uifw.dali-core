#ifndef __DALI_TOUCH_DATA_H__
#define __DALI_TOUCH_DATA_H__

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
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/object/base-handle.h>

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
struct TouchData;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

class Actor;
struct Vector2;

/**
 * @brief Touch events are a collection of points at a specific moment in time.
 *
 * When a multi-touch event occurs, each point represents the points that are currently being
 * touched or the points where a touch has stopped.
 *
 * The first point is the primary point that's used for hit-testing.
 * @SINCE_1_1.37
 * @note Should not use this in a TouchData container as it is just a handle and the internal object can change.
 */
class DALI_IMPORT_API TouchData : public BaseHandle
{
public:

  // Construction & Destruction

  /**
   * @brief An uninitialized TouchData instance.
   *
   * Calling member functions with an uninitialized TouchData handle is not allowed.
   * @SINCE_1_1.37
   */
  TouchData();

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_1.37
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchData( const TouchData& other );

  /**
   * @brief Destructor
   *
   * @SINCE_1_1.37
   */
  ~TouchData();

  // Operators

  /**
   * @brief Assignment Operator
   *
   * @SINCE_1_1.37
   * @param[in]  other  The TouchEventHandle to copy from.
   */
  TouchData& operator=( const TouchData& other );

  // Getters

  /**
   * @brief Returns the time (in ms) that the touch event occurred.
   *
   * @SINCE_1_1.37
   * @return The time (in ms) that the touch event occurred.
   */
  unsigned long GetTime() const;

  /**
   * @brief Returns the total number of points in this TouchEventHandle.
   *
   * @SINCE_1_1.37
   * @return Total number of Points.
   */
  unsigned int GetPointCount() const;

  /**
   * @brief Returns the ID of the device used for the Point specified.
   *
   * Each point has a unique device ID which specifies the device used for that point. This is returned by this method.
   *
   * @SINCE_1_1.37
   * @param[in]  point  The point required.
   * @return The Device ID of this point.
   * @note If point is greater than GetPointCount() then this method will assert.
   */
  int32_t GetDeviceId( unsigned int point ) const;

  /**
   * @brief Retrieves the State of the point specified.
   *
   * @SINCE_1_1.37
   * @param[in]  point  The point required.
   * @return The state of the point specified.
   * @note If point is greater than GetPointCount() then this method will assert.
   * @see State
   */
  PointState::Type GetState( unsigned int point ) const;

  /**
   * @brief Retrieve the actor that was underneath the point specified.
   *
   * @SINCE_1_1.37
   * @param[in]  point  The point required.
   * @return The actor that was underneath the point specified.
   * @note If point is greater than GetPointCount() then this method will assert.
   */
  Actor GetHitActor( unsigned int point ) const;

  /**
   * @brief Retrieve the co-ordinates relative to the top-left of the hit-actor at the point specified.
   *
   * @SINCE_1_1.37
   * @param[in]  point  The point required.
   * @return The co-ordinates relative to the top-left of the hit-actor of the point specified.
   *
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   * @note If point is greater than GetPointCount() then this method will assert.
   */
  const Vector2& GetLocal( unsigned int point ) const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the screen of the point specified.
   *
   * @SINCE_1_1.37
   * @param[in]  point  The point required.
   * @return The co-ordinates relative to the top-left of the screen of the point specified.
   * @note If point is greater than GetPointCount() then this method will assert.
   */
  const Vector2& GetScreen( unsigned int point ) const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used internally to Create an initialized TouchData handle.
   *
   * @SINCE_1_1.37
   * @param [in] touchData A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL TouchData( Internal::TouchData* touchData );
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_TOUCH_DATA_H__
