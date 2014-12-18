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

// CLASS HEADER
#include <dali/internal/event/common/proxy-object.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/event/animation/active-constraint-base.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/common/type-registry-impl.h>

using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{
const int SUPPORTED_CAPABILITIES = Dali::Handle::DYNAMIC_PROPERTIES;  // ProxyObject provides this capability
typedef Dali::Vector<ProxyObject::Observer*>::Iterator ObserverIter;
typedef Dali::Vector<ProxyObject::Observer*>::ConstIterator ConstObserverIter;

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_PROXY_OBJECT" );
#endif
} // unnamed namespace

ProxyObject::ProxyObject()
: mTypeInfo( NULL ),
  mConstraints( NULL ),
  mPropertyNotifications( NULL )
{
}

void ProxyObject::AddObserver(Observer& observer)
{
  // make sure an observer doesn't observe the same object twice
  // otherwise it will get multiple calls to OnSceneObjectAdd(), OnSceneObjectRemove() and ProxyDestroyed()
  DALI_ASSERT_DEBUG( mObservers.End() == std::find( mObservers.Begin(), mObservers.End(), &observer));

  mObservers.PushBack( &observer );
}

void ProxyObject::RemoveObserver(Observer& observer)
{
  // Find the observer...
  const ConstObserverIter endIter =  mObservers.End();
  for( ObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if( (*iter) == &observer)
    {
      mObservers.Erase( iter );
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mObservers.End());
}

void ProxyObject::OnSceneObjectAdd()
{
  // Notification for this object's constraints
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentSceneObjectAdded();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(),  endIter =  mObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->SceneObjectAdded(*this);
  }

  // enable property notifications in scene graph
  EnablePropertyNotifications();
}

void ProxyObject::OnSceneObjectRemove()
{
  // Notification for this object's constraints
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentSceneObjectRemoved();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(), endIter = mObservers.End(); iter != endIter; ++iter )
  {
    (*iter)->SceneObjectRemoved(*this);
  }

  // disable property notifications in scene graph
  DisablePropertyNotifications();
}

int ProxyObject::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool ProxyObject::Supports( Capability capability ) const
{
  return (capability & SUPPORTED_CAPABILITIES);
}

unsigned int ProxyObject::GetPropertyCount() const
{
  unsigned int count = GetDefaultPropertyCount();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Default Properties: %d\n", count );

  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    unsigned int manual( typeInfo->GetPropertyCount() );
    count += manual;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Manual Properties:  %d\n", manual );
  }

  unsigned int custom( mCustomProperties.Count() );
  count += custom;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Custom Properties:  %d\n", custom );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Total Properties:   %d\n", count );

  return count;
}

std::string ProxyObject::GetPropertyName( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return GetDefaultPropertyName( index );
  }

  if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyName( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Property index is invalid" );
    }
  }

  CustomProperty* custom = FindCustomProperty( index );
  if( custom )
  {
    return custom->name;
  }
  return "";
}

Property::Index ProxyObject::GetPropertyIndex(const std::string& name) const
{
  Property::Index index = GetDefaultPropertyIndex( name );

  if ( index == Property::INVALID_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      index = typeInfo->GetPropertyIndex( name );
    }
  }

  if( ( index == Property::INVALID_INDEX )&&( mCustomProperties.Count() > 0 ) )
  {
    Property::Index count = PROPERTY_CUSTOM_START_INDEX;
    const CustomPropertyLookup::ConstIterator end = mCustomProperties.End();
    for( CustomPropertyLookup::ConstIterator iter = mCustomProperties.Begin(); iter != end; ++iter, ++count )
    {
      CustomProperty* custom = *iter;
      if ( custom->name == name )
      {
        index = count;
        break;
      }
    }
  }

  return index;
}

bool ProxyObject::IsPropertyWritable( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return IsDefaultPropertyWritable( index );
  }

  if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->IsPropertyWritable( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Invalid property index" );
    }
  }

  CustomProperty* custom = FindCustomProperty( index );
  if( custom )
  {
    // Check that the index is valid
    return custom->IsWritable();
  }
  return false;
}

