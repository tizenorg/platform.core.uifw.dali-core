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
#include <dali/internal/event/animation/active-constraint-base.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>

using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

// Properties

//              Name        Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "weight",    FLOAT,   true,     true,    true,   Dali::ActiveConstraint::Property::WEIGHT )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

// Signals

const char* const SIGNAL_APPLIED = "applied";

BaseHandle Create()
{
  // not directly creatable
  return BaseHandle();
}

TypeRegistration mType( typeid(Dali::ActiveConstraint), typeid(Dali::Handle), Create );

SignalConnectorType signalConnector1( mType, SIGNAL_APPLIED, &ActiveConstraintBase::DoConnectSignal );

} // unnamed namespace

ActiveConstraintBase::ActiveConstraintBase( EventToUpdate& eventToUpdate, Property::Index targetPropertyIndex, SourceContainer& sources, unsigned int sourceCount )
: mEventToUpdate( eventToUpdate ),
  mTargetPropertyIndex( targetPropertyIndex ),
  mSources( sources ),
  mSourceCount( sourceCount ),
  mTargetObject( NULL ),
  mObservedObjects(),
  mSceneGraphConstraint( NULL ),
  mCustomWeight( NULL ),
  mOffstageWeight( Dali::ActiveConstraint::DEFAULT_WEIGHT ),
  mAlphaFunction( Dali::Constraint::DEFAULT_ALPHA_FUNCTION ),
  mRemoveAction( Dali::Constraint::DEFAULT_REMOVE_ACTION ),
  mTag(0),
  mApplyAnimation()
{
  // Skip init when any of the objects have been destroyed
  if ( mSources.size() != mSourceCount )
  {
    // Discard all object pointers
    mTargetObject = NULL;
    mSources.clear();
  }

  // Observe the objects providing properties
  for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
  {
    if ( OBJECT_PROPERTY == iter->sourceType )
    {
      DALI_ASSERT_ALWAYS( NULL != iter->object && "ActiveConstraint source object not found" );

      ObserveObject( *(iter->object) );
    }
  }
}

ActiveConstraintBase::~ActiveConstraintBase()
{
  StopObservation();

  // Disconnect from internal animation signals
  if ( mApplyAnimation )
  {
    GetImplementation(mApplyAnimation).SetFinishedCallback( NULL, NULL );
  }
}

void ActiveConstraintBase::SetCustomWeightObject( Object& weightObject, Property::Index weightIndex )
{
  const SceneGraph::PropertyBase* base = weightObject.GetSceneObjectAnimatableProperty( weightIndex );
  const SceneGraph::AnimatableProperty<float>* sceneProperty = dynamic_cast< const SceneGraph::AnimatableProperty<float>* >( base );

  if( sceneProperty )
  {
    mCustomWeight = sceneProperty;

    ObserveObject( weightObject );
  }
}

void ActiveConstraintBase::FirstApply( Object& parent, TimePeriod applyTime )
{
  DALI_ASSERT_ALWAYS( NULL == mTargetObject && "Parent of ActiveConstraint already set" );

  // No need to do anything, if the source objects are gone
  if( mSources.size() == mSourceCount )
  {
    mTargetObject = &parent;

    ConnectConstraint();
  }

  if ( applyTime.durationSeconds > 0.0f )
  {
    DALI_ASSERT_DEBUG( !mApplyAnimation );

    // Set start weight
    SetWeight( 0.0f );

    // Automatically animate (increase) the weight, until the constraint is fully applied
    mApplyAnimation = Dali::Animation::New( applyTime.delaySeconds + applyTime.durationSeconds );
    Dali::ActiveConstraint self( this );
    mApplyAnimation.AnimateTo( Property( self, Dali::ActiveConstraint::Property::WEIGHT ), Dali::ActiveConstraint::FINAL_WEIGHT, mAlphaFunction, applyTime );
    mApplyAnimation.Play();

    // Chain "Finish" to "Applied" signal
    GetImplementation(mApplyAnimation).SetFinishedCallback( &ActiveConstraintBase::FirstApplyFinished, this );
  }
}

