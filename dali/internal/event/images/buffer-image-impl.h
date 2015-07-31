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

namespace Dali
{

namespace Internal
{

class BufferImage;
typedef IntrusivePtr<BufferImage> BufferImagePtr;

class ResourceClient;
class ResourceManager;

/**
 * BufferImage represents an image resource that can be added to actors etc.
 * Its pixel buffer data is provided by the application developer.
 * Pixel buffer memory allocation can be handled by dali or application.
 */
class BufferImage : public Image
{
public:
  /**
   * Create a new BufferImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BufferImagePtr New( unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Create a new BufferImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * An internal copy is made of the Pixel Buffer, which can then be freed by the Application, unless if there will be a call to Update() later.
   * The buffer should only be freed when there is no chance of an Update() being called again.
   * Obtaining the buffer with GetBuffer() and altering the contents, then Update() will not work with externally owned buffers.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @deprecated Support for externally owned Pixel Buffers is due to be removed TBA.
   * It is recommended that a BufferImage owned Buffer be used instead.

   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BufferImagePtr New( PixelBuffer* pixBuf,
                             unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             unsigned int stride,
                             ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Create a new BufferImage.
   * Also a pixel buffer for image data is allocated.
   * An internal copy is made of the Pixel Buffer, which can then be freed by the Application, unless if there will be a call to Update() later.
   * The buffer should only be freed when there is no chance of Update() being called again.
   * Note: obtaining the buffer with GetBuffer(), writing changes, then Update() will cause any changes to be lost.
   * In this case, the BufferImage will update from the external buffer and so changes should be written there.
   * For better performance use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width image width in pixels
   * @param [in] height image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] releasePol optionally release memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BufferImage(unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT);

  /**
   * Create a new BufferImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * An internal copy is made of the Pixel Buffer, which can then be freed by the Application, unless if there will be a call to Update() later.
   * The buffer should only be freed when there is no chance of Update() being called again.
   * Note: obtaining the buffer with GetBuffer(), writing changes, then Update() will cause any changes to be lost.
   * In this case, the BufferImage will update from the external buffer and so changes should be written there.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BufferImage(PixelBuffer* pixBuf,
              unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              unsigned int stride,
              ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT);

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BufferImage();

public:
  /**
   * Notify Dali that the contents of the buffer have changed.
   * @param [in] updateArea area that has changed in buffer. An empty rect means the whole buffer has changed.
   */
  void Update (RectArea& updateArea);

  /**
   * @copydoc Dali::BufferImage::IsDataExternal
   */
  bool IsDataExternal() const;

  /**
   * Returns the pixel buffer of the Image.
   * The application developer can write to the buffer.
   * Upload the modified contents with Update().
   * @return the pixel buffer
   */
  PixelBuffer* GetBuffer() const
  {
    return ( mExternalBuffer == NULL ? mInternalBuffer : mExternalBuffer );
  }

  /**
   * Returns buffer size in bytes.
   * @return the buffer size in bytes
   */
  unsigned int GetBufferSize() const
  {
    return mBufferSize;
  }

  /**
   * Returns buffer stride (in bytes).
   * @return the buffer stride
   */
  unsigned int GetBufferStride() const
  {
    return mPixelStride * mBytesPerPixel;
  }

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

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

  void UpdateBufferArea( PixelBuffer* src, const RectArea& area );

  ImageTicketPtr AllocateBitmapImage( uint32_t width,
                                      uint32_t height,
                                      uint32_t bufferWidth,
                                      uint32_t bufferHeight,
                                      Pixel::Format pixelformat );

private:

  PixelBuffer*               mExternalBuffer;       ///< NULL if there is no external pixel data (this is never owned by BufferImage).
  PixelBuffer*               mInternalBuffer;       ///< internal Pixel Buffer to write to from applications.
  ResourceClient*            mResourceClient;       ///< pointer to the resource client.
  Integration::BitmapPtr     mBitmap;               ///< pointer to the bitmap object containing the pixel buffer used to update GL.
  uint32_t                   mBufferSize;           ///< size of the internal buffer.
  uint32_t                   mPixelStride;          ///< width in pixels of an external row of pixel data.
  uint32_t                   mBytesPerPixel;        ///< width of a pixel in bytes.
  Pixel::Format              mPixelFormat;          ///< pixel format of internal or external buffer.
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
