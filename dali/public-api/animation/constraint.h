#ifndef __DALI_CONSTRAINT_H__
#define __DALI_CONSTRAINT_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-input.h>

namespace Dali
{
struct TimePeriod;

namespace Internal DALI_INTERNAL
{
class Constraint;
}

typedef Vector< PropertyInput* > PropertyInputContainer;

/**
 * @brief An abstract base class for Constraints.
 * This can be used to constrain a property of an actor, after animations have been applied.
 * Constraints are applied in the following order:
 *   - Constraints are applied to on-stage actors in a depth-first traversal.
 *   - For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
 *   - Constraints are not applied to off-stage actors.
 */
class DALI_IMPORT_API Constraint : public BaseHandle
{
public:

  typedef Any AnyFunction; ///< Generic function pointer for constraint

  /**
   * @brief The action that will happen when the constraint is removed.
   *
   * Constraints can be applied gradually; see SetApplyTime() for more details.
   * When a constraint is fully-applied the final value may be "baked" i.e. saved permanently.
   * Alternatively the constrained value may be discarded when the constraint is removed.
   */
  enum RemoveAction
  {
    Bake,   ///< When the constraint is fully-applied, the constrained value is saved.
    Discard ///< When the constraint is removed, the constrained value is discarded.
  };

  static const AlphaFunction DEFAULT_ALPHA_FUNCTION; ///< AlphaFunctions::Linear
  static const RemoveAction  DEFAULT_REMOVE_ACTION;  ///< Bake

  /**
   * @brief Create an uninitialized Constraint; this can be initialized with Constraint::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Constraint();

  /**
   * @brief Create a constraint which targets a property.
   *
   * @param [in] target The index of the property to constrain.
   * @param [in] func A function which returns the constrained property value.
   * @return The new constraint.
   *
   * @tparam P The type of the property to constraint.
   */
  template < class P >
  static Constraint New( Property::Index target,
                         boost::function< void ( P& current, const PropertyInputContainer& inputs ) > func )
  {
    return New( target, PropertyTypes::Get<P>(), func );
  }

  /**
   * Adds a constraint source to the constraint
   *
   * @param[in] source The constraint source input to add
   */
  void AddSource( ConstraintSource source );

  /**
   * @brief Downcast an Object handle to Constraint handle.
   *
   * If handle points to a Constraint object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Constraint object or an uninitialized handle
   */
  static Constraint DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Constraint();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Constraint(const Constraint& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Constraint& operator=(const Constraint& rhs);

  /**
   * @brief Set the time taken for the constraint to be fully applied.
   *
   * The default is zero, meaning that the constraint is applied immediately.
   * @param [in] timePeriod The constraint will be applied during this time period.
   */
  void SetApplyTime( TimePeriod timePeriod );

  /**
   * @brief Retrieve the time taken for the constraint to be fully applied.
   *
   * @return The apply time.
   */
  TimePeriod GetApplyTime() const;

  /**
   * @brief Set the alpha function for a constraint; the default is AlphaFunctions::Linear.
   *
   * @param [in] func The alpha function to use when applying/removing the constraint.
   */
  void SetAlphaFunction( AlphaFunction func );

  /**
   * @brief Retrieve the alpha function of a constraint.
   *
   * @return The function.
   */
  AlphaFunction GetAlphaFunction();

  /**
   * @brief Set whether the constraint will "bake" a value when fully-applied.
   *
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * The default value is Constraint::Bake.
   * @param[in] action The remove-action.
   */
  void SetRemoveAction( RemoveAction action );

  /**
   * @brief Query whether the constraint will "bake" a value when fully-applied.
   *
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * @return The apply-action.
   */
  RemoveAction GetRemoveAction() const;

  /**
   * @brief Set a tag for the constraint so it can be identified later
   *
   * @param[in] tag An integer to identify the constraint
   */
  void SetTag( const unsigned int tag );

  /**
   * @brief Get the tag
   *
   * @return The tag
   */
  unsigned int GetTag() const;

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods
   * @param [in] constraint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Constraint( Internal::Constraint* constraint );

private: // Not intended for use by Application developers

  /**
   * @brief Construct a new constraint which targets a property.
   *
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] func The constraint function.
   * @return The new constraint.
   */
  static Constraint New( Property::Index target,
                         Property::Type targetType,
                         AnyFunction func );
};

} // namespace Dali

#endif // __DALI_CONSTRAINT_H__
