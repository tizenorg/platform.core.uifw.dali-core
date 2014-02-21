#ifndef __DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H__
#define __DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H__

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
#include <dali/public-api/animation/animation.h>
#include <dali/internal/update/animation/scene-graph-animator.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Animation;

typedef OwnerContainer< Animation* > AnimationContainer;

typedef AnimationContainer::Iterator AnimationIter;
typedef AnimationContainer::ConstIterator AnimationConstIter;

/**
 * Animations are used to change the properties of scene graph objects, as part of a scene
 * managers "update" phase. An animation is a container of Animator objects; the actual setting
 * of object values is done by the animators.
 */
class Animation
{
public:

  typedef Dali::Animation::EndAction EndAction;

  enum State
  {
    Stopped,
    Playing,
    Paused,
    Destroyed
  };

  /**
   * Construct a new Animation.
   * @param[in] durationSeconds The duration of the animation in seconds.
   * @param[in] isLooping Whether the animation will loop.
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] destroyAction The action to perform when the animation is destroyed.
   * @return A new Animation
   */
  static Animation* New( float durationSeconds, bool isLooping, EndAction endAction, EndAction destroyAction )
  {
    return new Animation( durationSeconds, isLooping, endAction, destroyAction );
  }

  /**
   * Virtual destructor
   */
  virtual ~Animation();

  /**
   * Set the duration of an animation.
   * @pre durationSeconds must be greater than zero.
   * @param[in] durationSeconds The duration in seconds.
   */
  void SetDuration(float durationSeconds);

  /**
   * Retrieve the duration of the animation.
   * @return The duration in seconds.
   */
  float GetDuration() const
  {
    return mDurationSeconds;
  }

  /**
   * Set whether the animation will loop.
   * @param[in] looping True if the animation will loop.
   */
  void SetLooping(bool looping);

  /**
   * Query whether the animation will loop.
   * @return True if the animation will loop.
   */
  bool IsLooping() const
  {
    return mLooping;
  }

  /**
   * Set the end action of the animation.
   * @param[in] action The end action.
   */
  void SetEndAction(EndAction action);

  /**
   * Retrieve the action performed when the animation ends.
   * @return The end action.
   */
  EndAction GetEndAction()
  {
    return mEndAction;
  }

  /**
   * Set the destroy action of the animation.
   * This action is performed during the next update when
   * the animation is destroyed.
   * @param[in] action The destroy action.
   */
  void SetDestroyAction(EndAction action);

  /**
   * Retrieve the action performed when the animation is destroyed.
   * @return The destroy action.
   */
  EndAction GetDestroyAction()
  {
    return mDestroyAction;
  }

  /**
   * Play the animation.
   */
  void Play();

  /**
   * Pause the animation.
   */
  void Pause();

  /**
   * Stop the animation.
   * @param[in] bufferIndex The buffer to update when mEndAction == Bake.
   * @return True if the animation has finished (otherwise it wasn't playing)
   */
  bool Stop(BufferIndex bufferIndex);

  /**
   * Called shortly before the animation is destroyed.
   * @param[in] bufferIndex The buffer to update when mDestroyAction == Bake.
   */
  void OnDestroy(BufferIndex bufferIndex);

  /**
   * Query whether the animation is playing, paused or stopped.
   * Note that even when paused, the Update() method should be called,
   * since the current progress must be reapplied each frame.
   */
  State GetState() const
  {
    return mState;
  }

  /**
   * Retrive a count of the number of times the animation has been played to completion.
   * This can be used to emit "Finised" signals from the public-api
   */
  int GetPlayCount() const
  {
    return mPlayCount;
  }

  /**
   * Add a newly created animator.
   * Animators are automatically removed, when orphaned from an animatable scene object.
   * @param[in] animator The animator to add.
   * @param[in] propertyOwner The scene-object that owns the animatable property.
   * @post The animator is owned by this animation.
   */
  void AddAnimator( AnimatorBase* animator, PropertyOwner* propertyOwner );

  /**
   * Retrieve the animators from an animation.
   * @return The container of animators.
   */
  AnimatorContainer& GetAnimators()
  {
    return mAnimators;
  }

  /**
   * This causes the animators to change the properties of objects in the scene graph.
   * @pre The animation is playing or paused.
   * @param[in] bufferIndex The buffer to update.
   * @param[in] elapsedSeconds The time elapsed since the previous frame.
   * @return True if the animation has finished.
   */
  bool Update(BufferIndex bufferIndex, float elapsedSeconds);


protected:

  /**
   * Protected constructor. See New()
   */
  Animation( float durationSeconds, bool isLooping, EndAction endAction, EndAction destroyAction );


private:

  /**
   * Helper for Update, also used to bake when the animation is stopped or destroyed.
   * @param[in] bufferIndex The buffer to update.
   * @param[in] bake True if the final result should be baked.
   */
  void UpdateAnimators(BufferIndex bufferIndex, bool bake);

  // Undefined
  Animation(const Animation&);

  // Undefined
  Animation& operator=(const Animation& rhs);

protected:

  float mDurationSeconds;
  bool mLooping;
  EndAction mEndAction;
  EndAction mDestroyAction;

  State mState;
  float mElapsedSeconds;
  int mPlayCount;

  AnimatorContainer mAnimators;
};

}; //namespace SceneGraph

// value types used by messages
template <> struct ParameterType< Dali::Animation::EndAction > : public BasicType< Dali::Animation::EndAction > {};

namespace SceneGraph
{

// Messages for Animation

inline void SetDurationMessage( EventToUpdate& eventToUpdate, const Animation& animation, float durationSeconds )
{
  typedef MessageValue1< Animation, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetDuration, durationSeconds );
}

inline void SetLoopingMessage( EventToUpdate& eventToUpdate, const Animation& animation, bool looping )
{
  typedef MessageValue1< Animation, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetLooping, looping );
}

inline void SetEndActionMessage( EventToUpdate& eventToUpdate, const Animation& animation, Dali::Animation::EndAction action )
{
  typedef MessageValue1< Animation, Dali::Animation::EndAction > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetEndAction, action );
}

inline void SetDestroyActionMessage( EventToUpdate& eventToUpdate, const Animation& animation, Dali::Animation::EndAction action )
{
  typedef MessageValue1< Animation, Dali::Animation::EndAction > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetDestroyAction, action );
}

inline void PlayAnimationMessage( EventToUpdate& eventToUpdate, const Animation& animation )
{
  typedef Message< Animation > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::Play );
}

inline void PauseAnimationMessage( EventToUpdate& eventToUpdate, const Animation& animation )
{
  typedef Message< Animation > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::Pause );
}

inline void AddAnimatorMessage( EventToUpdate& eventToUpdate, const Animation& animation, AnimatorBase& animator, const PropertyOwner& owner )
{
  typedef MessageValue2< Animation, OwnerPointer<AnimatorBase>, PropertyOwner* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::AddAnimator, &animator, const_cast<PropertyOwner*>( &owner ) );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H__