bool ProxyObject::IsPropertyAnimatable( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return IsDefaultPropertyAnimatable( index );
  }

  if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties are not animatable.
    return false;
  }

  CustomProperty* custom = FindCustomProperty( index );
  if( custom )
  {
    return custom->IsAnimatable();
  }
  return false;
}

bool ProxyObject::IsPropertyAConstraintInput( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return IsDefaultPropertyAConstraintInput( index );
  }

  if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties cannot be used as an input to a constraint.
    return false;
  }

  CustomProperty* custom = FindCustomProperty( index );
  if( custom )
  {
    // ... custom properties can be used as input to a constraint.
    return true;
  }
  return false;
}

Property::Type ProxyObject::GetPropertyType( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return GetDefaultPropertyType( index );
  }

  if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyType( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" );
    }
  }

  CustomProperty* custom = FindCustomProperty( index );
  if( custom )
  {
    return custom->type;
  }
  return Property::NONE;
}

void ProxyObject::SetProperty( Property::Index index, const Property::Value& propertyValue )
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    SetDefaultProperty( index, propertyValue );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      typeInfo->SetProperty( this, index, propertyValue );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" );
    }
  }
  else
  {
    CustomProperty* custom = FindCustomProperty( index );
    DALI_ASSERT_ALWAYS( custom && "Invalid property index" );
    if( custom->GetSceneGraphProperty() )
    {
      // set the scene graph property value
      SetSceneGraphProperty( index, *custom, propertyValue );
    }
    else if( custom->IsWritable() )
    {
      custom->value = propertyValue;
    }
    // trying to set value on read only property is no-op
  }
}

