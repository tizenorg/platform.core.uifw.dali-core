#ifndef __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__

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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

class Actor;
struct Vector2;
struct Vector4;

namespace Integration
{
struct HoverEvent;
}

namespace Internal
{

class Actor;
class Stage;

/**
 * <h3>Multi-Touch Event Processing:</h3>
 *
 * The HoverEventProcessor processes hover events and emits the Hovered signal on the hit actor (and
 * its parents).
 *
 * - Hit Testing & Hover Event Delivery are described in Dali::Actor.
 */
class HoverEventProcessor
{
public:

  /**
   * Create an event processor.
   * @param[in] stage The stage.
   */
  HoverEventProcessor( Stage& stage );

  /**
   * Non-virtual destructor; HoverEventProcessor is not a base class
   */
  ~HoverEventProcessor();

  /**
   * This function is called by the event processor whenever a hover event occurs.
   * @param[in] event The hover event that has occurred.
   */
  void ProcessHoverEvent( const Integration::HoverEvent& event );

private:

  // Undefined
  HoverEventProcessor(const HoverEventProcessor&);

  // Undefined
  HoverEventProcessor& operator=(const HoverEventProcessor& rhs);

private:

  /**
   * Stores an actor pointer and connects/disconnects to any required signals appropriately when set/unset.
   */
  struct ActorObserver : public ProxyObject::Observer
  {
  public:

    // Construction & Destruction

    /**
     * Constructor.
     */
    ActorObserver();

    /**
     * Non virtual destructor
     */
    ~ActorObserver();

    // Methods

    /**
     * Return the stored Actor pointer.
     * @return The Actor pointer.
     */
    Actor* GetActor();

    /**
     * Assignment operator.
     * This disconnects the required signals from the currently set actor and connects to the required
     * signals for the the actor specified (if set).
     */
    void SetActor( Actor* actor );

    /**
     * Resets the set actor and disconnects any connected signals.
     */
    void ResetActor();

  private:

    // Undefined
    ActorObserver( const ActorObserver& );
    ActorObserver& operator=( const ActorObserver& );

  private:

    /**
     * This will never get called as we do not observe objects that have not been added to the scene.
     * @param[in] proxy The proxy object.
     * @see ProxyObject::Observer::SceneObjectAdded()
     */
    virtual void SceneObjectAdded(ProxyObject& proxy) { }

    /**
     * This will be called when the actor is removed from the stage, we should clear and stop
     * observing it.
     * @param[in] proxy The proxy object.
     * @see ProxyObject::Observer::SceneObjectRemoved()
     */
    virtual void SceneObjectRemoved(ProxyObject& proxy);

    /**
     * This will be called when the actor is destroyed. We should clear the actor.
     * No need to stop observing as the object is being destroyed anyway.
     * @see ProxyObject::Observer::ProxyDestroyed()
     */
    virtual void ProxyDestroyed(ProxyObject& proxy);

  private:
    Actor* mActor;              ///< Raw pointer to an Actor.
    bool   mActorDisconnected;  ///< Indicates whether the actor has been disconnected from the scene
  };

  Stage& mStage; ///< Used to deliver touch events
  ActorObserver mLastPrimaryHitActor; ///< Stores the last primary point hit actor
  ActorObserver mLastConsumedActor; ///< Stores the last consumed actor
  ActorObserver mHoverStartConsumedActor; ///< Stores the hover-start consumed actor
  Dali::RenderTask mLastRenderTask; ///< The RenderTask used for the last hit actor
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__
