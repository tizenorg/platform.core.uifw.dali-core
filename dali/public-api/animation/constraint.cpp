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
#include <dali/public-api/animation/constraint.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/event/animation/property-constraint.h>

namespace Dali
{

namespace // unnamed namespace
{

template <class P>
Internal::PropertyConstraint<P>* CreatePropertyConstraint( CallbackBase* func )
{
  return new Internal::PropertyConstraint<P>( reinterpret_cast< Dali::Constraint::Function< P >* >( func ) );
}

} // unnamed namespace

const Constraint::RemoveAction Constraint::DEFAULT_REMOVE_ACTION  = Constraint::Bake;

Constraint::Constraint()
{
}

Constraint::Constraint(Internal::ConstraintBase* constraint)
: BaseHandle(constraint)
{
}

Constraint::~Constraint()
{
}

Constraint::Constraint(const Constraint& handle)
: BaseHandle(handle)
{
}

Constraint& Constraint::operator=(const Constraint& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Constraint::AddSource( ConstraintSource source )
{
  GetImplementation( *this ).AddSource( Internal::Source( source ) );
}

Handle Constraint::GetTargetObject()
{
  return GetImplementation(*this).GetTargetObject();
}

Property::Index Constraint::GetTargetProperty()
{
  return GetImplementation(*this).GetTargetProperty();
}

void Constraint::SetRemoveAction(Constraint::RemoveAction action)
{
  GetImplementation(*this).SetRemoveAction(action);
}

Constraint::RemoveAction Constraint::GetRemoveAction() const
{
  return GetImplementation(*this).GetRemoveAction();
}

void Constraint::SetTag( const unsigned int tag )
{
  GetImplementation(*this).SetTag( tag );
}

unsigned int Constraint::GetTag() const
{
  return GetImplementation(*this).GetTag();
}

Constraint Constraint::New( Property::Index targetIndex,
                            Property::Type targetType,
                            CallbackBase* func )
{
  Constraint constraint;
  Internal::SourceContainer sources;

  switch ( targetType )
  {
    case Property::BOOLEAN:
    {
      Internal::PropertyConstraintPtr<bool>::Type funcPtr( CreatePropertyConstraint<bool>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<bool>::New( targetIndex,
                                                            sources,
                                                            funcPtr ) );
      break;
    }

    case Property::FLOAT:
    {
      Internal::PropertyConstraintPtr<float>::Type funcPtr( CreatePropertyConstraint<float>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<float>::New( targetIndex,
                                                                         sources,
                                                                         funcPtr ) );
      break;
    }

    case Property::INTEGER:
    {
      Internal::PropertyConstraintPtr<int>::Type funcPtr( CreatePropertyConstraint<int>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<int>::New( targetIndex,
                                                                       sources,
                                                                       funcPtr ) );
      break;
    }

    case Property::VECTOR2:
    {
      Internal::PropertyConstraintPtr<Vector2>::Type funcPtr( CreatePropertyConstraint<Vector2>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Vector2>::New( targetIndex,
                                                                           sources,
                                                                           funcPtr ) );
      break;
    }

    case Property::VECTOR3:
    {
      Internal::PropertyConstraintPtr<Vector3>::Type funcPtr( CreatePropertyConstraint<Vector3>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Vector3>::New( targetIndex,
                                                                           sources,
                                                                           funcPtr ) );
      break;
    }

    case Property::VECTOR4:
    {
      Internal::PropertyConstraintPtr<Vector4>::Type funcPtr( CreatePropertyConstraint<Vector4>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Vector4>::New( targetIndex,
                                                                           sources,
                                                                           funcPtr ) );
      break;
    }

    case Property::ROTATION:
    {
      Internal::PropertyConstraintPtr<Quaternion>::Type funcPtr( CreatePropertyConstraint<Quaternion>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Quaternion>::New( targetIndex,
                                                                              sources,
                                                                              funcPtr ) );
      break;
    }

    case Property::MATRIX:
    {
      Internal::PropertyConstraintPtr<Matrix>::Type funcPtr( CreatePropertyConstraint<Matrix>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Matrix>::New( targetIndex,
                                                                          sources,
                                                                          funcPtr ) );
      break;
    }

    case Property::MATRIX3:
    {
      Internal::PropertyConstraintPtr<Matrix3>::Type funcPtr( CreatePropertyConstraint<Matrix3>( func ) );

      constraint = Dali::Constraint( Internal::Constraint<Matrix3>::New( targetIndex,
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

  delete func;

  return constraint;
}

Constraint Constraint::DownCast( BaseHandle handle )
{
  return Constraint( dynamic_cast<Dali::Internal::ConstraintBase*>(handle.GetObjectPtr()) );
}

} // namespace Dali
