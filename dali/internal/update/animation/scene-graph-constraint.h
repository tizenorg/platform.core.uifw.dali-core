#ifndef __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Used to constrain a property of a scene-object.
 * The constraint function takes another scene-object property as an input.
 */
template < class PropertyType, typename PropertyAccessorType >
class Constraint : public ConstraintBase
{
public:

  typedef typename PropertyConstraintPtr< PropertyType >::Type ConstraintFunctionPtr;
  typedef boost::function< PropertyType (const PropertyType&, const PropertyType&, float progress) > InterpolatorFunc;

  /**
   * Create a new scene-graph constraint.
   * @param[in] targetProperty The target property.
   * @param[in] ownerSet A set of property owners; func is connected to the properties provided by these objects.
   * @param[in] func The function to calculate the final constrained value.
   * @param[in] interpolator The function to interpolate between start & final value.
   * @return A smart-pointer to a newly allocated constraint.
   */
  static ConstraintBase* New( const PropertyBase& targetProperty,
                              PropertyOwnerSet& ownerSet,
                              ConstraintFunctionPtr func,
                              InterpolatorFunc interpolator )
  {
    // Scene-graph thread can edit these objects
    PropertyBase& property = const_cast< PropertyBase& >( targetProperty );

    return new Constraint< PropertyType, PropertyAccessorType >( property,
                                                                 ownerSet,
                                                                 func,
                                                                 interpolator );
  }

  /**
   * Virtual destructor.
   */
  virtual ~Constraint()
  {
  }

  /**
   * Query whether the constraint needs to be applied. If a constraint depends on a set of properties,
   * then it should be applied when any of those properties have changed.
   */
  bool ApplyNeeded()
  {
    if ( mFirstApply )
    {
      mFirstApply = false;
      return true;
    }

    if ( ! mTargetProperty.IsClean() ||
           mFunc->InputsChanged()    ||
         ! mWeight.IsClean() )
    {
      return true;
    }

    // We don't need to reapply constraint if none of the properties changed
    return false;
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::Apply()
   */
  virtual void Apply( BufferIndex updateBufferIndex )
  {
    if ( mDisconnected )
    {
      return; // Early-out when property owners have been disconnected
    }

    if ( mFunc->InputsInitialized() &&
         ApplyNeeded() )
    {
      const PropertyType& current = mTargetProperty.Get( updateBufferIndex );

      // FINAL_WEIGHT means the constraint is fully-applied, unless weight is still being animated
      if ( ! mWeight.IsClean() ||
           ! Equals( Dali::ActiveConstraint::FINAL_WEIGHT, mWeight[updateBufferIndex] ) )
      {
        // Constraint is not fully-applied; interpolation between start & final values
        mTargetProperty.Set( updateBufferIndex,
                             mInterpolator( current, mFunc->Apply( updateBufferIndex, current ), mWeight[updateBufferIndex] ) );
      }
      else
      {
        // Constraint is fully-applied; optionally bake the final value
        if ( Dali::Constraint::Bake == mRemoveAction )
        {
          mTargetProperty.Bake( updateBufferIndex, mFunc->Apply( updateBufferIndex, current ) );
        }
        else
        {
          mTargetProperty.Set( updateBufferIndex, mFunc->Apply( updateBufferIndex, current ) );
        }
      }

      INCREASE_COUNTER(PerformanceMonitor::CONSTRAINTS_APPLIED);
    }
    else
    {
      INCREASE_COUNTER(PerformanceMonitor::CONSTRAINTS_SKIPPED);
    }
  }

private:

  /**
   * @copydoc Dali::Internal::SceneGraph::Constraint::New()
   */
  Constraint( PropertyBase& targetProperty,
              PropertyOwnerSet& ownerSet,
              ConstraintFunctionPtr func,
              InterpolatorFunc interpolator )
  : ConstraintBase( ownerSet ),
    mTargetProperty( &targetProperty ),
    mFunc( func ),
    mInterpolator( interpolator )
  {
  }

  // Undefined
  Constraint( const Constraint& constraint );

  // Undefined
  Constraint& operator=( const Constraint& rhs );

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::OnDisconnect()
   */
  virtual void OnDisconnect()
  {
    // Discard target object/property pointers
    mTargetProperty.Reset();
    mFunc = NULL;
  }

protected:

  PropertyAccessorType mTargetProperty; ///< Raw-pointer to the target property. Not owned.

  ConstraintFunctionPtr mFunc;

  InterpolatorFunc mInterpolator;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__
