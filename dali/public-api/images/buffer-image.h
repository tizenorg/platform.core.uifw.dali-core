#ifndef __DALI_BUFFER_IMAGE_H__
#define __DALI_BUFFER_IMAGE_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class BufferImage;
}

typedef unsigned char         PixelBuffer;  ///< pixel data buffer
typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h)


/**
 * @brief BufferImage represents an image resource as a pixel data buffer
 *
 * If the pixel format of the pixel buffer contains an alpha channel,
 * then the image is considered to be have transparent pixels without
 * regard for the actual content of the channel, and will be blended.
 *
 * This object maintains a local pixel buffer, which may be modified
 * following a GetBuffer() call, and uploaded to GL via copy following
 * an Update() call. The pixel buffer lasts for the lifetime of this object.
 *
 * The GL memory is freed when this object is not staged ( i.e. it isn't used
 * by an on-stage actor, and automatically uploaded when it becomes staged.
 *
 * On context loss, the GL memory is lost, but the local pixel buffer remains
 * valid. On context regain, the pixel buffer is automatically uploaded for
 * staged images.
 */
class DALI_IMPORT_API BufferImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized BufferImage object.
   *
   * Use BufferImage::New(...) to create an initialised object.
   */
  BufferImage();

  /**
   * @brief Create a new BufferImage.
   *
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @pre width & height are greater than zero
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @return a handle to a new instance of BufferImage
   */
  static BufferImage New(unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief Create a new BufferImage.
   *
   * This will initialize the BufferImage's local copy using the given pixel buffer.
   *
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @note Supplying a smaller pixel buffer than the requested bitmap will
   * only copy to the start of the bitmap, and will not initialize the remaining
   * data.
   *
   * @pre width & height are greater than zero
   * @param [in] pixelBuffer  pixel buffer to copy
   * @param [in] width        image width in pixels
   * @param [in] height       image height in pixels
   * @param [in] pixelFormat  the pixel format (rgba 32 bit by default)
   * @param [in] stride       the internal stride of the pixelBuffer in pixels
   * @return a handle to a new instance of BufferImage
   */
  static BufferImage New( PixelBuffer*  pixelBuffer,
                          unsigned int  width,
                          unsigned int  height,
                          Pixel::Format pixelFormat=Pixel::RGBA8888,
                          unsigned int  stride=0 );

  /**
   * @brief Downcast an Object handle to BufferImage.
   *
   * If handle points to a BufferImage the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a BufferImage or an uninitialized handle
   */
  static BufferImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~BufferImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  BufferImage(const BufferImage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  BufferImage& operator=(const BufferImage& rhs);

  /**
   * @brief White pixel as image data.
   *
   * Can be used to create solid color actors.
   * @return 1 white pixel with 32 bit colordepth
   */
  static const BufferImage WHITE();

public:
  /**
   * @brief Returns the pixel buffer of the Image.
   *
   * The application can write to the buffer to modify its contents.
   *
   * Whilst the image is on stage, after writing to the buffer the
   * application should call Update().
   *
   * @return the pixel buffer
   */
  PixelBuffer* GetBuffer();

  /**
   * @brief Returns buffer size in bytes.
   *
   * @return the buffer size in bytes
   */
  unsigned int GetBufferSize() const;

  /**
   * @brief Returns buffer stride (in bytes).
   *
   * @return the buffer stride
   */
  unsigned int GetBufferStride() const;

  /**
   * @brief Returns the pixel format of the contained buffer
   *
   * @return the pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * @brief Inform Dali that the contents of the buffer have changed.
   *
   * SignalUploaded will be sent in response if the image is on stage
   * and the image data has been successfully copied to graphics
   * memory.
   *
   * @note: BufferImage::Update might not work with BGR/BGRA formats!
   * @note: Some GPUs may not support Non power of two buffer updates (for
   * example C110/SGX540)
   */
  void Update();

  /**
   * @copydoc Update()
   * @param [in] updateArea area that has changed in buffer
   */
  void Update( RectArea updateArea );


public: // Not intended for application developers

  explicit DALI_INTERNAL BufferImage(Internal::BufferImage*);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_BUFFER_IMAGE_H__
