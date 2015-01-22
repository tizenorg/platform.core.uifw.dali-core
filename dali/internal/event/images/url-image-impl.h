#ifndef __DALI_INTERNAL_URL_IMAGE_H__
#define __DALI_INTERNAL_URL_IMAGE_H__

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
#include <dali/public-api/images/image.h>
#include <dali/internal/event/images/image.h>

namespace Dali
{

namespace Internal
{

typedef Dali::UrlImage::LoadPolicy LoadPolicy;

const LoadPolicy IMAGE_LOAD_POLICY_DEFAULT = Dali::Image::IMMEDIATE;

/**
 * Image represents an image resource that can be added to actors etc.
 * When the Image object is created, resource loading will be attempted.
 * Provided this is successful, the resource will exist until the Image is destroyed.
 */
class UrlImage : public Image
{
  /**
   * Creates a pointer to an uninitialized Image object.
   * @return a pointer to a newly created object.
   */
  static UrlImage* New();

  /**
   * Creates object and loads image from filesystem
   * the maximum size of the image is limited by GL_MAX_TEXTURE_SIZE
   * @param [in] url The URL of the image file.
   * @param [in] attributes requested parameters for loading (size, scaling etc.)
   *                        if width or height is specified as 0, the natural size will be used.
   * @param [in] loadPol controls time of loading a resource from the filesystem (default: load when Image is created).
   * @param [in] releasePol optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   * @return a pointer to a newly created object.
   */
  static UrlImage* New( const std::string& url,
                        const Dali::ImageAttributes& attributes,
                        LoadPolicy loadPol = IMAGE_LOAD_POLICY_DEFAULT,
                        ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * @copydoc Dali::UrlImage::GetLoadingState()
   */
  Dali::LoadingState GetLoadingState() const { return mTicket ? mTicket->GetLoadingState() : ResourceLoading; }

  /**
   * @copydoc Dali::UrlImage::GetLoadPolicy()
   */
  LoadPolicy GetLoadPolicy () const { return mLoadPolicy; }

  /**
   * @copydoc Dali::UrlImage::LoadingFinishedSignal()
   */
  Dali::UrlImage::ImageSignalV2& LoadingFinishedSignal() { return mLoadingFinishedV2; }

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * Get the attributes of the image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return a copy of the attributes
   */
  const Dali::ImageAttributes& GetAttributes() const;

  /**
   * @copydoc Dali::UrlImage::GetUrl()
   */
  const std::string& GetUrl() const;

  /**
   * @copydoc Dali::UrlImage::Reload()
   */
  void Reload();

public: // From ResourceTicketObserver

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingFailed()
   */
  virtual void ResourceLoadingFailed(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingSucceeded()
   */
  virtual void ResourceLoadingSucceeded(const ResourceTicket& ticket);

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~UrlImage();

  /**
   * Constructor, with default parameters
   */
  UrlImage( LoadPolicy loadPol = ImageLoadPolicyDefault, ReleasePolicy releasePol = IMAGE_LOAD_POLICY_DEFAULT );

  /**
   * Second stage initialization
   */
  void Initialize();

private:

  /**
   * Helper method to determine if the filename indicates that the image has a 9 patch border.
   * @param[in] filename The filename to check
   * @return true if it is a 9 patch image
   */
  static bool IsNinePatchFileName( const std::string& filename );

private:

  Dali::UrlImage::ImageSignalV2 mLoadingFinishedV2;

  LoadPolicy mLoadPolicy : 2;    ///< 2 bits is enough space

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::UrlImage& GetImplementation(Dali::UrlImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::UrlImage&>(handle);
}

inline const Internal::UrlImage& GetImplementation(const Dali::UrlImage& image)
{
  DALI_ASSERT_ALWAYS( image && "Image handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::UrlImage&>(handle);
}

} // namespace Dali
#endif // __DALI_INTERNAL_URL_IMAGE_H__
