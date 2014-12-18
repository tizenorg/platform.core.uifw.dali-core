#ifndef __DALI_TEXTURE_ATLAS_H__
#define __DALI_TEXTURE_ATLAS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class TextureAtlas;
}

typedef unsigned char         PixelBuffer;  ///< pixel data buffer

class DALI_IMPORT_API TextureAtlas : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized TextureAtlas object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  TextureAtlas();

  static TextureAtlas New(unsigned int width,
                          unsigned int height,
                          Pixel::Format pixelformat=Pixel::RGBA8888);

  static TextureAtlas New( const Vector2& size, Pixel::Format pixelformat=Pixel::RGBA8888 );

  enum AtlasResult
  {
    SUCCESS,
    ADD_SUCCESS_SORTED,
    ADD_SUCCESS_SIZE_INCREASED,
    ADD_MAX_SIZE_FAILED,
    REMOVE_FAILED_IMAGE_NOT_IN_ATLAS
  };

  struct TextureAtlasResult
  {
    AtlasResult result;
    float u;
    float v;
    unsigned int x;
    unsigned int y;
  };

  TextureAtlasResult Add( const unsigned int width, const unsigned int height );


  enum ResizePolicy
  {
    RESIZE_AUTOMATICALLY,
    RESIZE_ERROR_ON_NO_ROOM,
    RESIZE_ERROR_ON_MAX_SIZE,
  };

  TextureAtlasResult Remove( const BitmapImage& bitmap );

  void SetSpacing( unsigned int pixels );

  void ShowBorders( const bool show );

  /**
   * @brief Downcast an Object handle to Texture Atlas.
   *
   * If handle points to a TextureAtlas the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a TextureAtlas or an uninitialized handle
   */
  static TextureAtlas DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextureAtlas();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  TextureAtlas(const TextureAtlas& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  TextureAtlas& operator=(const TextureAtlas& rhs);

public:
  /**
   * @brief Returns the pixel buffer of the Atlas.
   *
   * The application cannot write directly to the buffer to modify its contents.
   * Instead images must be added and removed using the exposed methods
   *
   * @return the pixel buffer
   */
  PixelBuffer* GetAtlasBuffer();

  /**
   * @brief Returns buffer size in bytes.
   *
   * @return the buffer size in bytes
   */
  unsigned int GetAtlasBufferSize() const;

  const Vector2 GetAtlasSize() const;

private:

  void Update();

public: // Not intended for application developers

  explicit DALI_INTERNAL TextureAtlas(Internal::TextureAtlas*);
};

} // namespace Dali

#endif // __DALI_TEXTURE_ATLAS_H__
