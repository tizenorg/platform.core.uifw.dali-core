#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__

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
#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/animation/animator-connector-base.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/property-accessor.h>
#include <dali/internal/update/animation/property-component-accessor.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{

namespace Internal
{

/**
 * AnimatorConnector is used to connect SceneGraph::Animators for newly created scene-graph objects.
 *
 * The scene-graph objects are created by a ProxyObject e.g. Actor is a proxy for SceneGraph::Node.
 * AnimatorConnector observes the proxy object, in order to detect when a scene-graph object is created.
 *
 * SceneGraph::Animators weakly reference scene objects, and are automatically deleted when orphaned.
 * Therefore the AnimatorConnector is NOT responsible for disconnecting animators.
 */
template < typename PropertyType >
class AnimatorConnector : public AnimatorConnectorBase, public ProxyObject::Observer
{
public:

  typedef boost::function< PropertyType (float, const PropertyType&) > AnimatorFunction;
  typedef SceneGraph::Animator< PropertyType, PropertyAccessor<PropertyType> > AnimatorType;
  typedef SceneGraph::AnimatableProperty< PropertyType > PropertyInterfaceType;

  /**
   * Construct a new animator connector.
   * @param[in] proxy The proxy for a scene-graph object to animate.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4 (INVALID_PROPERTY_COMPONENTINDEX to use the whole property)
   * @param[in] animatorFunction A function used to animate the property.
   * @param[in] alpha The alpha function to apply.
   * @param[in] period The time period of the animator.
   * @return A pointer to a newly allocated animator connector.
   */
  static AnimatorConnectorBase* New( ProxyObject& proxy,
                                     Animation& parent,
                                     Property::Index propertyIndex,
                                     int componentIndex,
                                     const AnimatorFunction& animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector< PropertyType >( proxy,
                                                  parent,
                                                  propertyIndex,
                                                  componentIndex,
                                                  animatorFunction,
                                                  alpha,
                                                  period );
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnector()
  {
    if( mProxy )
    {
      mProxy->RemoveObserver( *this );
    }

    if ( Stage::IsInstalled() )
    {
      const SceneGraph::Animation* animation = mParent->GetSceneObject();
      if( animation != NULL )
      {
        printf("RemoveAnimatorMessage\n");
        RemoveAnimatorMessage( mParent->GetUpdateManager().GetEventToUpdate(), *animation, *mAnimator );
      }
    }
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( ProxyObject& proxy,
                     Animation& parent,
                     Property::Index propertyIndex,
                     int componentIndex,
                     const AnimatorFunction& animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  :AnimatorConnectorBase(parent),
   mProxy( &proxy ),
   mConnected( false )
  {
    proxy.AddObserver( *this );

    //Create an SceneGraph::Animator
    const SceneGraph::PropertyBase* base = proxy.GetSceneObjectAnimatableProperty( propertyIndex );

    const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
    DALI_ASSERT_DEBUG( NULL != sceneProperty && "Animating property with invalid type" );

    mAnimator = AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
    DALI_ASSERT_DEBUG( NULL != mAnimator );

    const SceneGraph::Animation* animation = parent.GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != animation );

    //Add animator to the SceneGraph::Animation
    AddAnimatorMessage( parent.GetUpdateManager().GetEventToUpdate(), *animation, *mAnimator );

    const SceneGraph::PropertyOwner* object = proxy.GetSceneObject();
    if( object )
    {
      //Enable animator if object is on stage
      mAnimator->Attach( const_cast<SceneGraph::PropertyOwner*>( object ) );
      mConnected = true; // not really necessary, but useful for asserts
    }

  }

  // Undefined
  AnimatorConnector( const AnimatorConnector& );

  // Undefined
  AnimatorConnector& operator=( const AnimatorConnector& rhs );

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    SceneGraph::PropertyOwner* object = const_cast<SceneGraph::PropertyOwner*>(mProxy->GetSceneObject());
    DALI_ASSERT_DEBUG( NULL != object );

    mAnimator->Attach(object);
    mConnected = true;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
    mConnected = false;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    mConnected = false;
    mProxy = NULL;
  }

protected:

  ProxyObject* mProxy; ///< Not owned by the animator connector. Valid until ProxyDestroyed() is called.
  bool mConnected; ///< Used to guard against double connections
};

/**
 * Variant which allows float components to be animated individually.
 */
template <>
class AnimatorConnector<float> : public AnimatorConnectorBase, public ProxyObject::Observer
{
public:

  typedef boost::function< float (float, const float&) > AnimatorFunction;
  typedef SceneGraph::Animator< float, PropertyAccessor<float> > AnimatorType;

