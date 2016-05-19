#ifndef __DALI_POINT_H__
#define __DALI_POINT_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

class Actor;
class Vector2;

/**
 * @brief A Point represents a point on the screen that is currently being touched/hovered or where touch/hover has stopped.
 *
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Point
{
public:

  // Enumerations

  /**
   * @brief Point state
   * @SINCE_1_1.36
   */
  enum State
  {
    STARTED,        /**< Touch or hover started */
    FINISHED,       /**< Touch or hover finished */
    DOWN = STARTED, /**< Screen touched */
    UP = FINISHED,  /**< Touch stopped */
    MOTION,         /**< Finger dragged or hovered */
    LEAVE,          /**< Leave the boundary of an actor */
    STATIONARY,     /**< No change from last event.  Useful when a multi-point event occurs where
                         all points are sent but indicates that this particular point has not changed
                         since the last time */
    INTERRUPTED,    /**< A system event has occurred which has interrupted the touch or hover event sequence. */
  };

  // Construction & Destruction

  /**
   * @brief Default constructor
   */
  Point();

  /**
   * @brief Constructor
   *
   * @SINCE_1_1.36
   * @param[in]  deviceId  The touch device ID.
   * @param[in]  state     The state.
   * @param[in]  screen    The co-ordinates relative to the screen's origin.
   */
  Point( int32_t id, State state, const Vector2& screen );

  /**
   * @brief Constructor
   *
   * @SINCE_1_1.36
   * @param[in]  deviceId  The touch device ID.
   * @param[in]  state     The state.
   * @param[in]  screen    The co-ordinates relative to the screen's origin.
   * @param[in]  local     The co-ordinates relative to the top-left (0.0, 0.0, 0.5) of the actor.
   */
  Point( int32_t id, State state, const Vector2& screen, const Vector2& local );

  /**
   * @brief Copy Constructor
   *
   * @SINCE_1_1.36
   * @param[in]  other  The Point to copy from.
   */
  Point( const Point& other );

  /**
   * @brief Destructor
   * @SINCE_1_1.36
   */
  ~Point();

  // Operators

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_1.36
   * @param[in]  other  The Point to copy from.
   */
  Point& operator=( const Point& other );

  // Getters

  /**
   * @brief Returns the ID of the device used for this Point.
   *
   * Each point has a unique device ID which specifies the device used for that point. This is returned by this method.
   *
   * @SINCE_1_1.36
   * @return The Device ID of this point.
   */
  int32_t GetDeviceId() const;

  /**
   * @brief Retrieves the State of the Point.
   *
   * @SINCE_1_1.36
   * @return The state of the Point.
   * @see State
   */
  State GetState() const;

  /**
   * @brief Retrieve the actor that was underneath the Point.
   *
   * @SINCE_1_1.36
   * @return The actor that was underneath the Point.
   */
  Actor GetHitActor() const;

  /**
   * @brief Retrieve the co-ordinates relative to the top-left of the hit-actor.
   *
   * @SINCE_1_1.36
   * @return The co-ordinates relative to the top-left of the hit-actor.
   *
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   */
  const Vector2& GetLocal() const;

  /**
   * @brief Retrieves the co-ordinates relative to the top-left of the screen.
   *
   * @SINCE_1_1.36
   * @return The co-ordinates relative to the top-left of the screen.
   */
  const Vector2& GetScreen() const;

  // Setters

  /**
   * @brief Sets the ID of the device used for this Point.
   *
   * @SINCE_1_1.36
   * @param[in]  deviceId  The Device ID to set.
   *
   * @see GetDeviceId
   */
  void SetDeviceId( int32_t deviceId );

  /**
   * @brief Sets the State of the Point.
   *
   * @SINCE_1_1.36
   * @param[in]  state  The state to set.
   *
   * @see GetState
   */
  void SetState( State state );

  /**
   * @brief Sets the actor that was underneath the Point.
   *
   * @SINCE_1_1.36
   * @param[in]  actor  The hit-actor.
   *
   * @see SetHitActor
   */
  void SetHitActor( Actor actor );

  /**
   * @brief Sets the co-ordinates relative to the top-left of the hit-actor.
   *
   * @SINCE_1_1.36
   * @param[in]  local  The local co-ordinates.
   *
   * @see GetLocal
   */
  void SetLocal( const Vector2& local );

  /**
   * @brief Sets the co-ordinates relative to the top-left of the screen.
   *
   * @SINCE_1_1.36
   * @param[in]  screen  The screen co-ordinates.
   *
   * @see GetScreen
   */
  void SetScreen( const Vector2& screen );

private:

  struct Impl;
  Impl* mImpl;
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_POINT_H__
