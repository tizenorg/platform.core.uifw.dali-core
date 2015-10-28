#ifndef __DALI_RESOURCE_IMAGE_H__
#define __DALI_RESOURCE_IMAGE_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/images/image-operations.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class ResourceImage;
}

/**
 * @brief ResourceImage is an image loaded using a URL
 *
 * <h3>ResourceImage Loading</h3>
 *
 * When the ResourceImage is created, resource loading will be attempted unless a compatible resource is found in cache.
 * Scaling of images to a desired smaller size can be requested by providing desired dimensions,
 * scaling mode and filter mode to to ResourceImage::New().
 *
 * <i>Custom load requests</i>
 * Size, scaling mode, filter mode, and orientation compensation can be set when requesting an image.
 *
 * <i>Compatible resources</i>
 *
 * Before loading a new ResourceImage the internal image resource cache is checked by DALi.
 * If there is an image already loaded in memory and is deemed "compatible" with the requested image,
 * that resource is reused.
 * This happens for example if a loaded image exists with the same URL, scaling and filtering modes,
 * and the difference between both of the dimensions is less than a few pixels.
 *
 * <i>Reloading images</i>
 *
 * The same request used on creating the ResourceImage is re-issued when reloading images.
 * If the file changed since the last load operation, this might result in a different resource.
 * Reload only takes effect if the ResourceImage has already finished loading
 *
 * Signals
 * | %Signal Name           | Method                       |
 * |------------------------|------------------------------|
 * | image-loading-finished | @ref LoadingFinishedSignal() |
 */
class DALI_IMPORT_API ResourceImage : public Image
{
public:

  /**
   * @deprecated DALi 1.1.3 Image loading starts immediately in the frame when then ResourceImage object is created
   *
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
  typedef Signal< void (ResourceImage) > ResourceImageSignal;

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
  static ImageDimensions GetImageSize( const std::string& url );

  /**
   * @brief Constructor which creates an empty ResourceImage object.
   *
   * Use ResourceImage::New(...) to create an initialised object.
   */
  ResourceImage();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ResourceImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  ResourceImage( const ResourceImage& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  ResourceImage& operator=( const ResourceImage& rhs );

  /**
   * @name ResourceImageFactoryFunctions
   * Create ResourceImage object instances using these functions.
   */
  ///@{

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * Uses defaults for all options.
   *
   * @sa Dali::FittingMode::Type Dali::SamplingMode::Type
   * @param [in] url The URL of the image file to use.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, bool orientationCorrection = true );

  /**
   * @deprecated DALi 1.1.3 use New( const std::string& url ) instead.
   *
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url, LoadPolicy loadPol, ReleasePolicy releasePol, bool orientationCorrection = true );

  /**
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url,
                            ImageDimensions size,
                            FittingMode::Type fittingMode = FittingMode::DEFAULT,
                            SamplingMode::Type samplingMode = SamplingMode::DEFAULT,
                            bool orientationCorrection = true );

  /**
   * @deprecated DALi 1.1.3 use New( const std::string& url, ImageDimensions size ) instead.
   *
   * @brief Create an initialised ResourceImage object.
   *
   * @param [in] url The URL of the image file to use.
   * @param [in] loadPol    The LoadPolicy to apply when loading the image resource.
   * @param [in] releasePol The ReleasePolicy to apply to Image.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return A handle to a newly allocated object
   */
  static ResourceImage New( const std::string& url,
                            LoadPolicy loadPol,
                            ReleasePolicy releasePol,
                            ImageDimensions size,
                            FittingMode::Type fittingMode = FittingMode::DEFAULT,
                            SamplingMode::Type samplingMode = SamplingMode::DEFAULT,
                            bool orientationCorrection = true );

  ///@}

  /**
   * @brief Downcast an Object handle to ResourceImage handle.
   *
   * If handle points to a ResourceImage object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Image object or an uninitialized handle
   */
  static ResourceImage DownCast( BaseHandle handle );

  /**
   * @deprecated DALi 1.1.3
   *
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
   * @return The URL of the image file.
   */
  std::string GetUrl() const;

  /**
   * @brief Reload image from filesystem.
   *
   * The original set of image loading attributes (requested dimensions, scaling
   * mode and filter mode) are used when requesting the image again.
   * @note If image is offstage and OnDemand policy is set, the reload request is
   * ignored.
   */
  void Reload();

public: // Signals

  /**
   * @brief Emitted when the image data loads successfully, or when the loading fails.
   *
   * @return A signal object to Connect() with.
   */
  ResourceImageSignal& LoadingFinishedSignal();

public: // Not intended for application developers

  explicit DALI_INTERNAL ResourceImage( Internal::ResourceImage* );
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_RESOURCE_IMAGE_H__
