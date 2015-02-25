#ifndef __DALI_SAMPLER_H__
#define __DALI_SAMPLER_H__

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

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Sampler;
}

/**
 * @brief Sampler is handle to an object that can be used to provide an image to a material.
 */
class DALI_IMPORT_API Sampler : public Constrainable
{
public:

  /**
   * @brief Texture filter mode.
   */
  enum FilterMode
  {
    NONE,       ///< Use GL system defaults (minification NEAREST_MIPMAP_LINEAR, magnification LINEAR)
    DEFAULT,    ///< Use dali defaults (minification LINEAR, magnification LINEAR)
    NEAREST,    ///< Filter nearest
    LINEAR      ///< Filter linear
  };

  /**
   * @brief Texture wrap mode.
   */
  enum WrapMode
  {
    CLAMP_TO_EDGE,    ///< The texture coordinates get clamped to the range [0, 1]
    REPEAT,           ///< Only the fractional part of the texture coordinates is used
    MIRRORED_REPEAT,  ///< If the integer part of the texture coordinate is odd then it uses
                      ///< the fractional part, if it's even it uses 1 - the fractional part
  };

  /**
   * @brief Creates a new Sampler object
   *
   * @return An handle to a newly allocated Sampler
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
  Sampler( Sampler& handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   */
  Sampler& operator=( Sampler& handle );

  /**
   * @brief Creates a new Sampler object
   *
   * @param[in] uniformName String with the name of the uniform
   */
  static Sampler New( std::string uniformName );

  /**
   * @brief Sets the name of the shader uniform that will use this sampler
   *
   * @param[in] name String with the name of the uniform
   */
  void SetUniformName( std::string name );

  /**
   * @brief Set the image used by this sampler
   *
   * @param[in] image Image used by this sampler
   */
  void SetImage( Image image );

  /**
   * @brief Set the filter modes for this sampler
   *
   * @param[in] minFilter The minification filter that will be used
   * @param[in] minFilter The magnification filter that will be used
   */
  void SetFilterMode( FilterMode minFilter, FilterMode magFilter );

  /**
   * @brief Set the wrap modes for this sampler
   *
   * param[in] uWrap Wrap mode for u coordinates
   * param[in]vuWrap Wrap mode for v coordinates
   */
  void SetWrapMode( WrapMode uWrap, WrapMode vWrap );

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated PropertyBuffer
   */
  explicit DALI_INTERNAL Sampler(Internal::PropertyBuffer* pointer);
};

} //namespace Dali



#endif // __DALI_SAMPLER_H__
