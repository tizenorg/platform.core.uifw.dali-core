#ifndef DALI_INTERNAL_PROPERTY_BUFFER_H__
#define DALI_INTERNAL_PROPERTY_BUFFER_H__

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
#include <dali/public-api/common/dali-common.h>     // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>   // Dali::IntrusivePtr
#include <dali/public-api/geometry/geometry.h>      // Dali::Geometry
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object

namespace Dali
{
namespace Internal
{

class Geometry;
typedef IntrusivePtr<Geometry> GeometryPtr;

/**
 * Geometry is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Geometry : public Object
{
public:

  /**
   * Create a new Geometry.
   * @return A smart-pointer to the newly allocated Geometry.
   */
  static GeometryPtr New();

  /**
   * @copydoc Dali::Geometry::AddVertexBuffer()
   */
  std::size_t AddVertexBuffer( PropertyBuffer vertexBuffer );

  /**
   * @copydoc Dali::Geometry::GetNumberOfVertexBuffers()
   */
  std::size_t GetNumberOfVertexBuffers() const;

  /**
   * @copydoc Dali::Geometry::RemoveVertexBuffer()
   */
  void RemoveVertexBuffer( std::size_t index );

  /**
   * @copydoc Dali::Geometry::SetIndexBuffer()
   */
  void SetIndexBuffer( PropertyBuffer indexBuffer );

  /**
   * @copydoc Dali::Geometry::SetGeometryType()
   */
  void SetGeometryType( GeometryType geometryType );

  /**
   * @copydoc Dali::Geometry::GetGeometryType()
   */
  GeometryType GetGeometryType();

  /**
   * @copydoc Dali::Geometry::SetRequiresDepthTesting()
   */
  void SetRequiresDepthTesting( bool requiresDepthTest );

  /**
   * @copydoc Dali::Geometry::GetRequiresDepthTesting()
   */
  bool GetRequiresDepthTesting();


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
inline Internal::Geometry& GetImplementation(Dali::Geometry& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Geometry handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Geometry&>(object);
}

inline const Internal::Geometry& GetImplementation(const Dali::Geometry& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Geometry handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Geometry&>(object);
}

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_BUFFER_H__
