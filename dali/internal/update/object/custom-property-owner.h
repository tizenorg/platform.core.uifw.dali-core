#ifndef __DALI_INTERNAL_UPDATE_CUSTOM_PROPERTY_OWNER_H__
#define __DALI_INTERNAL_UPDATE_CUSTOM_PROPERTY_OWNER_H__

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
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{

namespace Internal
{

namespace Update
{

/**
 * An object for storing custom animatable properties
 */
class CustomPropertyOwner : public SceneGraph::PropertyOwner
{
public:

  /**
   * Create a new CustomPropertyOwner
   */
  static CustomPropertyOwner* New();

  /**
   * Virtual destructor
   */
  virtual ~CustomPropertyOwner();

private:

  /**
   * Protected constructor.
   */
  CustomPropertyOwner();

  // Undefined
  CustomPropertyOwner(const CustomPropertyOwner&);

  // Undefined
  CustomPropertyOwner& operator=(const CustomPropertyOwner&);

  /**
   * @copydoc PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex currentBufferIndex ) {}
};

} // namespace Update

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_CUSTOM_PROPERTY_OWNER_H__
