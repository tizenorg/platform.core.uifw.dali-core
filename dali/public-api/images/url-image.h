#ifndef __DALI_URL_IMAGE_H__
#define __DALI_URL_IMAGE_H__

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
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/object/image.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{
struct Vector2;
class ImageAttributes;

namespace Internal DALI_INTERNAL
{
class Image;
}

/**
 * @brief UrlImage is an image loaded using a URL
 *
 * <h3>UrlImage Loading</h3>
 *
 * When the UrlImage is created, resource loading will be attempted unless
 * the UrlImage is created with ON_DEMAND loading policy or a compatible resource is found in cache.
 * In case of loading images on demand, resource loading will only be attempted if the associated ImageActor
 * is put on Stage.
 * Custom loading requests can be made by providing an ImageAttributes object to UrlImage::New().
 *
 * <i>LoadPolicies</i>
 * - IMMEDIATE: acquire image resource when creating UrlImage.
 * - ON_DEMAND: only load in case the associated ImageActor is put on Stage
 *
 * <i>Resolution of conflicting policies</i>
 * If the same image is created more than once with conflicting policies, LoadPolicy "IMMEDIATE" overrides "ON_DEMAND".
 *
 * <i>Custom load requests</i>
 * Size, scaling mode, orientation compensation can be set when requesting an image.
 * See ImageAttributes for more details.
 *
 * <i>Compatible resources</i>
 *
 * Before loading a new UrlImage the internal image resource cache is checked by dali.
 * If there is an image already loaded in memory and is deemed "compatible" with the requested image,
 * that resource is reused.
 * This happens for example if a loaded image exists with the same URL, and the difference between both
 * of the dimensions is less than 50%.
 *
 * <i>Reloading images</i>
 *
 * The same request used on creating the UrlImage is re-issued when reloading images.
 * If the file changed since the last load operation, this might result in a different resource.
 * Reload only takes effect if both of these conditions apply:
 * - The UrlImage has already finished loading
 * - The UrlImage is either on Stage or using IMMEDIATE load policy
 */
class DALI_IMPORT_API UrlImage : public Image
{
public:
  /**
   * @brief Resource management options.
   */

  /**
   * @brief LoadPolicy controls the way images are loaded into memory.
   */
  enum LoadPolicy
  {
    IMMEDIATE, ///< load image once it is created (default)
    ON_DEMAND  ///< delay loading until the image is being used (a related actor is added to Stage)
  };

  /**
   * @brief Type of signal for LoadingFinished and Uploaded.
   */
  typedef SignalV2< void (UrlImage) > UrlImageSignal;

  // Signal Names
  static const char* const SIGNAL_IMAGE_LOADING_FINISHED; ///< Name of LoadingFinished signal

public:

  /**
   * @brief Get the size of an image from disk.
   *
   * This function will read the header info from file on disk and is
   * synchronous, so it should not be used repeatedly or in tight
   * loops.
   *
   * @param [in] url The URL of the image file.
   * @return The width and height in pixels of the image.
   */
  static Vector2 GetImageSize( const std::string& url );

  /**
   * @brief Constructor which creates an empty Image object.
   *
   * Use UrlImage::New(...) to create an initialised object.
   */
  UrlImage();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~UrlImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  UrlImage( const UrlImage& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  UrlImage& operator=( const UrlImage& rhs );

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] url The URL of the image file to use.
   * @return A handle to a newly allocated object
   */
  static UrlImage New( const std::string& url );

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static UrlImage New( const std::string& url, LoadPolicy loadPol, ReleasePolicy releasePol );

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @return A handle to a newly allocated object
   */
  static UrlImage New( const std::string& url, const ImageAttributes& attributes );

  /**
   * @brief Create an initialised image object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] attributes Requested parameters for loading (size, scaling etc.).
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @return A handle to a newly allocated object
   */
  static UrlImage New( const std::string& url, const ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol );

  /**
   * @brief Downcast an Object handle to Image handle.
   *
   * If handle points to a Image object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static UrlImage DownCast( BaseHandle handle );

  /**
   * @brief Return load policy.
   *
   * @return resource load policy
   */
  LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Query whether the image data has loaded.
   *
   * The asynchronous loading begins when the Image object is created.
   * After the Image object is discarded, the image data will be released from memory.
   * @return The loading state, either Loading, Success or Failed.
   */
  LoadingState GetLoadingState() const;

  /**
   * @brief Returns the URL of the image.
   *
   * @param [in] The URL of the image file.
   */
  std::string GetUrl() const;

  /**
   * @brief Reload image from filesystem.
   *
   * The set ImageAttributes are used when requesting the image again.
   * @note if Image is offstage and OnDemand policy is set, reload request is ignored.
   */
  void Reload();

  /**
   * @brief Get the attributes of an image.
   *
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
   * Connect to SignalLoadingFinished or use GetLoadingState to make sure this value is actual.
   * @pre image should be loaded
   * @return a copy of the attributes
   */
  ImageAttributes GetAttributes() const;

public: // Signals

  /**
   * @brief Emitted when the image data loads successfully, or when the loading fails.
   *
   * @return A signal object to Connect() with.
   */
  UrlImageSignal& LoadingFinishedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL UrlImage( Internal::UrlImage* );
};

} // namespace Dali

#endif // __DALI_URL_IMAGE_H__
