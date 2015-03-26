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
#include <dali/public-api/animation/time-period.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/animation/constraint-source-impl.h>

namespace Dali
{

const AlphaFunction            Constraint::DEFAULT_ALPHA_FUNCTION = AlphaFunctions::Linear;
const Constraint::RemoveAction Constraint::DEFAULT_REMOVE_ACTION  = Constraint::Bake;

Constraint::Constraint()
{
}

Constraint::Constraint(Internal::Constraint* constraint)
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

void Constraint::SetApplyTime( TimePeriod timePeriod )
{
  GetImplementation(*this).SetApplyTime( timePeriod );
}

TimePeriod Constraint::GetApplyTime() const
{
  return GetImplementation(*this).GetApplyTime();
}

void Constraint::SetAlphaFunction( AlphaFunction func )
{
  GetImplementation(*this).SetAlphaFunction( func );
}

AlphaFunction Constraint::GetAlphaFunction()
{
  return GetImplementation(*this).GetAlphaFunction();
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

Constraint Constraint::New( Property::Index target,
                            Property::Type targetType,
                            CallbackBase* func )
{
  return Constraint( new Internal::Constraint( target,
                                               targetType,
                                               func ) );
}

Constraint Constraint::DownCast( BaseHandle handle )
{
  return Constraint( dynamic_cast<Dali::Internal::Constraint*>(handle.GetObjectPtr()) );
}

} // namespace Dali