void ActiveConstraintBase::OnParentDestroyed()
{
  // Stop observing the remaining objects
  StopObservation();

  // Discard all object pointers
  mTargetObject = NULL;
  mSources.clear();
}

void ActiveConstraintBase::OnParentSceneObjectAdded()
{
  if ( NULL == mSceneGraphConstraint &&
       mTargetObject )
  {
    ConnectConstraint();
  }
}

void ActiveConstraintBase::OnParentSceneObjectRemoved()
{
  if ( mSceneGraphConstraint )
  {
    // Notify base class that the scene-graph constraint is being removed
    OnSceneObjectRemove();

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = NULL;
  }
}

void ActiveConstraintBase::BeginRemove()
{
  // Stop observing the remaining objects
  StopObservation();

  // Discard all object pointers
  mSources.clear();

  const SceneGraph::PropertyOwner* propertyOwner = mTargetObject ? mTargetObject->GetSceneObject() : NULL;

  if ( propertyOwner &&
       mSceneGraphConstraint )
  {
    // Notify base class that the scene-graph constraint is being removed
    OnSceneObjectRemove();

    // Remove from scene-graph
    RemoveConstraintMessage( mEventToUpdate, *propertyOwner, *(mSceneGraphConstraint) );

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = NULL;
  }
}

Object* ActiveConstraintBase::GetParent()
{
  return mTargetObject;
}

bool ActiveConstraintBase::Supports( Capability capability ) const
{
  return false; // switch-off support for dynamic properties
}

Dali::Handle ActiveConstraintBase::GetTargetObject()
{
  return Dali::Handle( mTargetObject );
}

Property::Index ActiveConstraintBase::GetTargetProperty()
{
  return mTargetPropertyIndex;
}

void ActiveConstraintBase::SetWeight( float weight )
{
  if ( mSceneGraphConstraint )
  {
    BakeWeightMessage( mEventToUpdate, *mSceneGraphConstraint, weight );
  }
  else
  {
    mOffstageWeight = weight;
  }
}

float ActiveConstraintBase::GetCurrentWeight() const
{
  float currentWeight( mOffstageWeight );

  if ( mSceneGraphConstraint )
  {
    currentWeight = mSceneGraphConstraint->GetWeight( mEventToUpdate.GetEventBufferIndex() );
  }

  return currentWeight;
}

ActiveConstraintSignalType& ActiveConstraintBase::AppliedSignal()
{
  return mAppliedSignal;
}

bool ActiveConstraintBase::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  ActiveConstraintBase* constraint = dynamic_cast<ActiveConstraintBase*>(object);

  if ( 0 == strcmp( signalName.c_str(), SIGNAL_APPLIED ) )
  {
    constraint->AppliedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ActiveConstraintBase::SetAlphaFunction( AlphaFunction alphaFunc )
{
  mAlphaFunction = alphaFunc;
}

AlphaFunction ActiveConstraintBase::GetAlphaFunction() const
{
  return mAlphaFunction;
}

void ActiveConstraintBase::SetRemoveAction( ActiveConstraintBase::RemoveAction action )
{
  mRemoveAction = action;
}

ActiveConstraintBase::RemoveAction ActiveConstraintBase::GetRemoveAction() const
{
  return mRemoveAction;
}

void ActiveConstraintBase::SetTag(const unsigned int tag)
{
  mTag = tag;
}

unsigned int ActiveConstraintBase::GetTag() const
{
  return mTag;
}

unsigned int ActiveConstraintBase::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ActiveConstraintBase::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* ActiveConstraintBase::GetDefaultPropertyName( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }
  else
  {
    return NULL;
  }
}

Property::Index ActiveConstraintBase::GetDefaultPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Only one name to compare with...
  if( 0 == strcmp( name.c_str(), DEFAULT_PROPERTY_DETAILS[0].name ) ) // Don't want to convert rhs to string
  {
    index = 0;
  }

  return index;
}

bool ActiveConstraintBase::IsDefaultPropertyWritable( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].writable;
}

bool ActiveConstraintBase::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
}

bool ActiveConstraintBase::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
}

Property::Type ActiveConstraintBase::GetDefaultPropertyType( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // Index out-of-range
  return Property::NONE;
}

