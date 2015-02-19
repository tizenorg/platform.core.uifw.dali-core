#ifndef __DALI_CONSTRAINT_V2_H__
#define __DALI_CONSTRAINT_V2_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/interpolator-functions.h>
#include <dali/public-api/object/any.h>



#include <dali/public-api/animation/constraint-source.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-input.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{
class Actor;

struct TimePeriod;

namespace Internal DALI_INTERNAL
{
class ConstraintV2;
}

typedef Dali::Vector< PropertyInput* > PropertyInputContainer;

/**
 * @brief An abstract base class for Constraints.
 *
 * This can be used to constrain a property of an actor, after animations have been applied.
 * Constraints are applied in the following order:
 *   - Constraints are applied to on-stage actors in a depth-first traversal.
 *   - For each actor, the constraints are applied in the same order as the calls to Actor::ApplyConstraint().
 *   - Constraints are not applied to off-stage actors.
 *
 * @see ConstraintV2::Function
 */
class DALI_IMPORT_API ConstraintV2 : public BaseHandle
{
public:

  /**
   * @brief Template for the Function that is called by the Constraint system.
   *
   * Supports:
   *  - C style functions
   *  - Static member methods of an object
   *  - Member functions of a particular class
   *  - Functors of a particular class
   *
   * The expected signature of the callback should be:
   * @code
   *   void Function( P&, const PropertyInputContainer& );
   * @endcode
   *
   * The P& parameter is an in,out parameter which stores the current value of the property. The callback
   * should change this value to the desired one. The PropertyInputContainer is a const reference to the property inputs
   * added to the Constraint in the order they were added via AddSource().
   *
   * @tparam  P  The property type to constrain.
   */
  template< typename P >
  class Function : public CallbackBase
  {
  public:

    /**
     * @brief Constructor which connects to the provided C function (or a static member function).
     *
     * The expected signature of the function is:
     * @code
     *   void MyFunction( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @param[in]  function  The function to call.
     */
    Function( void( *function )( P&, const PropertyInputContainer& ) )
    : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) ) { }

    /**
     * @brief Constructor which copies a function object and connects to the functor of that object.
     *
     * The function object should have a functor with the following signature:
     * @code
     *   void operator()( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @param[in]  object  The object to copy.
     *
     * @tparam  T  The type of the object.
     */
    template< class T >
    Function( const T& object )
    : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                    reinterpret_cast< CallbackBase::MemberFunction >( &T::operator() ),
                    reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher2< T, P&, const PropertyInputContainer& >::Dispatch ),
                    reinterpret_cast< CallbackBase::Destructor >( &Destroyer< T >::Delete ) ) { }

    /**
     * @brief Constructor which copies a function object and allows a connection to a member method.
     *
     * The object should have a method with the signature:
     * @code
     *   void MyObject::MyFunctor( P&, const PropertyInputContainer& );
     * @endcode
     *
     * @param[in]  object          The object to copy.
     * @param[in]  memberFunction  The member function to call. This has to be a member of the same class.
     *
     * @tparam  T  The type of the object.
     */
    template< class T >
    Function( const T& object, void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
    : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                    reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                    reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher2< T, P&, const PropertyInputContainer& >::Dispatch ),
                    reinterpret_cast< CallbackBase::Destructor >( &Destroyer< T >::Delete ) ) { }
  };

  /**
   * @brief Create an uninitialized Constraint; this can be initialized with Constraint::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ConstraintV2();

  /**
   * @brief Create a constraint which targets a property.
   *
   * @param[in]  actor     The actor
   * @param[in]  target    The index of the property to constrain.
   * @param[in]  function  The function to call to apply the constraint.
   *
   * @tparam  P  The type of the property to constrain
   *
   * @return The new constraint.
   */
  template< typename P >
  static ConstraintV2 New( Actor actor, Property::Index target, Function< P >* function )
  {
    return New( actor, target, PropertyTypes::Get< P >(), function );
  }

  /**
   * @brief Downcast an Object handle to Constraint handle.
   *
   * If handle points to a Constraint object the downcast produces a valid handle.
   * If not the returned handle is left uninitialized.
   *
   * @param[in]  handle  The handle to an object
   *
   * @return handle to a Constraint object or an uninitialized handle
   */
  static ConstraintV2 DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ConstraintV2();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in]  handle  A reference to the copied handle
   */
  ConstraintV2( const ConstraintV2& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in]  rhs  A reference to the copied handle
   * @return A reference to this
   */
  ConstraintV2& operator=( const ConstraintV2& rhs );

  /**
   * @brief Add a property source to the constraint
   *
   * The order in which sources are added is the order in which they will be sorted in the callback function's PropertyInputContainer.
   *
   * @param[in]  source  The source of a property; the current value will be passed as an element of PropertyInputContainer.
   */
  void AddSource( ConstraintSource source );

  /**
   * @brief Applies the constraint
   */
  void Apply();

  /**
   * @brief Set a tag for the constraint so it can be identified later
   *
   * @param[in]  tag  An integer to identify the constraint
   */
  void SetTag( const unsigned int tag );

  /**
   * @brief Get the tag
   *
   * @return The tag
   */
  unsigned int GetTag() const;

  //
  // May remove the following
  //

  typedef Any AnyFunction; ///< Generic function pointer for constraint and interpolator functions

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

  static const RemoveAction  DEFAULT_REMOVE_ACTION;  ///< Bake

  /**
   * @brief Set whether the constraint will "bake" a value when fully-applied.
   *
   * Otherwise the constrained value will be discarded, when the constraint is removed.
   * The default value is ConstraintV2::Bake.
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

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods
   *
   * @param [in] constraint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ConstraintV2( Internal::ConstraintV2* constraint );

private: // Not intended for use by Application developers

  /**
   * @brief Construct a new constraint which targets a property.
   *
   * @param[in] actor       The actor to apply the constraint to.
   * @param[in] target      The index of the property to constrain.
   * @param[in] targetType  The type of the constrained property.
   * @param[in] function    The constraint function.
   *
   * @return The new constraint.
   */
  static ConstraintV2 New( Actor actor,
                           Property::Index target,
                           Property::Type targetType,
                           CallbackBase* function );
};

