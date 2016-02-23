#ifndef __DALI_IMAGE_H__
#define __DALI_IMAGE_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Image;
}

/**
 * @brief An Image object represents an image resource that can be used for rendering
 *
 * Image objects can be shared between Actors. This is practical if you have a visual element on screen
 * which is repeatedly used.
 *
 * Image objects are responsible for the underlying resource's lifetime.
 * Note: if a resource was shared between Image objects it exists until its last reference is gone.
 *
 * Signals
 * | %Signal Name           | Method                       |
 * |------------------------|------------------------------|
 * | uploaded               | @ref UploadedSignal()        |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Image : public BaseHandle
{
public:

  /**
   * @DEPRECATED_1_1.3. Image resource is released as soon as last handle is released.
   * @brief ReleasePolicy controls the way images are deleted from memory.
   * @SINCE_1_0.0
   */
  enum ReleasePolicy
  {
    UNUSED, ///< release resource once image is not in use anymore (eg. all actors using it become offstage). Reload when resource is required again. @SINCE_1_0.0
    NEVER   ///< keep image data for the lifetime of the object. (default) @SINCE_1_0.0
  };

  /**
   * @brief Type of signal for Image Uploaded.
   * @SINCE_1_0.0
   */
  typedef Signal< void (Image) > ImageSignalType;

public:

  /**
   * @brief Constructor which creates an empty Image handle.
   *
   * Use Image::New(...) to create an initialised handle.
   * @SINCE_1_0.0
   */
  Image();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Image();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  Image(const Image& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Image& operator=(const Image& rhs);

  /**
   * @brief Downcast an Object handle to Image handle.
   *
   * If handle points to a Image object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static Image DownCast( BaseHandle handle );

  /**
   * @DEPRECATED_1_1.3
   *
   * @brief Return resource release policy.
   *
   * @SINCE_1_0.0
   * @return resource release policy
   */
  ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Returns the width of the image.
   *
   * Returns either the requested width or the actual loaded width if no specific size was requested.
   *
   * @SINCE_1_0.0
   * @return width of the image in pixels.
   */
  unsigned int GetWidth() const;

  /**
   * @brief Returns the height of the image.
   *
   * Returns either the requested height or the actual loaded height if no specific size was requested.
   *
   * @SINCE_1_0.0
   * @return height of the image in pixels.
   */
  unsigned int GetHeight() const;

public: // Signals

  /**
   * @brief This signal is emitted when the image data gets uploaded to GL.
   *
   * It Will be sent after an actor using the image is added to
   * the stage, when such a staged image is reloaded, or when a staged
   * BufferImage calls Update().
   * @SINCE_1_0.0
   * @return A signal object to Connect() with.
   */
  ImageSignalType& UploadedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL Image(Internal::Image*);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_IMAGE_H__