Property::Value ProxyObject::GetProperty(Property::Index index) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index is out of bounds" );

  Property::Value value;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    value = GetDefaultProperty( index );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      value = typeInfo->GetProperty( this, index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" );
    }
  }
  else if( mCustomProperties.Count() > 0 )
  {
    CustomProperty* custom = FindCustomProperty( index );
    DALI_ASSERT_ALWAYS( custom && "Invalid property index" );

    if( !custom->IsAnimatable() )
    {
      value = custom->value;
    }
    else
    {
      BufferIndex bufferIndex( Stage::GetCurrent()->GetEventBufferIndex() );

      switch ( custom->type )
      {
        case Property::BOOLEAN:
        {
          const AnimatableProperty<bool>* property = dynamic_cast< const AnimatableProperty<bool>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::FLOAT:
        {
          const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::INTEGER:
        {
          const AnimatableProperty<int>* property = dynamic_cast< const AnimatableProperty<int>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::VECTOR2:
        {
          const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::VECTOR3:
        {
          const AnimatableProperty<Vector3>* property = dynamic_cast< const AnimatableProperty<Vector3>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::VECTOR4:
        {
          const AnimatableProperty<Vector4>* property = dynamic_cast< const AnimatableProperty<Vector4>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::MATRIX:
        {
          const AnimatableProperty<Matrix>* property = dynamic_cast< const AnimatableProperty<Matrix>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::MATRIX3:
        {
          const AnimatableProperty<Matrix3>* property = dynamic_cast< const AnimatableProperty<Matrix3>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        case Property::ROTATION:
        {
          const AnimatableProperty<Quaternion>* property = dynamic_cast< const AnimatableProperty<Quaternion>* >( custom->GetSceneGraphProperty() );
          DALI_ASSERT_DEBUG( NULL != property );

          value = (*property)[ bufferIndex ];
          break;
        }

        default:
        {
          DALI_ASSERT_ALWAYS( false && "PropertyType enumeration is out of bounds" );
          break;
        }
      } // switch(type)
    } // if animatable

  } // if custom

  return value;
}

void ProxyObject::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.clear();

  // Default Properties
  GetDefaultPropertyIndices( indices );

  // Manual Properties
  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    typeInfo->GetPropertyIndices( indices );
  }

  // Custom Properties
  if ( mCustomProperties.Count() > 0 )
  {
    indices.reserve( indices.size() + mCustomProperties.Count() );

    CustomPropertyLookup::ConstIterator iter = mCustomProperties.Begin();
    const CustomPropertyLookup::ConstIterator endIter = mCustomProperties.End();
    int i=0;
    for ( ; iter != endIter; ++iter, ++i )
    {
      indices.push_back( PROPERTY_CUSTOM_START_INDEX + i );
    }
  }
}

Property::Index ProxyObject::RegisterProperty( const std::string& name, const Property::Value& propertyValue)
{
  // Create a new property
  Dali::Internal::OwnerPointer<PropertyBase> newProperty;

  switch ( propertyValue.GetType() )
  {
    case Property::BOOLEAN:
    {
      newProperty = new AnimatableProperty<bool>( propertyValue.Get<bool>() );
      break;
    }

    case Property::FLOAT:
    {
      newProperty = new AnimatableProperty<float>( propertyValue.Get<float>() );
      break;
    }

    case Property::INTEGER:
    {
      newProperty = new AnimatableProperty<int>( propertyValue.Get<int>() );
      break;
    }

    case Property::VECTOR2:
    {
      newProperty = new AnimatableProperty<Vector2>( propertyValue.Get<Vector2>() );
      break;
    }

    case Property::VECTOR3:
    {
      newProperty = new AnimatableProperty<Vector3>( propertyValue.Get<Vector3>() );
      break;
    }

    case Property::VECTOR4:
    {
      newProperty = new AnimatableProperty<Vector4>( propertyValue.Get<Vector4>() );
      break;
    }

    case Property::MATRIX:
    {
      newProperty = new AnimatableProperty<Matrix>( propertyValue.Get<Matrix>() );
      break;
    }

    case Property::MATRIX3:
    {
      newProperty = new AnimatableProperty<Matrix3>( propertyValue.Get<Matrix3>() );
      break;
    }

    case Property::ROTATION:
    {
      newProperty = new AnimatableProperty<Quaternion>( propertyValue.Get<Quaternion>() );
      break;
    }

    case Property::UNSIGNED_INTEGER:
    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      DALI_LOG_WARNING( "Property Type %d\n", propertyValue.GetType() );
      DALI_ASSERT_ALWAYS( false && "PropertyType is not animatable" );
      break;
    }

    default:
    {
      DALI_LOG_WARNING( "Property Type %d\n", propertyValue.GetType() );
      DALI_ASSERT_ALWAYS( false && "PropertyType enumeration is out of bounds" );
      break;
    }
  }

  const Property::Index index = PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count();
  mCustomProperties.PushBack( new CustomProperty( name, propertyValue.GetType(), newProperty.Get() ) );

  // The derived class now passes ownership of this new property to a scene-object
  // TODO: change this so that OwnerPointer is passed all the way as owership passing cannot be done with a reference
  InstallSceneObjectProperty( *(newProperty.Release()), name, index );

  return index;
}

Property::Index ProxyObject::RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode)
{
  Property::Index index = Property::INVALID_INDEX;

  if(Property::ANIMATABLE == accessMode)
  {
    index = RegisterProperty(name, propertyValue);
  }
  else
  {
    // Add entry to the property lookup
    index = PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count();
    mCustomProperties.PushBack( new CustomProperty( name, propertyValue, accessMode ) );
  }

  return index;
}

Dali::PropertyNotification ProxyObject::AddPropertyNotification(Property::Index index,
                                                                int componentIndex,
                                                                const Dali::PropertyCondition& condition)
{
  if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    if ( index <= PROPERTY_REGISTRATION_MAX_INDEX )
    {
      DALI_ASSERT_ALWAYS( false && "Property notification added to event side only property." );
    }
    else if ( mCustomProperties.Count() > 0 )
    {
      CustomProperty* custom = FindCustomProperty( index );
      DALI_ASSERT_ALWAYS( custom && "Invalid property index" );
      DALI_ASSERT_ALWAYS( custom->IsAnimatable() && "Property notification added to event side only property." );
    }
  }

  Dali::Handle self(this);
  Property target( self, index );

  PropertyNotificationPtr internal = PropertyNotification::New( target, componentIndex, condition );
  Dali::PropertyNotification propertyNotification(internal.Get());

  if( !mPropertyNotifications )
  {
    mPropertyNotifications = new PropertyNotificationContainer;
  }
  mPropertyNotifications->push_back(propertyNotification);

  return propertyNotification;
}

void ProxyObject::RemovePropertyNotification(Dali::PropertyNotification propertyNotification)
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    while(iter != mPropertyNotifications->end() )
    {
      if(*iter == propertyNotification)
      {
        mPropertyNotifications->erase(iter);
        // As we can't ensure all references are removed, we can just disable
        // the notification.
        GetImplementation(propertyNotification).Disable();
        return;
      }
      ++iter;
    }
  }
}

