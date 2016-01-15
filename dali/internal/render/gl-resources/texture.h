#ifndef __DALI_INTERNAL_TEXTURE_H__
#define __DALI_INTERNAL_TEXTURE_H__

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
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/images/pixel-data.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/render/gl-resources/texture-units.h>


namespace Dali
{

class NativeImage;

namespace Internal
{

class Context;
struct Vertex2D;

/**
 * Texture class.
 */
class Texture: public RefObject,
               public GlResourceOwner
{
public:

  /**
   * Used to define the area of the texture to display
   */
  typedef Rect<int> PixelArea;

  /**
   * Used to define a region of a bitmap.
   */
  typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h)

  /**
   * Initialization method for Texture.
   * Might or might not be needed in specific implementations.
   * @return true if successful, false otherwise
   */
  virtual bool Init() = 0;

  /**
   * Update the texture with the bitmap.
   */
  virtual void Update(Integration::Bitmap* bitmap);

  /**
   * Update the texture from the modified bitmap.
   * @param area to update
   */
  virtual void UpdateArea( const RectArea& area );

  /**
   * Update part of the texture with a different bitmap
   * @param[in] srcBitmap The bitmap to copy from
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  virtual void Update( Integration::Bitmap* srcBitmap, std::size_t xOffset, std::size_t yOffset ) {}

  /**
   * Update part of the texture with a pixel buffer
   * @param[in] srcPixelData The pixel data to copy from
   * @param [in] xOffset Specifies an offset in the x direction within the texture
   * @param [in] yOffset Specifies an offset in the y direction within the texture
   */
  virtual void Update( PixelData* srcPixelData, std::size_t xOffset, std::size_t yOffset ) {}

  /**
   * @return Return true if the texture should be updated on GL texture creation.
   */
  virtual bool UpdateOnCreate();

  /**
   * Binds the texture for use.
   * If there is no GL texture yet, it tries to create one.
   *
   * @param target (e.g. GL_TEXTURE_2D)
   * @param textureunit to bind to
   * @return True if the opengl texture was created, false if there was already a texture
   * or no texture could be created yet ( e.g. no bitmap data after context loss )
   */
  virtual bool Bind(GLenum target, TextureUnit textureunit);

  /**
   * Returns GL texture ID
   * @return texture id
   */
  unsigned int GetTextureId()
  {
    return mId;
  }

  /**
   * Return the width of image in pixels.
   * @return width
   */
  virtual unsigned int GetWidth() const;

  /**
   * Return the height of image in pixels.
   * @return height
   */
  virtual unsigned int GetHeight() const;

  /**
   * Query whether the texture data has an alpha channel.
   * @return True if the texture data has an alpha channel.
   */
  virtual bool HasAlphaChannel() const = 0;

  /**
   * Query whether the texture is completely opaque
   * @return True if all pixels of the texture data are opaque
   */
  virtual bool IsFullyOpaque() const = 0;

  /**
   * Sets the texture id.
   * @param id OpenGL texture id
   */
  void SetTextureId(GLuint id);

  /**
   * @brief Apply the given sampler to the texture.
   *
   * @param[in] texture unit to use
   * @param[in] samplerBitfield A bitfield with packed sampler options.
   */
  void ApplySampler( TextureUnit unit, unsigned int samplerBitfield );

protected:

  /**
   * Constructor.
   * @param[in] context The GL context
   * @param[in] width       The buffer width
   * @param[in] height      The buffer height
   * @param[in] imageWidth  The image width
   * @param[in] imageHeight The image height
   */
  Texture( Context&      context,
           unsigned int  width,
           unsigned int  height,
           unsigned int  imageWidth,
           unsigned int  imageHeight );
  /**
   * Constructor.
   * @param[in] context The GL context
   * @param[in] width       Both the buffer width and the image width (they are equal)
   * @param[in] height      Both the buffer height and the image height.
   */
  Texture( Context&      context,
           unsigned int  width,
           unsigned int  height );
public:
  /**
   * Initialize texture for rendering.
   * @return true on success
   */
  virtual bool CreateGlTexture() = 0;

  /**
   * Destructor.
   * Delete the GL texture associated with it.
   */
  virtual ~Texture();

public: // From GlResourceOwner

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

private:

  // Undefined
  Texture(const Texture&);

  // Undefined
  Texture& operator=(const Texture& rhs);

  /**
   * @brief Apply the given texture filtering parameters.
   *
   * @param[in] texture unit to use
   * @param[in] filterType Minification or magnification.
   * @param[in] currentFilterMode The current filter mode.
   * @param[in] newFilterMode The new filter mode.
   * @param[in] daliDefault The default dali filter mode for the given filterType.
   * @param[in] systemDefault The default system filter mode for the given filterType.
   */
  void ApplyFilterModeParameter( TextureUnit unit, GLint filterType, FilterMode::Type currentFilterMode, FilterMode::Type newFilterMode, GLint daliDefault, GLint systemDefault );

  /**
   * @brief Apply the given texture wrap mode.
   *
   * @param[in] texture unit to use
   * @param[in] wrapType UWrap or VWrap
   * @param[in] currentWrapMode The current wrap mode.
   * @param[in] newWrapMode The new wrap mode.
   */
  void ApplyWrapModeParameter( TextureUnit unit, GLint wrapType, WrapMode::Type currentWrapMode, WrapMode::Type newWrapMode );

protected:

  Context&      mContext;      ///< The GL Context

  GLuint        mId;           ///< Texture id

  unsigned int  mSamplerBitfield;    ///< The packed bitfield of the current sampler

  unsigned int  mWidth;        ///< texture width, may be scaled power of 2 (if not in an atlas)
  unsigned int  mHeight;       ///< texture width, may be scaled power of 2 (if not in an atlas)

  unsigned int  mImageWidth;   ///< width of the original image (may be smaller than texture width)
  unsigned int  mImageHeight;  ///< height of the original image (may be smaller than texture height)
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXTURE_H__
