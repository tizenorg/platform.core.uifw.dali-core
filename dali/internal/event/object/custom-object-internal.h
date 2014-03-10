#ifndef __DALI_INTERNAL_CUSTOM_OBJECT_H__
#define __DALI_INTERNAL_CUSTOM_OBJECT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/internal/event/common/proxy-object.h>
#include <dali/public-api/object/custom-handle.h>
#include <dali/public-api/object/custom-object-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class PropertyOwner;
}

class CustomObject : public ProxyObject
{
public:

  /**
   * Create a new custom object.
   * @return A pointer to the newly allocated object.
   */
  static CustomObject* New( CustomObjectImpl& extension );

  /**
   * Retrieve the custom object implementation.
   * @return The implementation, or an invalid pointer if no implementation was set.
   */
  CustomObjectImpl& GetImplementation()
  {
    return *mImpl;
  }

  /**
   * Retrieve the custom object implementation.
   * @return The implementation.
   */
  const CustomObjectImpl& GetImplementation() const
  {
    return *mImpl;
  }

  /**
   * @copydoc Dali::Internal::ProxyObject::IsSceneObjectRemovable()
   */
  virtual bool IsSceneObjectRemovable() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

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

  /**
   * @copydoc Dali::Internal::ProxyObject::InstallSceneObjectProperty()
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CustomObject();

  /**
   * Private constructor; see also CustomObject::New()
   */
  CustomObject( CustomObjectImpl& extension );

  // Undefined
  CustomObject(const CustomObject&);

  // Undefined
  CustomObject& operator=(const CustomObject& rhs);

protected:

  CustomObjectImplPtr mImpl;

  SceneGraph::PropertyOwner* mUpdateObject;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CustomObject& GetImpl( Dali::CustomHandle& obj )
{
  DALI_ASSERT_ALWAYS( obj && "CustomHandle is empty" );

  BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::CustomObject&>(handle);
}

inline const Internal::CustomObject& GetImpl( const Dali::CustomHandle& obj )
{
  DALI_ASSERT_ALWAYS( obj && "CustomHandle is empty" );

  const BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::CustomObject&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_CUSTOM_OBJECT_H__
