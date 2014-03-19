#ifndef __DALI_DYNAMICS_COLLISION_H__
#define __DALI_DYNAMICS_COLLISION_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

class Actor;
struct Vector3;

namespace Internal DALI_INTERNAL
{

class DynamicsCollision;

} // namespace Internal

/**
 * @brief Contains information about a collision between two actors.
 */
class DynamicsCollision : public BaseHandle
{
public:
  /**
   * @brief Create an uninitialized handle.
   *
   * Initialized handles are received in DynamicsWorld::SignalCollision handlers
   */
  DynamicsCollision();

  /**
   * @brief Virtual destructor.
   */
  virtual ~DynamicsCollision();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:
  /**
   * @brief Get the first actor in the collision.
   *
   * @return The first actor in the collision
   */
  Actor GetActorA();

  /**
   * @brief Get the second actor in the collision.
   *
   * @return The second actor in the collision
   */
  Actor GetActorB();

  /**
   * @brief Get the force of the impact.
   *
   * @return the impact force
   * @note currently returns 0.5f for a new collision and 0.0f for dispersal
   */
  float GetImpactForce() const;

  /**
   * @brief Get the point of contact on the first Actor.
   *
   * @return The point of contact on the first Actor
   */
  Vector3 GetPointOnA() const;

  /**
   * @brief Get the point of contact on the second Actor.
   *
   * @return The point of contact on the second Actor
   */
  Vector3 GetPointOnB() const;

  /**
   * @brief Get the collision normal.
   *
   * @return The collision normal
   */
  Vector3 GetNormal() const;

  // Not intended for application developers
public:
  /**
   * @brief This constructor is used internally by Dali.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsCollision( Internal::DynamicsCollision* internal );
}; // class DynamicsCollision

} // namespace Dali

#endif /* __DALI_DYNAMICS_COLLISION_H__ */
