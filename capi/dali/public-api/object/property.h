#ifndef __DALI_PROPERTY_H__
#define __DALI_PROPERTY_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <string>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

class Handle;

/**
 * An object + property pair.
 */
struct Property
{
  /**
   * A valid property index is zero or greater.
   */
  typedef int Index;

  static const int INVALID_INDEX; ///< -1 is not a valid property index

  /**
   * A value-type representing a property value.
   */
  class Value;

  /**
   * A Map of property values
   */
  typedef std::pair<std::string, Value> StringValuePair;
  typedef std::vector<StringValuePair> Map;

  /**
   * An Array of property values
   */
  typedef std::vector<Value> Array;

  /**
   * The property types supported.
   */
  enum Type
  {
    NONE,

    BOOLEAN,
    FLOAT,
    INTEGER,
    UNSIGNED_INTEGER,
    VECTOR2,         // a vector array of size=2 with float precision
    VECTOR3,
    VECTOR4,
    MATRIX3,         // a 3x3 matrix
    MATRIX,          // a 4x4 matrix
    RECTANGLE,       // an integer array of size=4
    ROTATION,        // either a quaternion or an axis angle rotation
    STRING,
    ARRAY,           // an array of Property::Value
    MAP,             // a string key to Property:value mapping
    TYPE_COUNT
  };

  /*
   * The access mode for custom properties
   */
  enum AccessMode
  {
    READ_ONLY,
    READ_WRITE,
    ANIMATABLE,
    ACCESS_MODE_COUNT
  };


  /**
   * Create a Property instance.
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyIndex The index of a property.
   */
  Property( Handle& object, Property::Index propertyIndex );

  /**
   * Create a Property instance.
   * @note This performs a property index query and is therefore slower than
   * constructing a Property directly with the index.
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyName The property name.
   */
  Property( Handle& object, const std::string& propertyName );

  /**
   * Non-virtual destructor; Property is not intended as a base class.
   */
  ~Property();

  Handle& object; ///< A valid handle to the target object.

  Index propertyIndex; ///< The index of a property provided by object.
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PROPERTY_H__
