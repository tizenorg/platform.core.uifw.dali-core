#ifndef __DALI_INTERNAL_IMAGE_ACTOR_H__
#define __DALI_INTERNAL_IMAGE_ACTOR_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/actor-attachments/image-attachment-impl.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/images/nine-patch-image-impl.h>

namespace Dali
{

namespace Internal
{

class Image;

/**
 * An actor which displays an Image object.
 *
 * This handles image fade-in if required, waiting for the image to load.
 *
 * If a new image is set on the actor, then this ensures that the old image
 * is displayed until the new image is ready to render to prevent flashing
 * to the actor color. This will also happen if the image is reloaded.
 *
 * This is achieved by using two connector objects to Image: mImageNext and
 * mImageAttachment's member object. The first one points to the Image object that is going to
 * be displayed next, the second one to the Image that is currently being displayed.
 */
class ImageActor : public RenderableActor, public ConnectionTrackerInterface
{
public:

  typedef Dali::ImageActor::Style Style;
  typedef Dali::ImageActor::PixelArea PixelArea;

  /**
   * @brief Create an initialised image actor.
   * When the image is loaded the actors size will reset to the image size,
   * unless a custom size chosen via Actor:SetSize().
   * @return A smart-pointer to a newly allocated image actor.
   */
  static ImageActorPtr New();

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize() ;

  /**
   * Set the image rendered by the actor's attachment.
   * When the image is loaded the actors size will be reset to the image size,
   * unless a custom size is chosen via Actor:SetSize().
   * The old image will continue to be displayed until the new image has loaded
   * @param[in] ImagePtr reference to the image object to display. Reference to avoid unnecessary increment/decrement reference.
   */
  void SetImage( ImagePtr& image );

  /**
   * Retrieve the image rendered by the actor's attachment.
   * @return The image (uninitialized Image object in case the ImageActor does not display anything).
   */
  ImagePtr GetImage();

  /**
   * @copydoc Dali::ImageActor::SetToNaturalSize()
   */
  void SetToNaturalSize();

  /**
   * Set a region of the image to display, in pixels.
   * @param [in] pixelArea The area of the image to display.
   * This in pixels, relative to the top-left (0,0) of the image.
   */
  void SetPixelArea( const PixelArea& pixelArea );

  /**
   * Retrieve the region of the image to display, in pixels.
   * @return The pixel area, or a default-constructed area if none was set.
   */
  const PixelArea& GetPixelArea() const;

  /**
   * Query whether a pixel area has been set.
   * @return True if a pixel area has been set.
   */
  bool IsPixelAreaSet() const;

  /**
   * Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
   */
  void ClearPixelArea();

  /**
   * Set how the image is rendered; the default is STYLE_QUAD.
   * @param [in] style The new style.
   */
  void SetStyle( Style style );

  /**
   * Query how the image is rendered.
   * @return The rendering style.
   */
  Style GetStyle() const;

  /**
   * @copydoc Dali::ImageActor::SetNinePatchBorder
   */
  void SetNinePatchBorder( const Vector4& border, bool inPixels = false );

  /**
   * @copydoc Dali::ImageActor::GetNinePatchBorder
   */
  Vector4 GetNinePatchBorder() const;

  /**
   * Set whether the image should gradually fade in when first rendered.
   * @param [in] enableFade True if the image should fade in.
   */
  void SetFadeIn(bool enableFade);

  /**
   * Query whether the image will gradually fade in when first rendered.
   * @return True if the image will fade in.
   */
  bool GetFadeIn() const;

  /**
   * Set the duration of the fade-in effect; the default is 1 second.
   * @param [in] durationSeconds The duration in seconds.
   */
  void SetFadeInDuration( float durationSeconds );

  /**
   * Retrieve the duration of the fade-in effect.
   * @return The duration in seconds.
   */
  float GetFadeInDuration() const;

  /**
   * Retrieve the attachment which renders the image.
   * @return The attachment.
   */
  ImageAttachment& GetImageAttachment();

  /**
   * @copydoc Dali::ImageActor::GetCurrentImageSize
   */
  Vector2 GetCurrentImageSize() const;

public: // Default property extensions from ProxyObject

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const std::string& GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

private: // From RenderableActor

  /**
   * @copydoc RenderableActor::GetRenderableAttachment
   */
  virtual RenderableAttachment& GetRenderableAttachment() const;

private: // From ConnectionTrackerInterface

  /**
   * @copydoc SignalObserver::SignalConnected
   */
  virtual void SignalConnected( SlotObserver*, CallbackBase* );

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  virtual void SignalDisconnected(Dali::SlotObserver*, Dali::CallbackBase* );

protected:

  /**
   * Protected constructor; see also ImageActor::New()
   */
  ImageActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageActor();

private:

  /**
   * Helper to set the actor to the image's natural size
   * @param image that is used
   */
  void SetNaturalSize( ImagePtr& image );

  /**
   * From Actor.
   * This is called after SizeSet() has been called.
   */
  virtual void OnSizeSet( const Vector3& targetSize );

  /**
   * @copydoc Actor::OnSizeAnimation( Animation& animation, const Vector3& targetSize )
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);

  /**
   * From Actor; used to trigger fade-in animations.
   */
  virtual void OnStageConnectionInternal();

  /**
   * From Actor; used to notify Image.
   */
  virtual void OnStageDisconnectionInternal();

  /**
   * Helper for when the image loads.
   * @param[in] image The newly (re)loaded image.
   */
  void ImageLoaded( Dali::Image image );

  /**
   * @copydoc ImageLoaded( Dali::Image )
   */
  void ImageLoadedInternal( ImagePtr& image );

  /**
   * Helper to start fade-in animations.
   */
  void FadeIn();


private:

  ImageAttachmentPtr mImageAttachment; ///< Used to display the image (holds a pointer to currently showed Image)
  ImageConnector     mImageNext;       ///< Manages the Image this ImageActor will show (used when changing displayed image)

  // For fade-in animations
  float mFadeInDuration;  ///< Length of animation

  // flags, compressed to bitfield (uses only 4 bytes)
  bool mUsingNaturalSize:1;      ///< True only when the actor is using
  bool mInternalSetSize:1;       ///< True whilst setting size internally, false at all other times
  bool mFadeIn:1;                ///< True if fade in animation is enabled
  bool mFadeInitial:1;           ///< True if fading in for the first time

  static bool mFirstInstance ;
  static DefaultPropertyLookup* mDefaultImageActorPropertyLookup; ///< Default properties

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ImageActor& GetImplementation(Dali::ImageActor& image)
{
  DALI_ASSERT_ALWAYS(image && "Image handle is empty");

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::ImageActor&>(handle);
}

inline const Internal::ImageActor& GetImplementation(const Dali::ImageActor& image)
{
  DALI_ASSERT_ALWAYS(image && "Image handle is empty");

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::ImageActor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_ACTOR_H__
