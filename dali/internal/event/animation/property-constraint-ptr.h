#ifndef __DALI_INTERNAL_PROPERTY_CONSTRAINT_PTR_H__
#define __DALI_INTERNAL_PROPERTY_CONSTRAINT_PTR_H__

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
#include <dali/internal/event/animation/property-constraint.h>
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{

namespace Internal
{

template <class P>
struct PropertyConstraintPtr
{
  typedef OwnerPointer< PropertyConstraintBase<P> > Type;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_CONSTRAINT_PTR_H__
