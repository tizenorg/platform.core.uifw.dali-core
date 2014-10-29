#ifndef __DALI_RANDOM_H__
#define __DALI_RANDOM_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>

namespace Dali
{

/**
 * @brief Provides methods to generate and use random values.
 */
namespace Random
{

/**
 * @brief Returns a random number between f0 and f1.
 *
 * Note, uses a limited number of values.
 * @param[in] f0 the lower bound
 * @param[in] f1 the upper bound
 * @return a random value between the lower and upper bound
 */
inline float Range(float f0, float f1)
{
  float min = std::min(f0, f1);
  float max = std::max(f0, f1);
  return min + (rand() & 0xfff) * (max-min) * (1.0f/4095.0f);
}

/**
 * @brief Function to return a normalized axis in a random direction.
 *
 * @return the axis
 */
inline Vector4 Axis()
{
  Vector4 axis;
  // This function needs to return a vector with direction, that is to say a non-zero vector
  // There is a possibility that a random vector will be a zero vector, so a loop is needed to ensure that a non-zero vector is returned
  do
  {
    axis.x = Range(-1.0f, 1.0f);
    axis.y = Range( 0.0f, 1.0f);
    axis.z = Range( 0.0f, 1.0f);
    axis.w = 0.0f;
  } while (axis == Vector4::ZERO);
  axis.Normalize();
  return axis;
}

/**
 * @brief Returns true if the value given is greater than a random value between 0 and 1.
 *
 * @param chance  A value between 0 and 1. [Default: 0.5]
 * @return        true if chance greater than the random value, otherwise false.
 */
inline bool Chance(float chance = 0.5f)
{
  return chance > Range(0.0f, 1.0f);
}

} // namespace Random

} // namespace Dali

#endif // __DALI_RANDOM_H__
