#ifndef DALI_SAMPLER_H
#define DALI_SAMPLER_H

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
#include <dali/public-api/images/image.h> // Dali::Image
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Sampler;
}

/**
 * @brief Sampler is a handle to an object that can be used to provide an image to a material.
 */
class DALI_IMPORT_API Sampler : public BaseHandle
{
public:

  /**
   * @brief Texture filter mode.
   */
  enum FilterMode
  {
    NONE,     ///< Use GL system defaults (minification NEAREST_MIPMAP_LINEAR, magnification LINEAR)
    DEFAULT,  ///< Use dali defaults (minification LINEAR, magnification LINEAR)
    NEAREST,  ///< Filter nearest
    LINEAR    ///< Filter linear
  };

  /**
   * @brief Texture wrap mode.
   */
  enum WrapMode
  {
    WRAPMODE_DEFAULT = 0, ///< Clamp to edge
    CLAMP_TO_EDGE,        ///< The texture coordinates get clamped to the range [0, 1]
    REPEAT,               ///< Only the fractional part of the texture coordinates is used
    MIRRORED_REPEAT,      ///< If the integer part of the texture coordinate is odd then it uses
                          ///< the fractional part, if it's even it uses 1 - the fractional part
  };

  /**
   * @brief An enumeration of properties belonging to the Sampler class.
   */
  struct Property
  {
    enum
    {
      MINIFICATION_FILTER = DEFAULT_OBJECT_PROPERTY_START_INDEX,  ///< name "minification-filter",  type STRING
      MAGNIGICATION_FILTER,                                       ///< name "magnification-filter", type STRING
      U_WRAP,                                                     ///< name "u-wrap",               type STRING
      V_WRAP
    };
  };

  /**
   * @brief Creates a new Sampler object
   *
   * @param[in] image Image used by this sampler
   * @param[in] uniformName String with the name of the uniform
   */
  static Sampler New();

  /**
   * @brief Default constructor, creates an empty handle
   */
  Sampler();

  /**
   * @brief Destructor
   */
  ~Sampler();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Sampler( const Sampler& handle );

  /**
   * @brief Downcast to a sampler handle.
   *
   * If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle or an uninitialized handle
   */
  static Sampler DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Sampler& operator=( const Sampler& handle );


  /**
   * @brief Set the filter modes for this sampler
   *
   * Calling this function sets the properties MINIFICATION_FILTER
   * and MAGNIFICATION_FILTER
   *
   * @param[in] minFilter The minification filter that will be used
   * @param[in] magFilter The magnification filter that will be used
   */
  void SetFilterMode( FilterMode minFilter, FilterMode magFilter );

  /**
   * @brief Set the wrap modes for this sampler
   *
   * Calling this function sets the properties U_WRAP and V_WRAP
   *
   * param[in] uWrap Wrap mode for u coordinates
   * param[in] vWrap Wrap mode for v coordinates
   */
  void SetWrapMode( WrapMode uWrap, WrapMode vWrap );

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Sampler
   */
  explicit DALI_INTERNAL Sampler( Internal::Sampler* pointer );
};

} //namespace Dali

#endif // DALI_SAMPLER_H
