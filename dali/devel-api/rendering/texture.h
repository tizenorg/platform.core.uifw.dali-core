#ifndef DALI_TEXTURE_H
#define DALI_TEXTURE_H

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX
#include <dali/public-api/images/pixel.h>
namespace Dali
{

namespace Internal DALI_INTERNAL
{
class NewTexture;
}

struct TextureUploadParams
{
  unsigned int layer,mipmap,x0,y0,width,height;
};

struct TextureCreateParams
{
  //FERRAN : TODO
};

namespace TextureType
{
  enum Type
  {
    TEXTURE_2D,
    TEXTURE_CUBE
  };

}
/**
 * @brief Geometry is handle to an object that can be used to define a geometric elements.
 */
class DALI_IMPORT_API Texture : public BaseHandle
{
public:

  /**
   * @brief Creates a new Texture object
   *
   * @return An handle to a newly allocated Geometry
   */
  static Texture New( TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height, bool hasMipmaps );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Texture();

  /**
   * @brief Destructor
   */
  ~Texture();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Texture( const Texture& handle );

  /**
   * @brief Downcast to a texture.
   *
   * If not the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return geometry handle or an uninitialized handle
   */
  static Texture DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Texture& operator=( const Texture& handle );

  /**
   * Upload data to the texture
   * FERRAN Comment parameters
   */
  void Upload( Vector<unsigned char>& buffer, const TextureUploadParams& params );

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Texture
   */
  explicit DALI_INTERNAL Texture( Internal::NewTexture* pointer );
};

} //namespace Dali

#endif // DALI_TEXTURE_H