  /**
   * Construct a new animator connector.
   * @param[in] proxy The proxy for a scene-graph object to animate.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4
   * @param[in] animatorFunction A function used to animate the property.
   * @param[in] alpha The alpha function to apply.
   * @param[in] period The time period of the animator.
   * @return A pointer to a newly allocated animator connector.
   */
  static AnimatorConnectorBase* New( ProxyObject& proxy,
                                     Animation& parent,
                                     Property::Index propertyIndex,
                                     int componentIndex,
                                     const AnimatorFunction& animatorFunction,
                                     AlphaFunction alpha,
                                     const TimePeriod& period )
  {
    return new AnimatorConnector<float>( proxy,
                                         parent,
                                         propertyIndex,
                                         componentIndex,
                                         animatorFunction,
                                         alpha,
                                         period );
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnector()
  {
    if( mProxy )
    {
      mProxy->RemoveObserver( *this );
    }

    if( Stage::IsInstalled())
    {
      const SceneGraph::Animation* animation = mParent->GetSceneObject();
      if( animation != NULL )
      {
        RemoveAnimatorMessage( mParent->GetUpdateManager().GetEventToUpdate(), *animation, *mAnimator );
      }
    }
  }

private:

  /**
   * Private constructor; see also AnimatorConnector::New().
   */
  AnimatorConnector( ProxyObject& proxy,
                     Animation& parent,
                     Property::Index propertyIndex,
                     int componentIndex,
                     const AnimatorFunction& animatorFunction,
                     AlphaFunction alpha,
                     const TimePeriod& period )
  : AnimatorConnectorBase(parent),
    mProxy( &proxy ),
    mConnected( false )
  {
    proxy.AddObserver( *this );

    //Create an SceneGraph::Animator

    const SceneGraph::PropertyBase* base = proxy.GetSceneObjectAnimatableProperty( propertyIndex );
    DALI_ASSERT_DEBUG( NULL != base );

    int propertyComponentIndex = proxy.GetPropertyComponentIndex( propertyIndex );
    if( propertyComponentIndex != Property::INVALID_COMPONENT_INDEX )
    {
      componentIndex = propertyComponentIndex;
    }

    if ( Property::INVALID_COMPONENT_INDEX == componentIndex )
    {
      // Not a Vector3 or Vector4 component, expecting float type
      DALI_ASSERT_DEBUG( PropertyTypes::Get< float >() == base->GetType() );

      typedef SceneGraph::AnimatableProperty< float > PropertyInterfaceType;

      const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
      DALI_ASSERT_DEBUG( NULL != sceneProperty );

      mAnimator = AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
    }
    else
    {
      // Expecting Vector3 or Vector4 type

      if ( PropertyTypes::Get< Vector3 >() == base->GetType() )
      {
        // Animate float component of Vector3 property
        typedef SceneGraph::AnimatableProperty< Vector3 > PropertyInterfaceType;

        const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
        DALI_ASSERT_DEBUG( NULL != sceneProperty );

        if ( 0 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorX<Vector3> > Vector3AnimatorType;
          mAnimator = Vector3AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
        else if ( 1 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorY<Vector3> > Vector3AnimatorType;
          mAnimator = Vector3AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
        else if ( 2 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector3> > Vector3AnimatorType;
          mAnimator = Vector3AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
      }
      else if ( PropertyTypes::Get< Vector4 >() == base->GetType() )
      {
        // Animate float component of Vector4 property
        typedef SceneGraph::AnimatableProperty< Vector4 > PropertyInterfaceType;

        const PropertyInterfaceType* sceneProperty = dynamic_cast< const PropertyInterfaceType* >( base );
        DALI_ASSERT_DEBUG( NULL != sceneProperty );

        if ( 0 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorX<Vector4> > Vector4AnimatorType;
          mAnimator = Vector4AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
        else if ( 1 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorY<Vector4> > Vector4AnimatorType;
          mAnimator = Vector4AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
        else if ( 2 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorZ<Vector4> > Vector4AnimatorType;
          mAnimator = Vector4AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
        else if ( 3 == componentIndex )
        {
          typedef SceneGraph::Animator< float, PropertyComponentAccessorW<Vector4> > Vector4AnimatorType;
          mAnimator = Vector4AnimatorType::New( *sceneProperty, animatorFunction, alpha, period );
        }
      }
    }

    DALI_ASSERT_DEBUG( NULL != mAnimator && "Animating property with invalid type" );

    const SceneGraph::Animation* animation = parent.GetSceneObject();
    DALI_ASSERT_DEBUG( NULL != animation );

    //Add the animator to the SceneGraph::Animation
    AddAnimatorMessage( parent.GetUpdateManager().GetEventToUpdate(), *animation, *mAnimator );

    SceneGraph::PropertyOwner* object = const_cast<SceneGraph::PropertyOwner*>( mProxy->GetSceneObject() );
    if( object )
    {
      //Enable animator if object is on stage
      mAnimator->Attach( object );
      mConnected = true; // not really necessary, but useful for asserts
    }
  }

  // Undefined
  AnimatorConnector( const AnimatorConnector& );

  // Undefined
  AnimatorConnector& operator=( const AnimatorConnector& rhs );

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectAdded( ProxyObject& proxy )
  {
    SceneGraph::PropertyOwner* object = const_cast<SceneGraph::PropertyOwner*>( mProxy->GetSceneObject() );
    DALI_ASSERT_DEBUG( NULL != object );

    mAnimator->Attach( object );
    mConnected = true;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy )
  {
    // Removing animators via Animation is not necessary.
    // The corresponding scene graph animation will destroy any animators that become disconnected.
    mConnected = false;
  }

  /**
   * From ProxyObject::Observer
   */
  virtual void ProxyDestroyed( ProxyObject& proxy )
  {
    mConnected = false;
    mProxy = NULL;
  }

protected:

  ProxyObject* mProxy; ///< Not owned by the animator connector. Valid until ProxyDestroyed() is called.
  bool mConnected; ///< Used to guard against double connections
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_H__
