#ifndef __DALI_PROPERTY_CONDITIONS_H__
#define __DALI_PROPERTY_CONDITIONS_H__

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
 * @addtogroup CAPI_DALI_OBJECT_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class PropertyCondition;
}

/**
 * @brief This represents a condition that can be evaluated on a Property::Value.
 */
class PropertyCondition : public BaseHandle
{
public:

  /**
   * @brief Argument container, represents the arguments to be supplied to the condition function.
   */
  typedef std::vector<Property::Value>      ArgumentContainer;
  typedef ArgumentContainer::iterator       ArgumentIter;     ///< Iterator for Dali::PropertyCondition::ArgumentContainer
  typedef ArgumentContainer::const_iterator ArgumentConstIter;///< Const Iterator for Dali::PropertyCondition::ArgumentContainer

public:

  /**
   * @brief Constructor for condition clause.
   */
  PropertyCondition();

  /**
   * @brief Destructor.
   */
  ~PropertyCondition();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public:

  /**
   * @brief Retrieve the arguments that this condition uses.
   *
   * @return The arguments used for this condition
   */
  ArgumentContainer GetArguments();

  /**
   * @brief Retrieve the arguments that this condition uses.
   *
   * @return The arguments used for this condition
   * @note The container will only be valid as long PropertyCondition is valid.
   */
  const ArgumentContainer& GetArguments() const;

};

/**
 * @brief LessThanCondition compares whether property is less than arg.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg The argument for the condition
 * @return A property condition function object
 */
PropertyCondition LessThanCondition(float arg);

/**
 * @brief GreaterThanCondition compares whether property is greater than arg.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg The argument for the condition
 * @return A property condition function object
 */
PropertyCondition GreaterThanCondition(float arg);

/**
 * @brief InsideCondition compares whether property is greater than arg0 and less than arg1.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 * @return A property condition function object
 */
PropertyCondition InsideCondition(float arg0, float arg1);

/**
 * @brief OutsideCondition compares whether property is less than arg0 or greater than arg1.
 *
 * property type:
 * bool (false = 0.0f, true = 1.0f)
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] arg0 The first argument for the condition
 * @param[in] arg1 The second argument for the condition
 * @return A property condition function object
 */
PropertyCondition OutsideCondition(float arg0, float arg1);

/**
 * @brief Detects when a property changes by stepAmount from initialValue, in both positive and negative directions. This will continue checking for multiples of stepAmount
 *
 * property type:
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] stepAmount The step size required to trigger condition
 * @param[in] initialValue The initial value to step from
 * @return A property condition function object
 */
PropertyCondition StepCondition(float stepAmount, float initialValue = 0.0f);

/**
 * @brief Receive notifications as a property goes above/below the inputted values. Values must be ordered and can be either ascending or descending
 *
 * property type:
 * float (float)
 * vector2 (the 2D length)
 * vector3 (the 3D length)
 * vector4 (the 4D length)
 * @param[in] steps List of values to receive notifications for as a property crosses them
 * @return A property condition function object
 */
PropertyCondition VariableStepCondition(const std::vector<float>& steps);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PROPERTY_CONDITIONS_H__
