#ifndef DALI_INTERNAL_RENDERER_H__
#define DALI_INTERNAL_RENDERER_H__

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
#include <dali/public-api/actors/renderer.h>        // Dali::Renderer
#include <dali/public-api/common/dali-common.h>     // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>   // Dali::IntrusivePtr
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object

namespace Dali
{
namespace Internal
{

class Renderer;
typedef IntrusivePtr<Renderer> RendererPtr;

/**
 * Renderer is an object that can be used to show content by combining a Geometry with a material.
 */
class Renderer : public Object
{
public:

  /**
   * Create a new Renderer.
   * @return A smart-pointer to the newly allocated Renderer.
   */
  static RendererPtr New();


  /**
   * @copydoc Dali::Renderer::SetGeometry()
   */
  void SetGeometry( Geometry geometry );

  /**
   * @copydoc Dali::Renderer::SetMaterial()
   */
  void SetMaterial( Material material );

  /**
   * @copydoc Dali::Renderer::SetDepthIndex()
   */
  void SetDepthIndex( int depthIndex );

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
  virtual void SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value );

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

};

// Helpers for public-api forwarding methods
inline Internal::Renderer& GetImplementation(Dali::Renderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Renderer&>(object);
}

inline const Internal::Renderer& GetImplementation(const Dali::Renderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Renderer&>(object);
}

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_RENDERER_H__