void ProxyObject::RemovePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    while(iter != mPropertyNotifications->end() )
    {
      // As we can't ensure all references are removed, we can just disable
      // the notification.
      GetImplementation(*iter).Disable();
      ++iter;
    }

    mPropertyNotifications->clear();
  }
}

void ProxyObject::EnablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    PropertyNotificationContainerIter endIter = mPropertyNotifications->end();

    for( ; iter != endIter; ++iter )
    {
      GetImplementation(*iter).Enable();
    }
  }
}

void ProxyObject::DisablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    PropertyNotificationContainerIter endIter = mPropertyNotifications->end();

    for( ; iter != endIter; ++iter )
    {
      GetImplementation(*iter).Disable();
    }
  }
}

Dali::ActiveConstraint ProxyObject::ApplyConstraint( Constraint& constraint )
{
  return Dali::ActiveConstraint( DoApplyConstraint( constraint, Dali::Constrainable() ) );
}

Dali::ActiveConstraint ProxyObject::ApplyConstraint( Constraint& constraint, Dali::Constrainable weightObject )
{
  return Dali::ActiveConstraint( DoApplyConstraint( constraint, weightObject ) );
}

ActiveConstraintBase* ProxyObject::DoApplyConstraint( Constraint& constraint, Dali::Constrainable weightObject )
{
  ActiveConstraintBase* activeConstraintImpl = constraint.CreateActiveConstraint();
  DALI_ASSERT_DEBUG( NULL != activeConstraintImpl );

  Dali::ActiveConstraint activeConstraint( activeConstraintImpl );

  if( weightObject )
  {
    ProxyObject& weightObjectImpl = GetImplementation( weightObject );
    Property::Index weightIndex = weightObjectImpl.GetPropertyIndex( "weight" );

    if( Property::INVALID_INDEX != weightIndex )
    {
      activeConstraintImpl->SetCustomWeightObject( weightObjectImpl, weightIndex );
    }
  }

  if( !mConstraints )
  {
    mConstraints = new ActiveConstraintContainer;
  }
  mConstraints->push_back( activeConstraint );

  activeConstraintImpl->FirstApply( *this, constraint.GetApplyTime() );

  return activeConstraintImpl;
}

void ProxyObject::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  if( entry.IsAnimatable() )
  {
    switch ( entry.type )
    {
      case Property::BOOLEAN:
      {
        const AnimatableProperty<bool>* property = dynamic_cast< const AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<bool>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<bool>() );
        break;
      }

      case Property::FLOAT:
      {
        const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<float>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<float>() );
        break;
      }

      case Property::INTEGER:
      {
        const AnimatableProperty<int>* property = dynamic_cast< const AnimatableProperty<int>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<int>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<int>() );
        break;
      }

      case Property::VECTOR2:
      {
        const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Vector2>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector2>() );
        break;
      }

      case Property::VECTOR3:
      {
        const AnimatableProperty<Vector3>* property = dynamic_cast< const AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Vector3>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector3>() );
        break;
      }

      case Property::VECTOR4:
      {
        const AnimatableProperty<Vector4>* property = dynamic_cast< const AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Vector4>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Vector4>() );
        break;
      }

      case Property::ROTATION:
      {
        const AnimatableProperty<Quaternion>* property = dynamic_cast< const AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Quaternion>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Quaternion>() );
        break;
      }

      case Property::MATRIX:
      {
        const AnimatableProperty<Matrix>* property = dynamic_cast< const AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Matrix>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Matrix>() );
        break;
      }

      case Property::MATRIX3:
      {
        const AnimatableProperty<Matrix3>* property = dynamic_cast< const AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        // property is being used in a separate thread; queue a message to set the property
        BakeMessage<Matrix3>( Stage::GetCurrent()->GetUpdateInterface(), *property, value.Get<Matrix3>() );
        break;
      }

      default:
      {
        // non-animatable scene graph property, do nothing
      }
    }
  }
}

