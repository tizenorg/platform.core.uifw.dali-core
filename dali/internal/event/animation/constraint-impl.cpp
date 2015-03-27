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
#include <dali/internal/event/animation/constraint-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/active-constraint-impl.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/event/animation/property-constraint.h>
#include <dali/internal/event/animation/property-input-accessor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

template <class P>
PropertyConstraint<P>* CreatePropertyConstraint( CallbackBase* func )
{
  return new PropertyConstraint<P>( reinterpret_cast< Dali::Constraint::Function< P >* >( func ) );
}

} // unnamed namespace

Constraint::Constraint( Property::Index targetIndex,
                        Property::Type targetType,
                        CallbackBase* func )
: mActiveConstraintTemplate(),
  mTargetIndex( targetIndex ),
  mTargetType( targetType ),
  mSources(),
  mFunc( func ),
  mRemoveAction( Dali::Constraint::DEFAULT_REMOVE_ACTION ),
  mTag( 0 )
{
}

void Constraint::AddSource( Source source )
{
  mActiveConstraintTemplate.Reset();
  mSources.push_back( source );
}

ActiveConstraintBase* Constraint::CreateActiveConstraint()
{
  if ( !mActiveConstraintTemplate )
  {
    switch ( mTargetType )
    {
      case Property::BOOLEAN:
      {
        PropertyConstraintPtr<bool>::Type funcPtr( CreatePropertyConstraint<bool>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<bool>::New( mTargetIndex,
                                                                                         mSources,
                                                                                         funcPtr ) );
        break;
      }

      case Property::FLOAT:
      {
        PropertyConstraintPtr<float>::Type funcPtr( CreatePropertyConstraint<float>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<float>::New( mTargetIndex,
                                                                                          mSources,
                                                                                          funcPtr ) );
        break;
      }

      case Property::INTEGER:
      {
        PropertyConstraintPtr<int>::Type funcPtr( CreatePropertyConstraint<int>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<int>::New( mTargetIndex,
                                                                                        mSources,
                                                                                        funcPtr ) );
        break;
      }

      case Property::VECTOR2:
      {
        PropertyConstraintPtr<Vector2>::Type funcPtr( CreatePropertyConstraint<Vector2>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector2>::New( mTargetIndex,
                                                                                            mSources,
                                                                                            funcPtr ) );
        break;
      }

      case Property::VECTOR3:
      {
        PropertyConstraintPtr<Vector3>::Type funcPtr( CreatePropertyConstraint<Vector3>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector3>::New( mTargetIndex,
                                                                                            mSources,
                                                                                            funcPtr ) );
        break;
      }

      case Property::VECTOR4:
      {
        PropertyConstraintPtr<Vector4>::Type funcPtr( CreatePropertyConstraint<Vector4>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector4>::New( mTargetIndex,
                                                                                            mSources,
                                                                                            funcPtr ) );
        break;
      }

      case Property::ROTATION:
      {
        PropertyConstraintPtr<Quaternion>::Type funcPtr( CreatePropertyConstraint<Quaternion>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Quaternion>::New( mTargetIndex,
                                                                                               mSources,
                                                                                               funcPtr ) );
        break;
      }

      case Property::MATRIX:
      {
        PropertyConstraintPtr<Matrix>::Type funcPtr( CreatePropertyConstraint<Matrix>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Matrix>::New( mTargetIndex,
                                                                                           mSources,
                                                                                           funcPtr ) );
        break;
      }

      case Property::MATRIX3:
      {
        PropertyConstraintPtr<Matrix3>::Type funcPtr( CreatePropertyConstraint<Matrix3>( mFunc ) );

        mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Matrix3>::New( mTargetIndex,
                                                                                            mSources,
                                                                                            funcPtr ) );
        break;
      }

      default:
      {
        DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should never come here
        break;
      }
    }

    ActiveConstraintBase& activeConstraintImpl = GetImplementation( mActiveConstraintTemplate );
    activeConstraintImpl.SetRemoveAction( mRemoveAction );
    activeConstraintImpl.SetTag( mTag );
  }

  return GetImplementation( mActiveConstraintTemplate ).Clone();
}

void Constraint::SetRemoveAction( Dali::Constraint::RemoveAction action )
{
  mRemoveAction = action;

  if ( mActiveConstraintTemplate )
  {
    GetImplementation( mActiveConstraintTemplate ).SetRemoveAction( action );
  }
}

Dali::Constraint::RemoveAction Constraint::GetRemoveAction() const
{
  return mRemoveAction;
}

void Constraint::SetTag(unsigned int tag)
{
  mTag = tag;

  if ( mActiveConstraintTemplate )
  {
    GetImplementation( mActiveConstraintTemplate ).SetTag(tag);
  }
}

unsigned int Constraint::GetTag() const
{
  return mTag;
}

Constraint::~Constraint()
{
  delete mFunc;
}

} // namespace Internal

} // namespace Dali
