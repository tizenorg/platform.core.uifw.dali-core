#ifndef __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
#define __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/size-negotiation/relayout-controller.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <dali/public-api/common/memory-pool-object-allocator.h>

namespace Dali
{

class RelayoutController;

namespace Internal
{

class Stage;

/**
 * @copydoc Dali::Internal::RelayoutController
 */
class RelayoutController : public Dali::BaseObject, public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * We should only create a unique instance.
   */
  RelayoutController();


  /**
   * Destructor
   */
  virtual ~RelayoutController();

  /**
   * @brief Get the singleton of RelayoutController object.
   *
   * @return A handle to the RelayoutController control.
   */
  static RelayoutController* Get();

  /**
   * @brief Request to relayout all actors dependent on the given actor.
   *
   * This flags the actor and all actors below it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on.
   * @param[in] dimension The dimension(s) to request the relayout on. Defaults to all dimensions.
   */
  void RequestRelayout( Dali::Actor& actor, Dimension dimension = ALL_DIMENSIONS );

  /**
   * @brief Request to relayout of all actors in the sub-tree below the given actor.
   *
   * This flags the actor and all actors below it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on.
   */
  void RequestRelayoutTree( Dali::Actor& actor );

  /**
   * @brief Relayouts all actors that have been marked as dirty
   */
  void Relayout();

  /**
   * Conenct to the stage OnApplicationSceneCreated signal
   */
  void OnStageCreated( IntrusivePtr<Stage> stage );

public: // CALLBACKS

  /**
   * @brief Callback raised after the application creates the scene
   */
  void OnApplicationSceneCreated();

  /**
   * @brief Callback for when an object is destroyed
   *
   * @param[in] object The object being destroyed
   */
  void OnObjectDestroyed( const Dali::RefObject* object );

private:

  /**
   * @brief Request for relayout. Relays out whole scene.
   */
  void Request();

  /**
   * @brief Add actor to request list
   *
   * @param[in] actor The root of the sub tree to add
   */
  void AddRequest( Dali::Actor& actor );

  /**
   * @brief Remove actor from request list
   *
   * @param[in] actor The root of the sub tree to remove
   */
  void RemoveRequest( Dali::Actor& actor );

  /**
   * @brief Resets the relayout controller.
   */
  void Reset();

  /**
   * @brief Disconnect the Relayout() method from the Stage::EventProcessingFinishedSignal().
   */
  void Disconnect();

  /**
   * @brief Propagate dirty layout flags to actor and all sub-actors. This will stop propagating when a dirty actor
   * is found.
   *
   * @param[in] actor The actor to propagate on
   * @param[in] dimension The dimension to propagate on
   * @param[in] topOfSubTreeStack The top of the sub tree that this actor is in
   * @param[in] potentialRedundantSubRoots Actors collected as potentially already being included in relayout
   */
  void PropagateAll( Dali::Actor& actor, Dimension dimension, Dali::ActorContainer& topOfSubTreeStack, Dali::ActorContainer& potentialRedundantSubRoots );

  /**
   * Queue an actor on the relayout container
   *
   * @param[in] actor The actor to be queued
   * @param[in] actors The container to add the actor to
   * @param[in] size The size that this actor should be
   */
  void QueueActor( Dali::Actor& actor, RelayoutContainer& actors, Vector2 size );

  // Undefined
  RelayoutController(const RelayoutController&);
  RelayoutController& operator=(const RelayoutController&);

private:

  typedef Dali::Vector< BaseObject* > RawActorList;

  MemoryPoolObjectAllocator< RelayoutContainer::RelayoutInfo > mRelayoutInfoAllocator;

  RawActorList mDirtyLayoutSubTrees;    ///< List of roots of sub trees that are dirty
  RelayoutContainer mRelayoutStack;     ///< Stack for relayouting
  RelayoutContainer mSizeContainer;     ///< Size container
  bool mRelayoutConnection : 1;         ///< Whether EventProcessingFinishedSignal signal is connected.
  bool mRelayoutFlag : 1;               ///< Relayout flag to avoid unnecessary calls
  bool mEnabled : 1;                    ///< Initially disabled. Must be enabled at some point.

};

} // namespace Internal


inline Internal::RelayoutController& GetImpl(Dali::RelayoutController& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::RelayoutController&>(handle);
}

inline const Internal::RelayoutController& GetImpl(const Dali::RelayoutController& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::RelayoutController&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_RELAYOUT_CONTROLLER_IMPL_H__