void ActiveConstraintBase::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( Dali::ActiveConstraint::Property::WEIGHT == index )
  {
    SetWeight( propertyValue.Get<float>() );
  }
}

Property::Value ActiveConstraintBase::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  if( Dali::ActiveConstraint::Property::WEIGHT == index )
  {
    value = GetCurrentWeight();
  }

  return value;
}

const SceneGraph::PropertyOwner* ActiveConstraintBase::GetSceneObject() const
{
  return mSceneGraphConstraint;
}

const SceneGraph::PropertyBase* ActiveConstraintBase::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_DEBUG( 0 == index ); // only 1 property supported

  // This method should only return a property which is part of the scene-graph
  if ( !mSceneGraphConstraint )
  {
    return NULL;
  }

  return &mSceneGraphConstraint->mWeight;
}

const PropertyInputImpl* ActiveConstraintBase::GetSceneObjectInputProperty( Property::Index index ) const
{
  DALI_ASSERT_DEBUG( 0 == index ); // only 1 property supported

  // This method should only return a property which is part of the scene-graph
  if ( !mSceneGraphConstraint )
  {
    return NULL;
  }

  return &mSceneGraphConstraint->mWeight;
}

void ActiveConstraintBase::SceneObjectAdded( Object& object )
{
  // Should not be getting callbacks when mSources has been cleared
   DALI_ASSERT_DEBUG( mSources.size() == mSourceCount );

  if ( NULL == mSceneGraphConstraint &&
       mTargetObject )
  {
    ConnectConstraint();
  }
}

void ActiveConstraintBase::SceneObjectRemoved( Object& object )
{
  // Notify base class that the scene-graph constraint is being removed
  OnSceneObjectRemove();

  if ( mSceneGraphConstraint )
  {
    // Preserve the previous weight
    mOffstageWeight = mSceneGraphConstraint->GetWeight( mEventToUpdate.GetEventBufferIndex() );

    const SceneGraph::PropertyOwner* propertyOwner = mTargetObject ? mTargetObject->GetSceneObject() : NULL;

    if( propertyOwner )
    {
      // Remove from scene-graph
      RemoveConstraintMessage( mEventToUpdate, *propertyOwner, *(mSceneGraphConstraint) );
    }

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = NULL;
  }
}

void ActiveConstraintBase::ObjectDestroyed( Object& object )
{
  // Remove object pointer from observation set
  ObjectIter iter = std::find( mObservedObjects.Begin(), mObservedObjects.End(), &object );
  DALI_ASSERT_DEBUG( mObservedObjects.End() != iter );
  mObservedObjects.Erase( iter );

  // Stop observing the remaining objects
  StopObservation();

  // Discard all object & scene-graph pointers
  mSceneGraphConstraint = NULL;
  mTargetObject = NULL;
  mSources.clear();
}

void ActiveConstraintBase::ObserveObject( Object& object )
{
  ObjectIter iter = std::find( mObservedObjects.Begin(), mObservedObjects.End(), &object );
  if ( mObservedObjects.End() == iter )
  {
    object.AddObserver( *this );
    mObservedObjects.PushBack( &object );
  }
}

void ActiveConstraintBase::StopObservation()
{
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    (*iter)->RemoveObserver( *this );
  }

  mObservedObjects.Clear();
}

void ActiveConstraintBase::FirstApplyFinished( Object* object )
{
  // trust the object is correct as its set in FirstApply (in this same file)
  ActiveConstraintBase* self = static_cast<ActiveConstraintBase*>( object );

  // This is necessary when the constraint was not added to scene-graph during the animation
  self->SetWeight( Dali::ActiveConstraint::FINAL_WEIGHT );

  // The animation is no longer needed
  GetImplementation(self->mApplyAnimation).SetFinishedCallback( NULL, NULL );
  self->mApplyAnimation.Reset();

  // Chain "Finish" to "Applied" signal

  if ( !self->mAppliedSignal.Empty() )
  {
    Dali::ActiveConstraint handle( self );
    self->mAppliedSignal.Emit( handle );
  }

  // WARNING - this constraint may now have been deleted; don't do anything else here
}

} // namespace Internal

} // namespace Dali
