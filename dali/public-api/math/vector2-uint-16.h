#ifndef __DALI_VECTOR_2_UINT_16_H__
#define __DALI_VECTOR_2_UINT_16_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * @brief Simple class for passing around pairs of small unsigned integers.
 *
 * Use this for integer dimensions and points with limited range such as image
 * sizes and pixel coordinates where a pair of floating point numbers is
 * inefficient and illogical (i.e. the data is inherently integer).
 * These are immutable. If you want to change a value, make a whole new object.
 * @note One of these can be passed in a single 32 bit integer register on
 * common architectures.
 */
class Vector2Uint16
{
public:
  /**
   * @brief Default constructor for the (0, 0) vector.
   */
  Vector2Uint16() : mData(0) {}

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @param[in] width The width or X dimension of the vector. Make sure it is less than 65536,
   * @param[in] height The height or Y dimension of the vector. Make sure it is less than 65536,
   */
  Vector2Uint16( uint32_t width, uint32_t height )
  {
    DALI_ASSERT_DEBUG( width < ( 1u << 16 ) && "Width parameter not representable." );
    DALI_ASSERT_DEBUG( height < ( 1u << 16 ) && "Height parameter not representable." );

    /* Do equivalent of the code below with one aligned memory access:
     * mComponents[0] = width;
     * mComponents[1] = height;
     * Unit tests make sure this is equivalent.
     **/
    mData = (height << 16u) + width;
  }

  /**
   * @brief Copy constructor.
   */
  Vector2Uint16( const Vector2Uint16& rhs )
  {
    mData = rhs.mData;
  }

  /**
   * @returns the x dimension stored in this 2-tuple.
   */
  uint16_t GetWidth() const
  {
    return mComponents[0];
  }

  /**
   * @returns the y dimension stored in this 2-tuple.
   */
  uint16_t GetHeight() const
  {
    return mComponents[1];
  }

  /**
   * @returns the x dimension stored in this 2-tuple.
   */
  uint16_t GetX()  const
  {
    return mComponents[0];
  }

  /**
   * @returns the y dimension stored in this 2-tuple.
   */
  uint16_t GetY() const
  {
    return mComponents[1];
  }

  /**
   * ...
   */
  bool operator==( const Vector2Uint16& rhs ) const
  {
    return mData == rhs.mData;
  }

  bool operator!=( const Vector2Uint16& rhs ) const
  {
    return mData != rhs.mData;
  }

  bool operator<( const Vector2Uint16& rhs ) const
  {
    return mData < rhs.mData;
  }

  bool operator>( const Vector2Uint16& rhs ) const
  {
    return mData > rhs.mData;
  }

  template<typename FLOAT_VEC>
  static Vector2Uint16 FromFloatVec2( const FLOAT_VEC& from)
  {
    return Vector2Uint16( from.x + 0.5f, from.y + 0-.5f );
  }

  template<typename FLOAT_ARRAY>
  static Vector2Uint16 FromFloatArray( const FLOAT_ARRAY& from)
  {
    return Vector2Uint16( from[0] + 0.5f, from[0] + 0-.5f );
  }

private:
  union
  {
    // Addressable view of X and Y:
    uint16_t mComponents[2];
    // Packed view of X and Y to force alignment and allow a faster copy:
    uint32_t mData;
  };
};

} // namespace Dali

#endif // __DALI_VECTOR_2_UINT_16_H__
