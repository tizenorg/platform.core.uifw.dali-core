#ifndef __DALI_CONSTRAINT_H__
#define __DALI_CONSTRAINT_H__

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
#include <boost/any.hpp>
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/animation/interpolator-functions.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>

namespace Dali DALI_IMPORT_API
{
struct TimePeriod;

namespace Internal DALI_INTERNAL
{
class Constraint;
}

/**
 * An abstract base class for Constraints.
 * This can be used to constrain a property of an actor, after animations have been applied.
 * Constraints are applied in the following order:
 *   - Constraints are applied to on-stage actors in a depth-first traversal.
 *   - For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
 *   - Constraints are not applied to off-stage actors.
 */
class DALI_IMPORT_API Constraint : public BaseHandle
{
public:

  typedef boost::any AnyFunction;

  /**
   * Constraints can be applied gradually; see SetApplyTime() for more details.
   * When a constraint is fully-applied the final value may be "baked" i.e. saved permanently.
   * Alternatively the constrained value may be discarded when the constraint is removed.
   */
  enum RemoveAction
  {
    Bake,   ///< When the constraint is fully-applied, the constrained value is saved.
    Discard ///< When the constraint is removed, the constrained value is discarded.
  };

  static const AlphaFunction DEFAULT_ALPHA_FUNCTION; // AlphaFunctions::Linear
  static const RemoveAction  DEFAULT_REMOVE_ACTION;  // Bake

