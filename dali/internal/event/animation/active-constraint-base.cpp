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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>

using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{

namespace Internal
{

ActiveConstraintBase::ActiveConstraintBase( Property::Index targetPropertyIndex, SourceContainer& sources )
: mTargetPropertyIndex( targetPropertyIndex ),
  mSources( sources ),
  mTargetObject( NULL ),
  mObservedObjects(),
  mSceneGraphConstraint( NULL ),
  mRemoveAction( Dali::Constraint::DEFAULT_REMOVE_ACTION ),
  mTag(0),
  mEventThreadServices( *Stage::GetCurrent() )
{
}

ActiveConstraintBase::~ActiveConstraintBase()
{
  StopObservation();
}

void ActiveConstraintBase::AddSource( Source source )
{
  mSources.push_back( source );
}

void ActiveConstraintBase::FirstApply( Object& parent )
{
  DALI_ASSERT_ALWAYS( NULL == mTargetObject && "Parent of ActiveConstraint already set" );

  // Observe the objects providing properties
  for ( SourceIter iter = mSources.begin(); mSources.end() != iter; ++iter )
  {
    if ( OBJECT_PROPERTY == iter->sourceType )
    {
      DALI_ASSERT_ALWAYS( NULL != iter->object && "ActiveConstraint source object not found" );

      ObserveObject( *(iter->object) );
    }
  }

  mTargetObject = &parent;

  ConnectConstraint();
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
    // Remove from scene-graph
    RemoveConstraintMessage( GetEventThreadServices(), *propertyOwner, *(mSceneGraphConstraint) );

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = NULL;
  }
}

Object* ActiveConstraintBase::GetParent()
{
  return mTargetObject;
}

Dali::Handle ActiveConstraintBase::GetTargetObject()
{
  return Dali::Handle( mTargetObject );
}

Property::Index ActiveConstraintBase::GetTargetProperty()
{
  return mTargetPropertyIndex;
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

void ActiveConstraintBase::SceneObjectAdded( Object& object )
{
  if ( NULL == mSceneGraphConstraint &&
       mTargetObject )
  {
    ConnectConstraint();
  }
}

void ActiveConstraintBase::SceneObjectRemoved( Object& object )
{
  if ( mSceneGraphConstraint )
  {
    const SceneGraph::PropertyOwner* propertyOwner = mTargetObject ? mTargetObject->GetSceneObject() : NULL;

    if( propertyOwner )
    {
      // Remove from scene-graph
      RemoveConstraintMessage( GetEventThreadServices(), *propertyOwner, *(mSceneGraphConstraint) );
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

} // namespace Internal

} // namespace Dali
