#ifndef DALI_INTERNAL_MATERIAL_H
#define DALI_INTERNAL_MATERIAL_H

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
#include <dali/public-api/common/vector-wrapper.h> // std::vector

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/devel-api/rendering/material.h> // Dali::Material
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/common/property-buffer-impl.h> // Dali::Internal::PropertyBuffer
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Material;
}


class Material;
typedef IntrusivePtr<Material> MaterialPtr;

/**
 * Material is an object that connects a Shader with Samplers and can be used
 * to shade a Geometry.
 */
class Material : public Object, public Connectable
{
public:

  /**
   * @copydoc Dali::Material::New()
   */
  static MaterialPtr New();

  /**
   * @copydoc Dali::Material::SetShader()
   */
  void SetShader( Shader& shader );

  /**
   * @copydoc Dali::Material::GetShader()
   */
  Shader* GetShader() const;

  /**
   * @copydoc Dali::Material::AddTexture()
   */
  size_t AddTexture( ImagePtr image, const std::string& uniformName, Sampler* sampler );

  /**
   * @copydoc Dali::Material::RemoveTexture()
   */
  void RemoveTexture( size_t index );

  /**
   * @copydoc Dali::Material::SetTextureImage()
   */
  void SetTextureImage( size_t index, Image* image );

  /**
   * @copydoc Dali::Material::SetTextureSampler()
   */
  void SetTextureSampler( size_t index, Sampler* sampler );

  /**
   * @copydoc Dali::Material::SetTextureUniformName()
   */
  void SetTextureUniformName( size_t index, const std::string& uniformName );

  /**
   * @copydoc Dali::Material::GetTextureIndex()
   */
  int GetTextureIndex( const std::string& uniformName );

  /**
   * @copydoc Dali::Material::SetTextureAffectsTransparency()
   */
  void SetTextureAffectsTransparency( size_t index, bool affectsTransparency );

  /**
   * @copydoc Dali::Material::GetNumberOfTextures()
   */
  size_t GetNumberOfTextures() const;
  /**
   * @copydoc Dali::Material::SetFaceCullingMode()
   */
  void SetFaceCullingMode( Dali::Material::FaceCullingMode cullingMode );

  /**
   * @copydoc Dali::Material::SetBlendMode()
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @copydoc Dali::Material::GetBlendMode()
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @copydoc Dali::Material::SetBlendFunc()
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba );

  /**
   * @copydoc Dali::Material::SetBlendFunc()
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @copydoc Dali::Material::GetBlendFunc()
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @copydoc Dali::Material::SetBlendEquation()
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @copydoc Dali::Material::SetBlendEquation()
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @copydoc Dali::Material::GetBlendEquation()
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @copydoc Dali::Material::SetBlendColor()
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @copydoc Dali::Material::GetBlendColor()
   */
  const Vector4& GetBlendColor() const;

  /**
   * @brief Get the material scene object
   *
   * @return the material scene object
   */
  const SceneGraph::Material* GetMaterialSceneObject() const;

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
   * @copydoc Dali::Internal::Object::SetSceneGraphProperty()
   */
  virtual void SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyOwner()
   */
  virtual const SceneGraph::PropertyOwner* GetPropertyOwner() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyComponentIndex()
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

public: // Functions from Connectable
  /**
   * @copydoc Dali::Internal::Connectable::OnStage()
   */
  virtual bool OnStage() const;

  /**
   * @copydoc Dali::Internal::Connectable::Connect()
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Connectable::Disconnect()
   */
  virtual void Disconnect();

private: // implementation

  struct Texture
  {
    Texture()
    :mUniformName(""),
     mImage(NULL),
     mSampler( NULL )
    {}

    Texture( const std::string& name, ImagePtr image, Sampler* sampler )
    :mUniformName(name),
     mImage( image ),
     mSampler( sampler )
    {}

    std::string mUniformName;
    ImagePtr    mImage;
    SamplerPtr  mSampler;
  };

  Material();

  /**
   * Second stage initialization
   */
  void Initialize();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Material();

private: // unimplemented methods
  Material( const Material& );
  Material& operator=( const Material& );

private: //data
  IntrusivePtr<Shader> mShader; ///< Connector that holds the shader used by this material
  std::vector<Material::Texture> mTextures; ///<Vector of textures used by this material
  SceneGraph::Material* mSceneObject;

  BlendingMode::Type mBlendingMode; ///< Local store
  BlendingOptions mBlendingOptions; ///< Local copy of blending options bitmask
  bool mOnStage;

};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Material& GetImplementation( Dali::Material& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Material handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Material&>(object);
}

inline const Internal::Material& GetImplementation( const Dali::Material& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Material handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Material&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_MATERIAL_H
