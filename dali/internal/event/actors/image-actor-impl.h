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
#include <dali/devel-api/rendering/cull-face.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
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
class ImageActor : public Actor
{
public:

  typedef Dali::ImageActor::PixelArea PixelArea;

  /**
   * @brief Create an image actor instance.
   * @return A smart-pointer to a newly allocated image actor.
   */
  static ImageActorPtr New();

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize() ;

  /**
   * @see Dali::ImageActor::SetImage()
   * @param[in] ImagePtr reference to the image object to display. Reference to avoid unnecessary increment/decrement reference.
   */
  void SetImage( ImagePtr& image );

  /**
   * Retrieve the image rendered by the actor's attachment.
   * @return smart pointer to the image or an empty one if no image is assigned
   */
  ImagePtr GetImage() const;

  /**
   * @copydoc Dali::ImageActor::SetPixelArea()
   */
  void SetPixelArea( const PixelArea& pixelArea );

  /**
   * @copydoc Dali::ImageActor::GetPixelArea()
   */
  const PixelArea& GetPixelArea() const;

  /**
   * @copydoc Dali::ImageActor::IsPixelAreaSet()
   */
  bool IsPixelAreaSet() const;

  /**
   * @copydoc Dali::ImageActor::ClearPixelArea()
   */
  void ClearPixelArea();

  /**
   * @copydoc Dali::RenderableActor::SetSortModifier()
   */
  void SetSortModifier(float modifier);

  /**
   * @copydoc Dali::RenderableActor::GetSortModifier()
   */
  float GetSortModifier() const;

  /**
   * @copydoc Dali::RenderableActor::SetCullFace()
   */
  void SetCullFace(CullFaceMode mode);

  /**
   * @copydoc Dali::RenderableActor::GetCullFace()
   */
  CullFaceMode GetCullFace() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendMode()
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @copydoc Dali::RenderableActor::GetBlendMode()
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendFunc()
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba,   BlendingFactor::Type destFactorRgba );

  /**
   * @copydoc Dali::RenderableActor::SetBlendFunc()
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendEquation()
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @copydoc Dali::RenderableActor::SetBlendEquation()
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendEquation()
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendColor()
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @copydoc Dali::RenderableActor::GetBlendColor()
   */
  const Vector4& GetBlendColor() const;

  /**
   * @copydoc Dali::RenderableActor::SetFilterMode()
   */
  void SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter );

  /**
   * @copydoc Dali::RenderableActor::GetFilterMode()
   */
  void GetFilterMode( FilterMode::Type& minFilter, FilterMode::Type& magFilter ) const;

  /**
   * @brief Allows this ImageActor to respond to the event that the a shader effect's uniform has updated
   *
   * @param[in] uniform The shader effect uniform that has been updated
   */
  void EffectUniformUpdated( const ShaderEffect::Uniform& uniform );

  /**
   * @brief Allows this ImageActor to respond to the eventa that the shader effect's effect texture has been changed
   */
  void EffectImageUpdated();

public:
  /**
   * @copydoc Actor::SetShaderEffect
   */
  virtual void SetShaderEffect(ShaderEffect& effect);

  /**
   * @copydoc Actor::GetShaderEffect
   */
  virtual ShaderEffectPtr GetShaderEffect() const;

  /**
   * @copydoc Actor::RemoveShaderEffect
   */
  virtual void RemoveShaderEffect();

public: // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

public: // From Actor

  /**
   * @copydoc Dali::Actor::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize() const;

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
   * Calculate the natural size of this image actor
   *
   * @return Return the natural size as a Vector2
   */
  Vector2 CalculateNaturalSize() const;

  /**
   * @copydoc Dali::Actor::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

private:

  ShaderEffectPtr    mShaderEffect;            ///< Optional referenced shader effect
  RendererPtr        mRenderer;                ///< The renderer used to render the image
  PixelArea          mPixelArea;               ///< The pixel area of the image to render
  Vector4            mBlendColor;              ///< The blend color for this ImageActor
  int                mRendererIndex;           ///< The index location of mRenderer
  size_t             mTextureIndex;			   ///< The texture index for this ImageActor's texture
  size_t             mEffectTextureIndex;      ///< The texture index for this ImageActor's effect texture
  FilterMode::Type   mMinFilter;               ///< The minification filter currently set
  FilterMode::Type   mMagFilter;               ///< The magnification filter currently set
  bool               mIsPixelAreaSet;          ///< Flag indicating if the pixel area has been set
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
