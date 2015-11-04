#ifndef __DALI_INTERNAL_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_BUFFER_IMAGE_H__

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
#include <dali/integration-api/bitmap.h> // For Integration::BitmapPtr
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/internal/event/images/context-recovery-interface.h>

namespace Dali
{

namespace Internal
{

class BufferImage;
typedef IntrusivePtr<BufferImage> BufferImagePtr;

class ResourceClient;
class ResourceManager;

/**
 * BufferImage represents an image resource that can be added to
 * actors etc. It maintains a bitmap with a pixel buffer allocated by
 * Dali, and remains allocated for the lifetime of this
 * object. Uploads to GL are done via copy, so this local buffer is
 * never used by RenderThread.
 */
class BufferImage : public Image, public ContextRecoveryInterface
{
public:
  /**
   * @copydoc Dali::BufferImage::New()
   */
  static BufferImagePtr New( unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat );

  /**
   * @copydoc Dali::BufferImage::New()
   */
  static BufferImagePtr New( PixelBuffer*  pixBuf,
                             unsigned int  width,
                             unsigned int  height,
                             Pixel::Format pixelformat,
                             unsigned int  stride );

  /**
   * Create a new BufferImage, which uses external data source.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   */
  BufferImage( PixelBuffer*  pixBuf,
               unsigned int  width,
               unsigned int  height,
               Pixel::Format pixelformat,
               unsigned int  stride );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BufferImage();

public:
  /**
   * @copydoc Dali::BufferImage::Update(RectArea)
   */
  void Update (RectArea& updateArea);

  /**
   * @copydoc Dali::BufferImage::GetBuffer()
   */
  PixelBuffer* GetBuffer();

  /**
   * @copydoc Dali::BufferImage::GetBuffer()
   */
  unsigned int GetBufferSize() const;

  /**
   * @copydoc Dali::BufferImage::GetBuffer()
   */
  unsigned int GetBufferStride() const;

  /**
   * @copydoc Dali::BufferImage::GetBuffer()
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * @brief Upload the local pixel data to a specified resource at an offset
   *
   * @param destId ResourceId of the destination
   * @param xOffset x offset in the destination
   * @param yOffset y offset in the destination
   */
  void UploadBitmap( ResourceId destId, std::size_t xOffset, std::size_t yOffset );

  /**
   * @copydoc ContextRecoveryInterface::RecoverFromContextLoss
   */
  virtual void RecoverFromContextLoss();

protected: // From Resource
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
   * Create a bitmap of the given dimensions, possibly with some initial data.
   *
   * @param[in] pixBuf The pixel buffer to copy from. May be NULL
   * @param[in] width The width of the bitmap
   * @param[in] height The height of the bitmap
   * @param[in] pixelFormat The pixel format of the bitmap
   * @param[in] stride The stride of the pixBuf, if present
   * @param[in] discardPolicy The discard policy of the bitmap
   */
  Integration::BitmapPtr CreateBitmap(
    PixelBuffer* pixBuf,
    unsigned int width,
    unsigned int height,
    Pixel::Format pixelFormat,
    unsigned int stride,
    ResourcePolicy::Discardable discardPolicy );

  /**
   * Create a separate bitmap for passing to Texture Cache to enforce
   * separation of pixel buffers. This clone will discard it's pixel
   * buffer after upload
   */
  void CreateTicket();

  /**
   * This copies an area of the source bitmap ( that held by this BufferImage )
   * into the destination buffer. Enough space must have been allocated to hold
   * this area.
   * @param[in] dest The destination pixel buffer
   * @param[in] area The area of the source to copy from
   */
  void CopyPixelArea( PixelBuffer* dest, const RectArea& area );

  /**
   * Register newly created images
   */
  static void RegisterImage( BufferImage* bufferImage );

  /**
   * Deregister images that are being destroyed.
   */
  static void DeregisterImage( BufferImage* bufferImage );

private:
  ResourceClient*        mResourceClient;
  Integration::BitmapPtr mBitmap;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::BufferImage& GetImplementation(Dali::BufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BufferImage handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::BufferImage&>(handle);
}

inline const Internal::BufferImage& GetImplementation(const Dali::BufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BufferImage handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::BufferImage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_BUFFER_IMAGE_H__
