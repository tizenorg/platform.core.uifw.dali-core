#ifndef __DALI_CUSTOM_OBJECT_IMPL_H__
#define __DALI_CUSTOM_OBJECT_IMPL_H__

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class CustomObject;
}

class CustomHandle;
class CustomObjectImpl;

typedef IntrusivePtr<CustomObjectImpl> CustomObjectImplPtr;

/**
 * CustomObjectImpl is an base class for custom object implementations.
 * This stores addition data for the derived type of CustomObject.
 */
class DALI_IMPORT_API CustomObjectImpl : public Dali::RefObject
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~CustomObjectImpl();

  /**
   * Used by derived CustomObjectImpl instances, to access the public CustomObject API.
   * @return A handle to self, or an empty handle if the CustomObjectImpl is not owned.
   */
  CustomHandle Self() const;

protected: // For derived classes

  /**
   * Create a CustomObjectImpl.
   * @param[in] requiresTouchEvents True if the OnTouchEvent() callback is required.
   */
  CustomObjectImpl();

public: // Not intended for application developers

  /**
   * Called when ownership of the CustomObjectImpl passed to a CustomObject.
   * @pre The CustomObjectImpl is not already owned.
   * @param[in] owner The owning object.
   */
  void Initialize( Internal::CustomObject& owner );

  /**
   * This method is needed when creating additional handle objects to existing objects.
   * Owner is the Dali::Internal::CustomObject that owns the implementation of the custom object inside core.
   * @return A pointer to the CustomObject that owns this custom object implementation.
   */
  Internal::CustomObject* GetOwner() const;

private:

  Internal::CustomObject* mOwner;
};

} // namespace Dali

#endif // __DALI_CUSTOM_OBJECT_IMPL_H__
