#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/manager/update-manager.h>
#include <string>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class UniformMap;
class PropertyOwner;

// Property Messages for PropertyOwner

class PropertyMessageBase : public MessageBase
{
public:

  /**
   * Create a message.
   */
  PropertyMessageBase(UpdateManager& updateManager);

  /**
   * Virtual destructor
   */
  virtual ~PropertyMessageBase();

private:

  // Undefined
  PropertyMessageBase(const PropertyMessageBase&);
  PropertyMessageBase& operator=(const PropertyMessageBase& rhs);

protected:

  UpdateManager& mUpdateManager;
};

/**
 * Templated message which bakes a property.
 */
template< typename P >
class PropertyMessage : public PropertyMessageBase
{
public:

  typedef void(AnimatableProperty<P>::*MemberFunction)( BufferIndex, typename ParameterType< P >::PassingType );

  /**
   * Create a message.
   * @note The scene object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] sceneObject The property owner scene object
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  static void Send( UpdateManager& updateManager,
                    const PropertyOwner* sceneObject,
                    const AnimatableProperty<P>* property,
                    MemberFunction member,
                    typename ParameterType< P >::PassingType value )
  {
    // Reserve some memory inside the message queue
    unsigned int* slot = updateManager.GetEventToUpdate().ReserveMessageSlot( sizeof( PropertyMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) PropertyMessage( updateManager, sceneObject, property, member, value );
  }

  /**
   * Virtual destructor
   */
  virtual ~PropertyMessage()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex updateBufferIndex )
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The property owner is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] sceneObject the property owner scene object
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the property.
   */
  PropertyMessage( UpdateManager& updateManager,
                   const PropertyOwner* sceneObject,
                   const AnimatableProperty<P>* property,
                   MemberFunction member,
                   typename ParameterType< P >::PassingType value )
  : PropertyMessageBase( updateManager ),
    mSceneObject( const_cast< PropertyOwner* >( sceneObject ) ),
    mProperty( const_cast< AnimatableProperty<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:

  PropertyOwner* mSceneObject;
  AnimatableProperty<P>* mProperty;
  MemberFunction mMemberFunction;
  typename ParameterType< P >::HolderType mParam;
};

/**
 * Templated message which bakes a property.
 */
template< typename P >
class PropertyComponentMessage : public PropertyMessageBase
{
public:

  typedef void(AnimatableProperty<P>::*MemberFunction)( BufferIndex, float );

  /**
   * Send a message.
   * @note The scene object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] sceneObject The property owner scene object
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
   */
  static void Send( UpdateManager& updateManager,
                    const PropertyOwner* sceneObject,
                    const AnimatableProperty<P>* property,
                    MemberFunction member,
                    float value )
  {
    // Reserve some memory inside the message queue
    unsigned int* slot = updateManager.GetEventToUpdate().ReserveMessageSlot( sizeof( PropertyComponentMessage ) );

    // Construct message in the message queue memory; note that delete should not be called on the return value
    new (slot) PropertyComponentMessage( updateManager, sceneObject, property, member, value );
  }

  /**
   * Virtual destructor
   */
  virtual ~PropertyComponentMessage()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex updateBufferIndex )
  {
    (mProperty->*mMemberFunction)( updateBufferIndex, mParam );
  }

private:

  /**
   * Create a message.
   * @note The scene object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager The update-manager.
   * @param[in] sceneObject The property owner scene object
   * @param[in] property The property to bake.
   * @param[in] member The member function of the object.
   * @param[in] value The new value of the X,Y,Z or W component.
  */
  PropertyComponentMessage( UpdateManager& updateManager,
                            const PropertyOwner* sceneObject,
                            const AnimatableProperty<P>* property,
                            MemberFunction member,
                            float value )
  : PropertyMessageBase( updateManager ),
    mSceneObject( const_cast< PropertyOwner* >( sceneObject ) ),
    mProperty( const_cast< AnimatableProperty<P>* >( property ) ),
    mMemberFunction( member ),
    mParam( value )
  {
  }

private:
  PropertyOwner* mSceneObject;
  AnimatableProperty<P>* mProperty;
  MemberFunction mMemberFunction;
  float mParam;
};


// Messages for PropertyOwner

inline void InstallCustomPropertyMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, PropertyBase* property )
{
  typedef MessageValue1< PropertyOwner, OwnerPointer<PropertyBase> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::InstallCustomProperty, property );
}

inline void ApplyConstraintMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, ConstraintBase& constraint )
{
  typedef MessageValue1< PropertyOwner, OwnerPointer<ConstraintBase> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::ApplyConstraint, &constraint );
}

inline void RemoveConstraintMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, const ConstraintBase& constConstraint )
{
  // The update-thread can modify this object.
  ConstraintBase& constraint = const_cast< ConstraintBase& >( constConstraint );

  typedef MessageValue1< PropertyOwner, ConstraintBase* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &PropertyOwner::RemoveConstraint, &constraint );
}

inline void AddUniformMapMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, UniformPropertyMapping* map )
{
  typedef MessageValue1< PropertyOwner, OwnerPointer< UniformPropertyMapping > > LocalType;
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );
  new (slot) LocalType( &owner, &PropertyOwner::AddUniformMapping, map );
}

inline void RemoveUniformMapMessage( EventToUpdate& eventToUpdate, const PropertyOwner& owner, const std::string& uniformName )
{
  typedef MessageValue1< PropertyOwner, std::string > LocalType;
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );
  new (slot) LocalType( &owner, &PropertyOwner::RemoveUniformMapping, uniformName );
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_OWNER_MESSAGES_H__
