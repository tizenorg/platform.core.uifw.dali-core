#ifndef __DALI_INTERNAL_ATLAS_H__
#define __DALI_INTERNAL_ATLAS_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/atlas.h>
#include <dali/internal/event/images/context-recovery-interface.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/buffer-image-impl.h>

namespace Dali
{

namespace Internal
{

class ResourceClient;

/**
 * @brief An Atlas is a large image containing multiple smaller images.
 *
 * Bitmap images must be uploaded at a specified position, to populate the Atlas.
 * The client is reponsible for generating the appropriate geometry (UV coordinates),
 * needed to draw images within the Atlas.
 */
class Atlas : public Image, public ContextRecoveryInterface
{
public:

  /**
   * @brief Create a new Atlas.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       The atlas width in pixels.
   * @param [in] height      The atlas height in pixels.
   * @param [in] pixelFormat The pixel format (rgba 32 bit by default).
   * @return A pointer to a new Atlas.
   */
  static Atlas* New( unsigned int width,
                     unsigned int height,
                     Pixel::Format pixelFormat = Pixel::RGBA8888 );

  /**
   * @copydoc Dali::Atlas::Clear
   */
  void Clear( const Vector4& color  );

  /**
   * @copydoc Dali::Atlas::Upload( const BufferImage&, unsigned int, unsigned int )
   */
  bool Upload( BufferImage& bufferImage,
               unsigned int xOffset,
               unsigned int yOffset );

  /**
   * @copydoc Dali::Atlas::Upload( const std::string&, unsigned int, unsigned int )
   */
  bool Upload( const std::string& url,
               unsigned int xOffset,
               unsigned int yOffset );

  /**
   * @copydoc ContextRecoveryInterface::RecoverFromContextLoss
   */
  virtual void RecoverFromContextLoss();

protected:

  /**
   * @brief Protected constructor.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       The atlas width in pixels.
   * @param [in] height      The atlas height in pixels.
   * @param [in] pixelFormat The pixel format (rgba 32 bit by default).
   */
  Atlas( unsigned int width,
         unsigned int height,
         Pixel::Format pixelFormat );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Atlas();

  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

private:

  /**
   * Helper for Upload methods
   * @return True if the bitmap has the same pixel format and its size fits within the atlas at the specified offset
   */
  bool Compatible( Pixel::Format pixelFormat,
                   unsigned int x,
                   unsigned int y );
  /**
   * Helper to create the Atlas resource
   */
  void AllocateAtlas();

  /**
   * Helper to release the Atlas resource
   */
  void ReleaseAtlas();

  /**
   * Upload a bitmap with the given color to clear the background.
   */
  void ClearBackground( const Vector4& color  );

  /**
   * Clear all the current tiles and resources of the atlas
   */
  void ClearCache();

private:

  struct Tile
  {
    Tile( unsigned int xOffset, unsigned int yOffset, unsigned int sourceIndex, bool isBufferImage )
    : xOffset( xOffset ), yOffset( yOffset ), sourceIndex(sourceIndex), isBufferImage( isBufferImage )
    {}

    unsigned int xOffset;
    unsigned int yOffset;
    unsigned int sourceIndex;
    bool isBufferImage:1;

    ~Tile(){};

  private:
    Tile(const Tile& rhs); ///< not defined
    Tile& operator=(const Tile& rhs); ///< not defined
  };

  struct UrlResource
  {
    UrlResource( const std::string& url )
    : bitmap( NULL ),
      url( url )
    {}

    void LoadBitmap();

    Integration::BitmapPtr bitmap;
    std::string url;

     ~UrlResource()
    {
      bitmap.Reset();
    }

  private:
    UrlResource(const UrlResource& rhs); ///< not defined
    UrlResource& operator=(const UrlResource& rhs); ///< not defined
  };

private:

  ResourceClient&              mResourceClient;
  Vector4                      mClearColor;
  std::vector<Tile*>           mTiles;
  std::vector<BufferImagePtr>  mBufferImages;
  std::vector<UrlResource*>    mUrlResources;
  Pixel::Format                mPixelFormat;
  bool                         mClear:1;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Atlas& GetImplementation(Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::Atlas&>(handle);
}

inline const Internal::Atlas& GetImplementation(const Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::Atlas&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_H__
