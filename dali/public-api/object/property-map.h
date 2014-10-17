#ifndef __DALI_PROPERTY_MAP_H__
#define __DALI_PROPERTY_MAP_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/property-value.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief A Map of property values.
 */
class Property::Map
{
public:

  /**
   * @brief Default constructor.
   */
  Map();

  /**
   * @brief Copy Constructor.
   *
   * @param[in] other The Map to copy from.
   */
  Map( const Map& other );

  /**
   * @brief Non-virtual destructor.
   */
  ~Map();

  /**
   * @brief Retrieve the number of elements in the map.
   *
   * @return The number of elements in the map.
   */
  unsigned int Count() const;

  /**
   * @brief Retrieve the value at the specified position.
   *
   * @return A reference to the value at the specified position.
   *
   * @note Will assert if position >= Count() then this will assert
   */
  Value& GetValue( unsigned int position ) const;

  /**
   * @brief Retrieve the key at the specified position.
   *
   * @return A const reference to the key at the specified position.
   *
   * @note Will assert if position >= Count() then this will assert
   */
  const std::string& GetKey( unsigned int position ) const;

  /**
   * @brief Finds the value for the specified key if it exists.
   *
   * @param[in]  key   The key to find.
   *
   * @return A const pointer to the value if it exists, NULL otherwise
   */
  Value* Find( const std::string& key ) const;

  /**
   * @brief Clears the map.
   */
  void Clear();

  /**
   * @brief Const operator to access element with the specified key.
   *
   * @param[in] key The key whose value to access.
   *
   * @return The value for the element with the specified key, if key doesn't exist, then Property::NONE is returned.
   *
   * @note Will assert if invalid-key is given.
   */
  const Value& operator[]( const std::string& key ) const;

  /**
   * @brief Operator to access the element with the specified key.
   *
   * @param[in] key The key whose value to access.
   *
   * @return A reference to the value for the element with the specified key.
   *
   * @note If an element with the key does not exist, then it is created.
   */
  Value& operator[]( const std::string& key );

  /**
   * @brief Assignment Operator
   *
   * @param[in] other The map to copy from.
   *
   * @return The copied map.
   */
  Map& operator=( const Map& other );

private:
  struct Impl; ///< Private data
  Impl* mImpl; ///< Pointer to private data
};

} // namespace Dali

#endif // __DALI_PROPERTY_MAP_H__