/**
 * @brief Creates a callback that can be used with constraints from a C function or static member function.
 *
 * The expected signature of the function is:
 * @code
 *   void MyFunction( P&, const PropertyInputContainer& );
 * @endcode
 *
 * @param[in]  function  The function to call.
 *
 * @tparam  P  The type of the property to constrain.
 *
 * @return a newly allocated Constraint::Function object, ownership transferred to caller
 */
template< typename P >
inline ConstraintV2::Function< P >* MakeCallback( void( *function )( P&, const PropertyInputContainer& ) )
{
  return new ConstraintV2::Function< P >( function );
}

/**
 * @brief Creates a callback from a class's member function that can be used with constraints.
 *
 * The object should have a method with the signature:
 * @code
 *   void MyObject::MyFunctor( P&, const PropertyInputContainer& );
 * @endcode *
 *
 * @param[in]  object    The object to call.
 * @param[in]  function  The member function to call which has to be a member of the same class.
 *
 * @tparam  T  The type of the object.
 * @tparam  P  The type of the property to constrain.
 *
 * @return a newly allocated Callback object, ownership transferred to caller
 */
template< class T, typename P >
inline ConstraintV2::Function< P >* MakeCallback( T object, void ( T::*memberFunction ) ( P&, const PropertyInputContainer& ) )
{
  return new ConstraintV2::Function< P >( object, memberFunction );
}

/**
 * @brief Creates a callback from a class whose functor can be used with constraints.
 *
 * The object should have a functor with the signature:
 * @code
 *   void MyObject::MyFunctor( P&, const PropertyInputContainer& );
 * @endcode *
 *
 * @param[in]  object  The object to call.
 *
 * @tparam  T  The type of the object.
 * @tparam  P  The type of the property to constrain.
 *
 * @return a newly allocated Callback object, ownership transferred to caller
 */
template< class T, typename P >
inline ConstraintV2::Function< P >* MakeCallback( T object )
{
  return new ConstraintV2::Function< P >( object );
}

} // namespace Dali

#endif // __DALI_CONSTRAINT_V2_H__
