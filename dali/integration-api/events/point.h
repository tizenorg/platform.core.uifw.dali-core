#ifndef __DALI_INTEGRATION_POINT_H__
#define __DALI_INTEGRATION_POINT_H__

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
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/events/touch-point.h>

namespace Dali
{

namespace Integration
{

/**
 * @brief A Point represents a point on the screen that is currently being touched or where touch has stopped.
 */
struct DALI_IMPORT_API Point
{
  /**
   * @brief Default Constructor
   */
  Point();

  /**
   * @brief Destructor
   */
  ~Point();

  /**
   * @brief Set the Unique Device ID.
   *
   * Each touch point has a unique device ID which specifies the touch device for that point.
   *
   * @param[in]  deviceId  The Unique Device ID.
   */
  void SetDeviceId( int deviceId );

  /**
   * @brief Set the state of the point.
   * @param[in]  state  The state of the point.
   */
  void SetState( PointState::Type state );

  /**
   * @brief Set the screen position of the point from the top-left of the screen.
   * @param[in]  screenPosition  The screen position of the point from the top-left of the screen.
   */
  void SetScreenPosition( const Vector2& screenPosition );

  /**
   * @brief Set the radius of the press point.
   *
   * This is the average of both the horizontal and vertical radii of the press point.
   * @param[in]  radius  The average of both the horizontal and vertical radii.
   */
  void SetRadius( float radius );

  /**
   * @brief Set the radius of the press point as an ellipse.
   * @param[in]  radius         The average of both the horizontal and vertical radii.
   * @param[in]  ellipseRadius  The horizontal and vertical radii of the press point (different if an ellipse).
   */
  void SetRadius( float radius, Vector2 ellipseRadius );

  /**
   * @brief Sets the angle of the press point relative to the Y-Axis.
   * @param[in]  angle  The angle of the press point relative to the Y-Axis.
   */
  void SetAngle( Degree angle );

  /**
   * @brief Retrieve the Unique Device ID of the point.
   * @return The Unique Device ID of the point.
   */
  int GetDeviceId() const;

  /**
   * @brief Retrieve the state of the point.
   * @return The state of the point.
   */
  PointState::Type GetState() const;

  /**
   * @brief Retrieve the screen position from the top-left of the screen.
   * @return The screen position from the top-left of the screen.
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief Retrieve the radius of the press point.
   * @return The radius of the press point.
   * @see SetRadius(float)
   * @see SetRadius(float,Vector2)
   */
  float GetRadius() const;

  /**
   * @brief Retrieve BOTH the horizontal and the vertical radii of the press point.
   * @return The radius of the press point.
   * @see SetRadius(float)
   * @see SetRadius(float,Vector2)
   */
  const Vector2& GetEllipseRadius() const;

  /**
   * @brief Retrieve the angle of the press point relative to the Y-Axis.
   * @return The angle of the press point.
   */
  Degree GetAngle() const;

public: // Not intended for Integration API developers

  /**
   * @brief Sets the hit actor under this point.
   * @param[in]  hitActor  The hit actor.
   */
  void SetHitActor( Actor hitActor );

  /**
   * @brief Set the co-ordinates relative to the top-left of the hit-actor.
   * @param[in]  localPosition  The local position.
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   */
  void SetLocalPosition( const Vector2& localPosition );

  /**
   * @brief Retrieve the Hit Actor.
   * @return The hit actor.
   */
  Actor GetHitActor() const;

  /**
   * @brief Retrieve the local position relative to the top-left of the hit-actor.
   * @return The local position.
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   */
  const Vector2& GetLocalPosition() const;

  /**
   * @brief Retrieve the touch point equivalent of this point for old API.
   * @return The touch point equivalent.
   */
  const TouchPoint& GetTouchPoint() const;

private:

  TouchPoint mTouchPoint; ///< Stores screen position, device Id, local & screen positions and the hit-actor. @see TouchPoint
  Vector2 mEllipseRadius; ///< Radius of both the horizontal and vertical radius (useful if an ellipse).
  float mRadius; ///< Radius of the press point, an average of the ellipse radius.
  Degree mAngle; ///< The angle of the press point, relative to the Y-Axis.
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_TOUCH_POINT_H__
