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
#include <dali/public-api/animation/constraint-v2.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/time-period.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/animation/constraint-source-impl.h>

namespace Dali
{

const ConstraintV2::RemoveAction ConstraintV2::DEFAULT_REMOVE_ACTION  = ConstraintV2::Bake;

ConstraintV2::ConstraintV2()
{
}

ConstraintV2::ConstraintV2( Internal::ConstraintV2* constraint )
: BaseHandle(constraint)
{
}

ConstraintV2::ConstraintV2(const ConstraintV2& handle)
: BaseHandle(handle)
{
}

ConstraintV2::~ConstraintV2()
{
}

ConstraintV2& ConstraintV2::operator=(const ConstraintV2& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

ConstraintV2 ConstraintV2::DownCast( BaseHandle handle )
{
  return ConstraintV2( dynamic_cast< Dali::Internal::ConstraintV2* >( handle.GetObjectPtr() ) );
}

void ConstraintV2::SetRemoveAction( ConstraintV2::RemoveAction action )
{
  GetImplementation(*this).SetRemoveAction(action);
}

ConstraintV2::RemoveAction ConstraintV2::GetRemoveAction() const
{
  return GetImplementation(*this).GetRemoveAction();
}

void ConstraintV2::SetTag( const unsigned int tag )
{
  GetImplementation(*this).SetTag( tag );
}

unsigned int ConstraintV2::GetTag() const
{
  return GetImplementation(*this).GetTag();
}

ConstraintV2 ConstraintV2::New( Actor actor,
                                Property::Index target,
                                Property::Type targetType,
                                CallbackBase* function )
{
//  return ConstraintV2( new Internal::ConstraintV2( target,
//                                               targetType,
//                                               sources,
//                                               func,
//                                               interpolator ) );

  return ConstraintV2();
}


} // namespace Dali