const TypeInfo* ProxyObject::GetTypeInfo() const
{
  if ( !mTypeInfo )
  {
    // This uses a dynamic_cast so can be quite expensive so we only really want to do it once
    // especially as the type-info does not change during the life-time of an application

    Dali::TypeInfo typeInfoHandle = TypeRegistry::Get()->GetTypeInfo( this );
    if ( typeInfoHandle )
    {
      mTypeInfo = &GetImplementation( typeInfoHandle );
    }
  }

  return mTypeInfo;
}

void ProxyObject::RemoveConstraint( ActiveConstraint& constraint, bool isInScenegraph )
{
  // guard against constraint sending messages during core destruction
  if ( Stage::IsInstalled() )
  {
    if( isInScenegraph )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( constraint );
      baseConstraint.BeginRemove();
    }
  }
}

void ProxyObject::RemoveConstraint( Dali::ActiveConstraint activeConstraint )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    bool isInSceneGraph( NULL != GetSceneObject() );

    ActiveConstraintIter it( std::find( mConstraints->begin(), mConstraints->end(), activeConstraint ) );
    if( it !=  mConstraints->end() )
    {
      RemoveConstraint( *it, isInSceneGraph );
      mConstraints->erase( it );
    }
  }
}

void ProxyObject::RemoveConstraints( unsigned int tag )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    bool isInSceneGraph( NULL != GetSceneObject() );

    ActiveConstraintIter iter( mConstraints->begin() );
    while(iter != mConstraints->end() )
    {
      ActiveConstraintBase& constraint = GetImplementation( *iter );
      if( constraint.GetTag() == tag )
      {
        RemoveConstraint( *iter, isInSceneGraph );
        iter = mConstraints->erase( iter );
      }
      else
      {
        ++iter;
      }
    }
  }
}

void ProxyObject::RemoveConstraints()
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    // If we have nothing in the scene-graph, just clear constraint containers
    const SceneGraph::PropertyOwner* propertyOwner = GetSceneObject();
    if ( NULL != propertyOwner )
    {
      const ActiveConstraintConstIter endIter = mConstraints->end();
      for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
      {
        RemoveConstraint( *iter, true );
      }
    }

    delete mConstraints;
    mConstraints = NULL;
  }
}

void ProxyObject::SetTypeInfo( const TypeInfo* typeInfo )
{
  mTypeInfo = typeInfo;
}

ProxyObject::~ProxyObject()
{
  // Notification for this object's constraints
  // (note that the ActiveConstraint handles may outlive the ProxyObject)
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentDestroyed();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(), endIter =  mObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->ProxyDestroyed(*this);
  }

  delete mConstraints;
  delete mPropertyNotifications;
}

CustomProperty* ProxyObject::FindCustomProperty( Property::Index index ) const
{
  CustomProperty* property( NULL );
  int arrayIndex = index - PROPERTY_CUSTOM_START_INDEX;
  if( arrayIndex >= 0 )
  {
    if( arrayIndex < (int)mCustomProperties.Count() ) // we can only access the first 2 billion custom properties
    {
      property = mCustomProperties[ arrayIndex ];
    }
  }
  return property;
}

} // namespace Internal

} // namespace Dali
