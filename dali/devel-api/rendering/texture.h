#ifndef DALI_TEXTURE_H
#define DALI_TEXTURE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/pixel.h>
#include <dali/devel-api/images/pixel-data.h>
#include <dali/public-api/images/native-image-interface.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class NewTexture;
}

namespace TextureType
{
/**
 * @brief Texture types
 */
enum Type
{
  TEXTURE_2D,   ///< One 2D image
  TEXTURE_CUBE  ///< Six 2D images arranged in a cube-shape
};
}

namespace CubeMap
{
/**
 * @brief Faces of a cube map
 */
  const unsigned int POSITIVE_X  = 0u;
  const unsigned int NEGATIVE_X  = 1u;
  const unsigned int POSITIVE_Y  = 2u;
  const unsigned int NEGATIVE_Y  = 3u;
  const unsigned int POSITIVE_Z  = 4u;
  const unsigned int NEGATIVE_Z  = 5u;
}


/**
 * @brief Texture represents a texture object used as input or output by shaders
 */
class DALI_IMPORT_API Texture : public BaseHandle
{
public:

  /**
   * @brief Creates a new Texture object
   *
   * @param[in] type The type of the texture
   * @param[in] format The format of the pixel data
   * @param[in] width The width of the texture
   * @param[in] height The height of the texture
   * @return A handle to a newly allocated Texture
   */
  static Texture New( TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height );

  /**
   * @brief Creates a new Texture object
   * @param[in] nativeImageInterface A reference to the object of the interface implementation.
   * @return A handle to a newly allocated Texture
   */
  static Texture New( NativeImageInterface& nativeImageInterface );

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
   * @return texture handle or an uninitialized handle
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
   * @brief Upload data to the texture
   * @param[in] buffer A vector with the data to be uploaded
   * @note buffer data will be moved after this call. If the application needs to keep the data,
   * it will have to pass a copy of the original buffer into this function, otherwise the data
   * will be lost
   */
  void Upload( Vector<unsigned char>& buffer );

  /**
   * @brief Upload data to the texture
   * @param[in] buffer A vector with the data to be uploaded
   * @param[in] layer Specifies the layer of a cube map or array texture. (Unused for 2D textures)
   * @param[in] mipmap Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image
   * @param[in] xOffset Specifies a texel offset in the x direction within the texture array
   * @param[in] yOffset Specifies a texel offset in the y direction within the texture array
   * @param[in] width Specifies the width of the texture subimage
   * @param[in] height Specifies the height of the texture subimage
   * @note buffer data will be moved after this call. If the application needs to keep the data,
   * it will have to pass a copy of the original buffer into this function, otherwise the data
   * will be lost
   */
  void Upload( Vector<unsigned char>& buffer,
               unsigned int layer, unsigned int mipmap,
               unsigned int xOffset, unsigned int yOffset,
               unsigned int width, unsigned int height );

  /**
   * @brief Upload data to the texture from a PixelData object
   * @param[in] pixelData The pixelData object
   */
  void Upload( PixelData pixelData );

  /**
   * @brief Generate mipmaps for the texture
   * This will auto generate all the mipmaps for the texture based on the data in the base level
   */
  void GenerateMipmaps();

  /**
   * @brief Returns de width of the texture
   * @return The width, in pixels, of the texture
   */
  unsigned int GetWidth() const;

  /**
   * @brief Returns de height of the texture
   * @return The height, in pixels, of the texture
   */
  unsigned int GetHeight() const;

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
