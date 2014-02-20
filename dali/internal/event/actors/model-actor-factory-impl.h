#ifndef __DALI_INTERNAL_MODEL_ACTOR_FACTORY_H__
#define __DALI_INTERNAL_MODEL_ACTOR_FACTORY_H__

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
#include <dali/public-api/modeling/model.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/animation/animation-impl.h>

namespace Dali
{

class Model;
class Entity;

namespace Internal
{

/**
 * This factory class is used to generate a tree of actors and mesh actors.
 */
class ModelActorFactory
{
public:

  /**
   * @copydoc Dali::ModelActorFactory::BuildActorTree
   */
  static ActorPtr BuildActorTree(Dali::Model& model, const std::string& name);


  /**
   * Create an animation on the actor and child actors using the named animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] animationName The name of the animation to build
   * @return A pointer to an animation.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, const std::string& animationName);

  /**
   * Create an animation on the actor and child actors using the named animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] animationName The name of the animation to build
   * @param[in] durationSeconds The duration over which to run the animation
   * @return A pointer to an animation.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, const std::string& animationName, float durationSeconds);

  /**
   * Create an animation on the actor and child actors using the named animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] animationName The name of the animation to build
   * @param[in] alpha The alpha function to apply to the overall progress
   * @return A pointer to an animation.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, const std::string& animationName, AlphaFunction alpha);

  /**
   * Create an animation on the actor and child actors using the named animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] animationName The name of the animation to build
   * @param[in] alpha The alpha function to apply to the overall progress
   * @param[in] durationSeconds The duration over which to run the animation
   * @return A pointer to an animation.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, const std::string& animationName, AlphaFunction alpha, float durationSeconds);

  /**
   * Create an animation on the actor and child actors using the indexed animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] index The index of the animation
   * @return A handle to an animation, or uninitialised.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, size_t index);

  /**
   * Create an animation on the actor and child actors using the indexed animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] index The index of the animation
   * @param[in] durationSeconds The duration over which to run the animation
   * @return A handle to an animation, or uninitialised.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, size_t index, float durationSeconds);

  /**
   * Create an animation on the actor and child actors using the indexed animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] index The index of the animation
   * @param[in] alpha The alpha function to apply to the overall progress
   * @return A handle to an animation, or uninitialised.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, size_t index, AlphaFunction alpha);

  /**
   * Create an animation on the actor and child actors using the indexed animation map.
   * If the animation map does not exist in the model, then this returns an uninitialised
   * handle.
   * @param[in] model A model resource handle
   * @param[in] rootActor An actor tree (generated by BuildActorTree) to animate
   * @param[in] index The index of the animation
   * @param[in] alpha The alpha function to apply to the overall progress
   * @param[in] durationSeconds The duration over which to run the animation
   * @return A handle to an animation, or uninitialised.
   */
  static AnimationPtr BuildAnimation(Model& model, Actor& rootActor, size_t index, AlphaFunction alpha, float durationSeconds);



private:
  /**
   * Recursively descend entity tree, creating either actors or meshActors.
   * @param[in] modelData The model data source.
   * @param[in] entity The entity to create and descend
   * @return an Actor representing the entity (May be a handle to a MeshActor)
   */
  static ActorPtr RecurseNew(ModelDataPtr modelData, Dali::Entity entity);

  /**
   * Recursive method which descends the actor tree from actor. For
   * any mesh actor found, bind it's bones, searching from the
   * rootActor. (If a mesh is affected by bones, there is no
   * requirement that they are below it's entity in the model's entity
   * tree and corresponding actor tree)
   *
   * @param[in] rootActor - the actor to search for bones
   * @param[in] actor     - the current actor to check for meshes.
   */
  static void BindBonesToMeshActors(ActorPtr rootActor, ActorPtr actor);

  /**
   * Create animation and connect each animator in the animation map to the
   * relevant actor matching entity. This uses the root actor as a point to
   * search for entity names, so it's advised to use the topmost actor generated
   * by BuildActorTree.
   *
   * @param[in] rootActor The root actor created from the model,
   * @param[in] animationData The animation map containing entity animator data.
   * @param[in] alpha The alpha function to apply to the overall progress
   * @param[in] durationSeconds The duration over which the animation will occur.
   * @return animation The generated animation
   */
  static AnimationPtr CreateAnimation(Actor& rootActor, const ModelAnimationMap* animationData, AlphaFunction alpha, float durationSeconds);

};

} // Internal
} // namespace Dali

#endif /* _DALI_INTERNAL_MODEL_ACTOR_FACTORY_H_ */
