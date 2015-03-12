#ifndef __DALI_DEGREE_H__
#define __DALI_DEGREE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{

/**
 * @brief An angle in degrees.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 */
struct Degree
{
  /**
   * @brief default constructor, initialises to 0.
   */
  Degree()
  : degree( 0.f )
  { }

  /**
   * @brief Create an angle in degrees.
   *
   * @param[in] value The initial value in degrees.
   */
  explicit Degree( float value )
  : degree( value )
  { }

  /**
   * @brief Compare equality between two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if the value is identical
   */
  bool operator==( const Degree& rhs ) const
  {
    return fabsf( degree - rhs.degree ) < GetRangedEpsilon( degree, rhs.degree );
  }

  /**
   * @brief Compare inequality between two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if the value is not identical
   */
  bool operator!=( const Degree& rhs ) const
  {
    return !(this->operator==(rhs));
  }

  /**
   * @brief Compare two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if this is less than the value
   */
  bool operator<( const Degree& rhs ) const
  {
    return degree < rhs.degree;
  }

  /**
   * @brief Assign an angle from a float value.
   *
   * @param[in] value Float value in degrees
   * @return a reference to this
   */
  Degree& operator=( float value )
  {
    degree = value;
    return *this;
  }

public:

  // member data
  float degree; ///< The value in degrees

};

/**
 * @brief Create an angle in degrees from an angle in radians.
 *
 * @return value in degrees.
 */
inline Degree DegreeFromRadian( float radian )
{
  Degree degree( radian * Math::ONE80_OVER_PI );
  return degree;
}

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

} // namespace Dali

#endif // __DALI_DEGREE_H__
