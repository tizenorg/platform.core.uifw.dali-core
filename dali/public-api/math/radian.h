#ifndef __DALI_RADIAN_H__
#define __DALI_RADIAN_H__

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
#include <dali/public-api/math/degree.h>

namespace Dali
{

/**
 * @brief An angle in radians.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 */
struct Radian
{
  /**
   * @brief default constructor, initialises to 0.
   */
  Radian()
  : radian( 0.f )
  { }

  /**
   * @brief Create an angle in radians.
   *
   * @param[in] value The initial value in radians.
   */
  explicit Radian( float value )
  : radian( value )
  { }

  /**
   * @brief Create an angle in radians from an angle in degrees.
   *
   * @param[in] value The initial value in degrees.
   */
  explicit Radian( Degree degree )
  : radian( degree.degree * Math::PI_OVER_180 )
  { }

  /**
   * @brief Compare equality between two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if the value is identical
   */
  bool operator==( const Radian& rhs ) const
  {
    return fabsf( radian - rhs.radian ) < GetRangedEpsilon( radian, rhs.radian );
  }

  /**
   * @brief Compare inequality between two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if the value is not identical
   */
  bool operator!=( const Radian& rhs ) const
  {
    return !(this->operator==(rhs));
  }

  /**
   * @brief Compare two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if this is less than the value
   */
  bool operator<( const Radian& rhs ) const
  {
    return radian < rhs.radian;
  }

  /**
   * @brief Assign an angle from a float value.
   *
   * @param[in] value Float value in radians
   * @return a reference to this object
   */
  Radian& operator=( float value )
  {
    radian = value;
    return *this;
  }

public:

  // member data
  float radian; ///< The value in radians

};

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

} // namespace Dali

#endif // __DALI_RADIAN_H__
