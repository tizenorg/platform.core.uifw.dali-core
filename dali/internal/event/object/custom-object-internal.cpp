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

// CLASS HEADER
#include <dali/internal/event/object/custom-object-internal.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

using Dali::Internal::SceneGraph::PropertyOwner;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::AnimatableProperty;

namespace
{
const std::string INVALID_PROPERTY_NAME;
}

namespace Dali
{

namespace Internal
{

CustomObject* CustomObject::New( CustomObjectImpl& extension )
{
  return new CustomObject( extension );
}

bool CustomObject::IsSceneObjectRemovable() const
{
  return false;
}

const SceneGraph::PropertyOwner* CustomObject::GetSceneObject() const
{
  return mUpdateObject;
}

const PropertyBase* CustomObject::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const PropertyBase* property( NULL );

  CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

  DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "index is invalid" );

  property = dynamic_cast<const PropertyBase*>( entry->second.GetSceneGraphProperty() );

  return property;
}

const PropertyInputImpl* CustomObject::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

unsigned int CustomObject::GetDefaultPropertyCount() const
{
  return 0u;
}

void CustomObject::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.clear();
}

const std::string& CustomObject::GetDefaultPropertyName( Property::Index index ) const
{
  return INVALID_PROPERTY_NAME;
}

Property::Index CustomObject::GetDefaultPropertyIndex(const std::string& name) const
{
  return Property::INVALID_INDEX;
}

bool CustomObject::IsDefaultPropertyWritable(Property::Index index) const
{
  return false;
}

bool CustomObject::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false;
}

Property::Type CustomObject::GetDefaultPropertyType(Property::Index index) const
{
  return Property::NONE;
}

void CustomObject::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // do nothing
}

void CustomObject::SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  DALI_ASSERT_ALWAYS( entry.IsAnimatable() && "CustomObject has only animatable properties" );

  switch ( entry.type )
  {
    case Property::BOOLEAN:
    {
      AnimatableProperty<bool>* property = dynamic_cast< AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<bool>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<bool>() );
      break;
    }

    case Property::FLOAT:
    {
      AnimatableProperty<float>* property = dynamic_cast< AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<float>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<float>() );
      break;
    }

    case Property::VECTOR2:
    {
      AnimatableProperty<Vector2>* property = dynamic_cast< AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector2>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector2>() );
      break;
    }

    case Property::VECTOR3:
    {
      AnimatableProperty<Vector3>* property = dynamic_cast< AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector3>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector3>() );
      break;
    }

    case Property::VECTOR4:
    {
      AnimatableProperty<Vector4>* property = dynamic_cast< AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector4>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector4>() );
      break;
    }

    case Property::ROTATION:
    {
      AnimatableProperty<Quaternion>* property = dynamic_cast< AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Quaternion>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Quaternion>() );
      break;
    }

    case Property::MATRIX:
    {
      AnimatableProperty<Matrix>* property = dynamic_cast< AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Matrix>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Matrix>() );
      break;
    }

    case Property::MATRIX3:
    {
      AnimatableProperty<Matrix3>* property = dynamic_cast< AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Matrix3>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Matrix3>() );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "Property type enumeration out of bounds"); // should not come here
      break;
    }
  }
}

Property::Value CustomObject::GetDefaultProperty(Property::Index index) const
{
  return Property::Value();
}

void CustomObject::InstallSceneObjectProperty( PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  if( NULL != mUpdateObject )
  {
    // mUpdateObject is being used in a separate thread; queue a message to add the property
    InstallCustomPropertyMessage( Stage::GetCurrent()->GetUpdateInterface(), *mUpdateObject, newProperty ); // Message takes ownership
  }
}

CustomObject::~CustomObject()
{
}

CustomObject::CustomObject( CustomObjectImpl& extension )
: mImpl( &extension )
{
  mUpdateObject = PropertyOwner::New();
}

} // namespace Internal

} // namespace Dali
