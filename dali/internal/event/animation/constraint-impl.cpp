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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

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
PropertyConstraintBase<P>* CreatePropertyConstraint( Constraint::AnyFunction& func, unsigned int sourceCount )
{
  PropertyConstraintBase<P>* propertyConstraint( NULL );

  switch ( sourceCount )
  {
    case 0u:
    {
      propertyConstraint = new PropertyConstraint0<P>( AnyCast<boost::function<P (const P&)> >( func ) );
      break;
    }

    case 1u:
    {
      propertyConstraint = new PropertyConstraint1<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }

    case 2u:
    {
      propertyConstraint = new PropertyConstraint2<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }

    case 3u:
    {
      propertyConstraint = new PropertyConstraint3<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }

    case 4u:
    {
      propertyConstraint = new PropertyConstraint4<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }

    case 5u:
    {
      propertyConstraint = new PropertyConstraint5<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }

    case 6u:
    {
      propertyConstraint = new PropertyConstraint6<P, PropertyInputAccessor>( AnyCast< boost::function<P (const P&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&,
                                                                                                                  const PropertyInput&)> >( func ) );
      break;
    }


    default:
    {
      // should never come here
      DALI_ASSERT_ALWAYS( false && "Cannot have more than 6 property constraints" );
      break;
    }
  }

  return propertyConstraint;
}

} // unnamed namespace

Constraint::Constraint( Property::Index targetIndex,
                        Property::Type targetType,
                        SourceContainer& sources,
                        AnyFunction& func )
: mApplyTime( 0.0f )
{
  switch ( targetType )
  {
    case Property::BOOLEAN:
    {
      PropertyConstraintPtr<bool>::Type funcPtr( CreatePropertyConstraint<bool>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<bool>::New( targetIndex,
                                                                                       sources,
                                                                                       funcPtr ) );
      break;
    }

    case Property::INTEGER:
    {
      PropertyConstraintPtr<int>::Type funcPtr( CreatePropertyConstraint<int>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<int>::New( targetIndex,
                                                                                      sources,
                                                                                      funcPtr ) );
      break;
    }

    case Property::UNSIGNED_INTEGER:
    {
      PropertyConstraintPtr<unsigned int>::Type funcPtr( CreatePropertyConstraint<unsigned int>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<unsigned int>::New( targetIndex,
                                                                                               sources,
                                                                                               funcPtr ) );
      break;
    }

    case Property::FLOAT:
    {
      PropertyConstraintPtr<float>::Type funcPtr( CreatePropertyConstraint<float>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<float>::New( targetIndex,
                                                                                        sources,
                                                                                        funcPtr ) );
      break;
    }

    case Property::VECTOR2:
    {
      PropertyConstraintPtr<Vector2>::Type funcPtr( CreatePropertyConstraint<Vector2>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector2>::New( targetIndex,
                                                                                          sources,
                                                                                          funcPtr ) );
      break;
    }

    case Property::VECTOR3:
    {
      PropertyConstraintPtr<Vector3>::Type funcPtr( CreatePropertyConstraint<Vector3>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector3>::New( targetIndex,
                                                                                          sources,
                                                                                          funcPtr ) );
      break;
    }

    case Property::VECTOR4:
    {
      PropertyConstraintPtr<Vector4>::Type funcPtr( CreatePropertyConstraint<Vector4>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Vector4>::New( targetIndex,
                                                                                          sources,
                                                                                          funcPtr ) );
      break;
    }

    case Property::ROTATION:
    {
      PropertyConstraintPtr<Quaternion>::Type funcPtr( CreatePropertyConstraint<Quaternion>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Quaternion>::New( targetIndex,
                                                                                             sources,
                                                                                             funcPtr ) );
      break;
    }

    case Property::MATRIX:
    {
      PropertyConstraintPtr<Matrix>::Type funcPtr( CreatePropertyConstraint<Matrix>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Matrix>::New( targetIndex,
                                                                                         sources,
                                                                                         funcPtr ) );
      break;
    }

    case Property::MATRIX3:
    {
      PropertyConstraintPtr<Matrix3>::Type funcPtr( CreatePropertyConstraint<Matrix3>( func, sources.size() ) );

      mActiveConstraintTemplate = Dali::ActiveConstraint( ActiveConstraint<Matrix3>::New( targetIndex,
                                                                                          sources,
                                                                                          funcPtr ) );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "Property type enumeration out of bounds" ); // should never come here
      break;
    }
  }
}

ActiveConstraintBase* Constraint::CreateActiveConstraint()
{
  return GetImplementation( mActiveConstraintTemplate ).Clone();
}

void Constraint::SetApplyTime( TimePeriod timePeriod )
{
  mApplyTime = timePeriod;
}

TimePeriod Constraint::GetApplyTime() const
{
  return mApplyTime;
}

void Constraint::SetAlphaFunction( Dali::AlphaFunction func )
{
  GetImplementation( mActiveConstraintTemplate ).SetAlphaFunction( func );
}

Dali::AlphaFunction Constraint::GetAlphaFunction() const
{
  return GetImplementation( mActiveConstraintTemplate ).GetAlphaFunction();
}

void Constraint::SetRemoveAction( Dali::Constraint::RemoveAction action )
{
  GetImplementation( mActiveConstraintTemplate ).SetRemoveAction( action );
}

Dali::Constraint::RemoveAction Constraint::GetRemoveAction() const
{
  return GetImplementation( mActiveConstraintTemplate ).GetRemoveAction();
}


void Constraint::SetTag(unsigned int tag)
{
  GetImplementation( mActiveConstraintTemplate ).SetTag(tag);
}

unsigned int Constraint::GetTag() const
{
  return GetImplementation( mActiveConstraintTemplate ).GetTag();
}



Constraint::~Constraint()
{
}

} // namespace Internal

} // namespace Dali
