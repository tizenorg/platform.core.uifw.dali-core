#ifndef __DALI_PROPERTY_ARRAY_H__
#define __DALI_PROPERTY_ARRAY_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

/**
 * @brief A Array of property values.
 */
class DALI_IMPORT_API Property::Array
{
public:

  typedef std::size_t SizeType;

  /**
   * @brief Default constructor.
   */
  Array();

  /**
   * @brief Copy Constructor.
   *
   * @param[in] other The Array to copy from.
   */
  Array( const Array& other );

  /**
   * @brief Non-virtual destructor.
   */
  ~Array();

  /**
   * @brief Retrieve the number of elements in the array.
   *
   * @return The number of elements in the array.
   */
  SizeType Size() const
  {
    return Count();
  }

  /**
   * @brief Retrieve the number of elements in the array.
   *
   * @return The number of elements in the array.
   */
  SizeType Count() const;

  /**
   * @brief Returns whether the array is empty.
   *
   * @return true if empty, false otherwise
   */
  bool Empty() const
  {
    return Count() == 0;
  }

  /**
   * @brief Clears the array.
   */
  void Clear();

  /**
   * @brief Increase the capcity of the array.
   */
  void Reserve( SizeType size );

  /**
   * @brief Resize to size.
   */
  void Resize( SizeType size );

  /**
   * @brief Retrieve the capacity of the array.
   *
   * @return The allocated capacity of the array
   */
  SizeType Capacity();

  /**
   * @brief Add an element to the array.
   *
   * @param[in] value The value to add to the end of the array
   */
  void PushBack( const Value& value );

  /**
   * @brief Const access an element.
   *
   * @param[in] index The element index to access. No bounds checking is performed.
   *
   * @return The a reference to the element.
   */
  const Value& GetElementAt( SizeType index ) const
  {
    return operator[]( index );
  }

  /**
   * @brief Access an element.
   *
   * @param[in] index The element index to access. No bounds checking is performed.
   *
   * @return The a reference to the element.
   */
  Value& GetElementAt( SizeType index )
  {
    return operator[]( index );
  }

  /**
   * @brief Const operator to access an element.
   *
   * @param[in] index The element index to access. No bounds checking is performed.
   *
   * @return The a reference to the element.
   *
   */
  const Value& operator[]( SizeType index ) const;

  /**
   * @brief Operator to access an element.
   *
   * @param[in] index The element index to access. No bounds checking is performed.
   *
   * @return The a reference to the element.
   *
   */
  Value& operator[]( SizeType index );

  /**
   * @brief Assignment Operator
   *
   * @param[in] other The array to copy from.
   *
   * @return The copied array.
   */
  Array& operator=( const Array& other );

private:
  struct DALI_INTERNAL Impl; ///< Private data
  Impl* mImpl; ///< Pointer to private data
};

} // namespace Dali

#endif // __DALI_PROPERTY_ARRAY_H__
