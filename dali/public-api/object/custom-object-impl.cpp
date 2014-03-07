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
#include <dali/public-api/object/custom-object-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/custom-handle.h>
#include <dali/internal/event/object/custom-object-internal.h>

namespace Dali
{

CustomObjectImpl::~CustomObjectImpl()
{
}

CustomHandle CustomObjectImpl::Self() const
{
  return CustomHandle(mOwner);
}

CustomObjectImpl::CustomObjectImpl()
: mOwner(NULL)
{
}

void CustomObjectImpl::Initialize(Internal::CustomObject& owner)
{
  DALI_ASSERT_DEBUG(mOwner == NULL); // should not already be owned

  mOwner = &owner;
}

Internal::CustomObject* CustomObjectImpl::GetOwner() const
{
  return mOwner;
}

} // namespace Dali