  /**
   * Create an uninitialized Constraint; this can be initialized with Constraint::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Constraint();

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         boost::function<P (const P& current)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                func,
                GetDefaultInterpolator( PropertyTypes::Get<P>() ) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         boost::function<P (const P& current)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         boost::function<P (const P& current, const PropertyInput& input1)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                func,
                GetDefaultInterpolator( PropertyTypes::Get<P>() ) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         boost::function<P (const P& current, const PropertyInput& input1)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         boost::function<P (const P& current, const PropertyInput& input1, const PropertyInput& input2)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                func,
                GetDefaultInterpolator(PropertyTypes::Get<P>()) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         ConstraintSource source2,
                                         boost::function<P (const P& current, const PropertyInput& input1, const PropertyInput& input2)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         boost::function<P (const P& current, const PropertyInput& input1, const PropertyInput& input2, const PropertyInput& input3)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                func,
                GetDefaultInterpolator(PropertyTypes::Get<P>()) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         ConstraintSource source2,
                                         ConstraintSource source3,
                                         boost::function<P (const P& current, const PropertyInput& input1, const PropertyInput& input2, const PropertyInput& input3)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         boost::function<P (const P& current,
                                            const PropertyInput& input1,
                                            const PropertyInput& input2,
                                            const PropertyInput& input3,
                                            const PropertyInput& input4)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                func,
                GetDefaultInterpolator(PropertyTypes::Get<P>()) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         ConstraintSource source2,
                                         ConstraintSource source3,
                                         ConstraintSource source4,
                                         boost::function<P (const P& current,
                                                            const PropertyInput& input1,
                                                            const PropertyInput& input2,
                                                            const PropertyInput& input3,
                                                            const PropertyInput& input4)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         ConstraintSource source5,
                         boost::function<P (const P& current,
                                            const PropertyInput& input1,
                                            const PropertyInput& input2,
                                            const PropertyInput& input3,
                                            const PropertyInput& input4,
                                            const PropertyInput& input5)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                source5,
                func,
                GetDefaultInterpolator(PropertyTypes::Get<P>()) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         ConstraintSource source2,
                                         ConstraintSource source3,
                                         ConstraintSource source4,
                                         ConstraintSource source5,
                                         boost::function<P (const P& current,
                                                            const PropertyInput& input1,
                                                            const PropertyInput& input2,
                                                            const PropertyInput& input3,
                                                            const PropertyInput& input4,
                                                            const PropertyInput& input5)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                source5,
                func,
                interpolator );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * Animation will be performed using the default interpolator.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] source6 The source of a property; the current value will be passed as the 7th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   */
  template <class P>
  static Constraint New( Property::Index target,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         ConstraintSource source5,
                         ConstraintSource source6,
                         boost::function<P (const P& current,
                                            const PropertyInput& input1,
                                            const PropertyInput& input2,
                                            const PropertyInput& input3,
                                            const PropertyInput& input4,
                                            const PropertyInput& input5,
                                            const PropertyInput& input6)> func )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                source5,
                source6,
                func,
                GetDefaultInterpolator(PropertyTypes::Get<P>()) );
  }

  /**
   * Create a constraint which targets a property.
   * The templated parameter P, is the type of the property to constrain.
   * @param [in] target The index of the property to constrain.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] source6 The source of a property; the current value will be passed as the 7th parameter of func.
   * @param [in] func A function which returns the constrained property value.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   * @return The new constraint.
   */
  template <class P>
  static Constraint NewWithInterpolator( Property::Index target,
                                         ConstraintSource source1,
                                         ConstraintSource source2,
                                         ConstraintSource source3,
                                         ConstraintSource source4,
                                         ConstraintSource source5,
                                         ConstraintSource source6,
                                         boost::function<P (const P& current,
                                                            const PropertyInput& input1,
                                                            const PropertyInput& input2,
                                                            const PropertyInput& input3,
                                                            const PropertyInput& input4,
                                                            const PropertyInput& input5,
                                                            const PropertyInput& input6)> func,
                                         boost::function<P (const P&, const P&, float)> interpolator )
  {
    return New( target,
                PropertyTypes::Get<P>(),
                source1,
                source2,
                source3,
                source4,
                source5,
                source6,
                func,
                interpolator );
  }

  /**
   * Downcast an Object handle to Constraint handle. If handle points to a Constraint object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Constraint object or an uninitialized handle
   */
  static Constraint DownCast( BaseHandle handle );

  /**
   * Destructor
   */
  virtual ~Constraint();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Set the time taken for the constraint to be fully applied.
   * The default is zero, meaning that the constraint is applied immediately.
   * @param [in] timePeriod The constraint will be applied during this time period.
   */
  void SetApplyTime( TimePeriod timePeriod );

  /**
   * Retrieve the time taken for the constraint to be fully applied.
   * @return The apply time.
   */
  TimePeriod GetApplyTime() const;

  /**
   * Set the time taken for the constraint to be fully removed.
   * The default is zero, meaning that the constraint is removed immediately.
   * @param [in] timePeriod The constraint will be removed during this time period.
   */
  void SetRemoveTime( TimePeriod timePeriod );

  /**
   * Retrieve the time taken for the constraint to be fully removed.
   * @return The remove time.
   */
  TimePeriod GetRemoveTime() const;

  /**
   * Set the alpha function for a constraint; the default is AlphaFunctions::Linear.
   * @param [in] func The alpha function to use when applying/removing the constraint.
   */
  void SetAlphaFunction( AlphaFunction func );

  /**
   * Retrieve the alpha function of a constraint.
   * @return The function.
   */
  AlphaFunction GetAlphaFunction();

  /**
   * Set whether the constraint will "bake" a value when fully-applied.
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * The default value is Constraint::Bake.
   * @param[in] action The remove-action.
   */
  void SetRemoveAction( RemoveAction action );

  /**
   * Query whether the constraint will "bake" a value when fully-applied.
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * @return The apply-action.
   */
  RemoveAction GetRemoveAction() const;

public: // Not intended for use by Application developers

  /**
   * This constructor is used by Dali New() methods
   * @param [in] constraint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Constraint( Internal::Constraint* constraint );

private: // Not intended for use by Application developers

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         ConstraintSource source5,
                         AnyFunction func,
                         AnyFunction interpolator );

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] source1 The source of a property; the current value will be passed as the 2nd parameter of func.
   * @param [in] source2 The source of a property; the current value will be passed as the 3rd parameter of func.
   * @param [in] source3 The source of a property; the current value will be passed as the 4th parameter of func.
   * @param [in] source4 The source of a property; the current value will be passed as the 5th parameter of func.
   * @param [in] source5 The source of a property; the current value will be passed as the 6th parameter of func.
   * @param [in] source6 The source of a property; the current value will be passed as the 7th parameter of func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         ConstraintSource source1,
                         ConstraintSource source2,
                         ConstraintSource source3,
                         ConstraintSource source4,
                         ConstraintSource source5,
                         ConstraintSource source6,
                         AnyFunction func,
                         AnyFunction interpolator );
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CONSTRAINT_H__
