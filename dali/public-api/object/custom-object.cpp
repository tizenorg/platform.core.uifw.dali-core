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
#include <dali/public-api/object/custom-object.h>

// INTERNAL INCLUDES
#include <dali/internal/event/object/custom-object-internal.h>

namespace Dali
{

CustomObject::CustomObject()
{
}

CustomObject CustomObject::DownCast( BaseHandle handle )
{
  return CustomObject( dynamic_cast<Dali::Internal::CustomObject*>(handle.GetObjectPtr()) );
}

CustomObject::~CustomObject()
{
}

CustomObjectImpl& CustomObject::GetImplementation()
{
  Internal::CustomObject& internal = GetImpl(*this);

  return internal.GetImplementation();
}

const CustomObjectImpl& CustomObject::GetImplementation() const
{
  const Internal::CustomObject& internal = GetImpl(*this);

  return internal.GetImplementation();
}

CustomObject::CustomObject(CustomObjectImpl& implementation)
: Handle(Internal::CustomObject::New(implementation))
{
}

CustomObject::CustomObject(Internal::CustomObject* internal)
: Handle(internal)
{
  // Check we haven't just constructed a new handle, while in the internal custom actors destructor.
  // This can happen if the user defined CustomObjectImpl destructor calls Self(), to create a new handle
  // to the CustomObject.
  //
  // If it's in the destructor then the ref count is zero, so once we've created a new handle it will be 1
  // Without this check, the actor will be deleted a second time, when the handle is disposed of
  // causing a crash.

  if (internal)
  {
    DALI_ASSERT_ALWAYS(internal->ReferenceCount() != 1 && "Are you trying to use CustomObjectImpl::Self() inside a CustomObjectImpl destructor?");
  }
}

} // namespace Dali
