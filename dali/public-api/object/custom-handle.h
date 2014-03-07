#ifndef __DALI_CUSTOM_HANDLE_H__
#define __DALI_CUSTOM_HANDLE_H__

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
#include <dali/public-api/object/constrainable.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class CustomObject;
}

/**
 * A handle to a custom object.
 * This is mainly useful for registering properties, without the extra overhead of using CustomActor.
 */
class DALI_IMPORT_API CustomHandle : public Constrainable
{
public:

  /**
   * Create a custom object.
   * @return A handle to a newly allocated object.
   */
  static CustomHandle New();

  /**
   * Create an empty handle.
   */
  CustomHandle();

  /**
   * Downcast a handle to a custom object.
   * @param[in] handle The handle to cast.
   * @return A handle to a custom object or an empty handle.
   */
  static CustomHandle DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Handle derived classes do not contain member data.
   */
  virtual ~CustomHandle();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Not intended for application developers

  /**
   * This constructor is used internally to create additional handles.
   * @param [in] obj A pointer to a newly allocated object.
   */
  CustomHandle( Internal::CustomObject* obj );
};

} // namespace Dali

#endif // __DALI_CUSTOM_HANDLE_H__
