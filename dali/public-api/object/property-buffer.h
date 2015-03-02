#ifndef DALI_PROPERTY_BUFFER_H
#define DALI_PROPERTY_BUFFER_H

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
#include <cstddef> // std::size_t
#include <string> // std::string

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property.h> // Dali::Property::Index

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class PropertyBuffer;
}

/**
 * @brief PropertyBuffer is a handle to an object that contains a buffer of structured properties
 */
class DALI_IMPORT_API PropertyBuffer : public Handle
{
public:

  /**
   * @brief Type of property buffer
   */
  enum Type
  {
    STATIC,     ///< The property buffer properties will not be animatable
    ANIMATABLE, ///< The property buffer properties will be animatable
  };

  /**
   * @brief Create a non-animatable PropertyBuffer
   *
   * Property buffers that are not animatable use less memory.
   *
   * @param[in] type Type of property buffer, will make properties animatable or not
   * @param[in] bufferFormat Map of names and types that describes the components of the buffer
   * @return Handle to a newly allocated PropertyBuffer
   */
  static PropertyBuffer New( Type type, Property::Map bufferFormat );


  /**
   * @brief Create an animatable PropertyBuffer
   *
   * @param[in] bufferFormat Map of names and types that describes the components of the buffer
   * @return Handle to a newly allocated PropertyBuffer
   */
  static PropertyBuffer NewAnimatable( Property::Map bufferFormat );

  /**
   * @brief Default constructor, creates an empty handle
   */
  PropertyBuffer();

  /**
   * @brief Destructor
   */
  ~PropertyBuffer();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  PropertyBuffer( PropertyBuffer& handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   */
  PropertyBuffer& operator=( PropertyBuffer& handle );

  /**
   * @brief Set the number of elements in the buffer
   *
   * @param[in] size Number of elements to expand or contract the buffer
   */
  void SetSize( std::size_t size );

  /**
   * @brief Get the number of elements in the buffer
   *
   * @return Number of elements to expand or contract the buffer
   */
  std::size_t GetSize();

  /**
   * @brief Update the whole buffer information
   *
   * This function expects a pointer to an array of structures with the same
   * format that was given in the construction, and the number of elements to
   * be the sane as the size of the buffer.
   *
   * If the initial structure was: { "position", VECTOR3
   *
   * @param[in] data A pointer to the data that will be copied to the buffer.
   */
  void SetData( void* data );

  /**
   * @brief Get the property index for a value in the buffer
   *
   * @param[in] name Name of the component
   * @param[in] index Index of the object in the buffer
   *
   * Indices are calculated as follows:
   *   NumberOfComponents * index + OffsetOf( name )
   * Example:
   *   For a PropertyBuffer with 2 components {"position", "normal"}
   *   The Property::Index for the property "position" in the 20th object would be:
   *     Property::Index( 2 * 19 + 0 ) => Property::Index( 38 )
   */
  Property::Index GetPropertyIndex( const std::string name, std::size_t index );

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated PropertyBuffer
   */
  explicit DALI_INTERNAL PropertyBuffer(Internal::PropertyBuffer* pointer);
};

} // namespace Dali

#endif // DALI_PROPERTY_BUFFER_H
