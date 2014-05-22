#ifndef __DALI_INTERNAL_STAGE_H__
#define __DALI_INTERNAL_STAGE_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/common/view-mode.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/render-tasks/render-task.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-notifier.h>
#endif

namespace Dali
{

struct Vector2;

namespace Integration
{
class SystemOverlay;
class DynamicsFactory;
}

namespace Internal
{

namespace SceneGraph
{
class UpdateManager;
}

class AnimationPlaylist;
class PropertyNotificationManager;
class Layer;
class LayerList;
class SystemOverlay;
class CameraActor;
class RenderTaskList;

/**
 * Implementation of Stage
 */
class Stage : public BaseObject, public RenderTaskDefaults
{
public:

  /**
   * Create the stage
   * @param[in] playlist for animations
   * @param[in] propertyNotificationManager
   * @param[in] updateManager
   * @param[in] notificationManager
   */
  static StagePtr New( AnimationPlaylist& playlist,
                       PropertyNotificationManager& propertyNotificationManager,
                       SceneGraph::UpdateManager& updateManager,
                       NotificationManager& notificationManager );

  /**
   * Initialize the stage.
   */
  void Initialize();

  /**
   * Uninitialize the stage.
   */
  void Uninitialize();

  /**
   * @copydoc Dali::Stage::GetCurrent()
   */
  static StagePtr GetCurrent();

  /**
   * @copydoc Dali::Stage::IsInstalled().
   */
  static bool IsInstalled();

  /**
   * @copydoc Dali::Stage::GetObjectRegistry()
   */
  ObjectRegistry& GetObjectRegistry();

  /**
   * Retrieve the root actor (not publically accessible).
   * @return The root actor.
   */
  Layer& GetRootActor();

  /**
   * Retrieve the UpdateManager associated with this Stage
   * @return The UpdateManager.
   */
  SceneGraph::UpdateManager& GetUpdateManager();

  /**
   * Helper for actors, to retrieve the current Event buffer index.
   * @return The buffer index.
   */
  BufferIndex GetEventBufferIndex() const
  {
    // inlined as its called often from event thread
    return mUpdateManager.GetEventBufferIndex();
  }

  /**
   * Retrieve the interface for accessing update-thread data.
   * @return The EventToUpdate interface.
   */
  EventToUpdate& GetUpdateInterface();

  /**
   * Returns the animation playlist.
   * @return reference to the animation playlist.
   */
  AnimationPlaylist& GetAnimationPlaylist();

  /**
   * Returns the property notification manager.
   * @return reference to the property notification manager.
   */
  PropertyNotificationManager& GetPropertyNotificationManager();

  // Root actor accessors

  /**
   * @copydoc Dali::Stage::Add()
   */
  void Add( Actor& actor );

  /**
   * @copydoc Dali::Stage::Remove()
   */
  void Remove( Actor& actor );

  /**
   * Sets the size of the stage and indirectly, the root actor.
   * @param [in] width  The new width.
   * @param [in] height The new height.
   */
  void SetSize( float width, float height );

  /**
   * Returns the size of the Stage in pixels as a Vector.
   * The x component will be the width of the Stage in pixels
   * The y component will be the height of the Stage in pixels
   * @return The size of the Stage as a Vector.
   */
  Vector2 GetSize() const;

  /**
   * @copydoc Dali::Stage::GetRenderTaskList()
   */
  RenderTaskList& GetRenderTaskList() const;

  /**
   * Create a default camera actor
   */
  void CreateDefaultCameraActor();

  /**
   * From RenderTaskDefaults; retrieve the default root actor.
   * @return The default root actor.
   */
  virtual Actor& GetDefaultRootActor();

  /**
   * From RenderTaskDefaults; retrieve the default camera actor.
   * @return The default camera actor.
   */
  virtual CameraActor& GetDefaultCameraActor();

  // Layers

  /**
   * @copydoc Dali::Stage::GetLayerCount()
   */
  unsigned int GetLayerCount() const;

  /**
   * @copydoc Dali::Stage::GetLayer()
   */
  Dali::Layer GetLayer( unsigned int depth ) const;

  /**
   * @copydoc Dali::Stage::GetRootLayer()
   */
  Dali::Layer GetRootLayer() const;

  /**
   * Retrieve the ordered list of on-stage layers.
   * @return The layer-list.
   */
  LayerList& GetLayerList();

  // System-level overlay actors

  /**
   * @copydoc Dali::Integration::Core::GetSystemOverlay()
   */
  Integration::SystemOverlay& GetSystemOverlay();

  /**
   * Retrieve the internal implementation of the SystemOverlay.
   * @return The implementation, or NULL if this has never been requested from Integration API.
   */
  SystemOverlay* GetSystemOverlayInternal();

  // Stereoscopy

  /**
   * @copydoc Dali::Integration::Core::SetViewMode()
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * @copydoc Dali::Integration::Core::GetViewMode()
   */
  ViewMode GetViewMode() const;

  /**
   * @copydoc Dali::Integration::Core::SetStereoBase()
   */
  void SetStereoBase( float stereoBase );

  /**
   * @copydoc Dali::Integration::Core::GetStereoBase()
   */
  float GetStereoBase() const;

  // Keyboard stuff

  /**
   * As one virtual keyboard per stage, the stage will hold a pointer to the Actor currently
   * set to receive keyboard input.
   * @param[in] actor to receive keyboard input
   */
  void SetKeyboardFocusActor( Actor* actor );

  /**
   * Get the actor that is currently set to receive keyboard inputs
   * @return Pointer to the actor set to receive keyboard inputs.
   */
  Actor* GetKeyboardFocusActor() const;

  /**
   * Removes the given actor from keyboard focus so it will no longer receive key events from keyboard.
   * @param [in] actor which should be removed from focus.
   */
  void RemoveActorFromKeyFocus( Actor* actor );

  // Misc

  /**
   * @copydoc Dali::Stage::SetBackgroundColor
   */
  void SetBackgroundColor(Vector4 color);

  /**
   * @copydoc Dali::Stage::GetBackgroundColor
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @copydoc Dali::Stage::GetDpi
   */
  Vector2 GetDpi() const;

  /**
   * Sets horizontal and vertical pixels per inch value that is used by the display
   * @param[in] dpi Horizontal and vertical dpi value
   */
  void SetDpi( Vector2 dpi );

#ifdef DYNAMICS_SUPPORT

  /**
   * Return the Dynamics Simulation Notifier object
   * @return The Dynamics Simulation Notifier object
   */
  DynamicsNotifier& GetDynamicsNotifier();

  /**
   * @copydoc Dali::Stage::InitializeDynamics
   */
  DynamicsWorldPtr InitializeDynamics(DynamicsWorldConfigPtr config);

  /**
   * @copydoc Dali::Stage::GetDynamicsWorld
   */
  DynamicsWorldPtr GetDynamicsWorld();

  /**
   * @copydoc Dali::Stage::TerminateDynamics
   */
  void TerminateDynamics();

#endif // DYNAMICS_SUPPORT

  NotificationManager& GetNotificationManager()
  {
    return mNotificationManager;
  }

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

  /**
   * Used by the EventProcessor to emit key event signals.
   * @param[in] event The key event.
   */
  void EmitKeyEventSignal(const KeyEvent& event);

  /**
   * Emits the event processing finished signal.
   *
   * @see Dali::Stage::SignalEventProcessingFinished()
   */
  void EmitEventProcessingFinishedSignal();

  /**
   * Emits the touched signal.
   * @param[in] touch The touch event details.
   */
  void EmitTouchedSignal( const TouchEvent& touch );

  /**
   * @copydoc Dali::Stage::KeyEventSignal()
   */
  Dali::Stage::KeyEventSignalV2& KeyEventSignal();

  /**
   * @copydoc Dali::Stage::SignalEventProcessingFinished()
   */
  Dali::Stage::EventProcessingFinishedSignalV2& EventProcessingFinishedSignal();

  /**
    * @copydoc Dali::Stage::TouchedSignal()
    */
  Dali::Stage::TouchedSignalV2& TouchedSignal();

private:

  /**
   * Protected constructor; see also Stage::New()
   */
  Stage( AnimationPlaylist& playlist,
         PropertyNotificationManager& propertyNotificationManager,
         SceneGraph::UpdateManager& updateManager,
         NotificationManager& notificationManager );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Stage();

private:

  // For 'Fire and forget' animation support
  AnimationPlaylist& mAnimationPlaylist;

  PropertyNotificationManager& mPropertyNotificationManager;

  SceneGraph::UpdateManager& mUpdateManager;

  NotificationManager& mNotificationManager;

  // The Actual size of the stage.
  Vector2 mSize;

  // Cached for public GetBackgroundColor()
  Vector4 mBackgroundColor;

  LayerPtr mRootLayer;

  // Ordered list of currently on-stage layers
  OwnerPointer<LayerList> mLayerList;

  IntrusivePtr<CameraActor> mDefaultCamera;

  ViewMode mViewMode;
  float mStereoBase;

  Vector2 mDpi;

  // The object registry
  ObjectRegistryPtr mObjectRegistry;

#ifdef DYNAMICS_SUPPORT

  DynamicsNotifier mDynamicsNotifier;

  // The Dynamics simulation world object
  Integration::DynamicsFactory* mDynamicsFactory;   // Not owned pointer to DynamicsFactory (PlatformAbstraction will clean up)
  DynamicsWorldPtr mDynamicsWorld;

#endif // DYNAMICS_SUPPORT

  // The list of render-tasks
  IntrusivePtr<RenderTaskList> mRenderTaskList;

  Dali::RenderTask mRightRenderTask;
  IntrusivePtr<CameraActor> mRightCamera;
  Dali::RenderTask mLeftRenderTask;
  IntrusivePtr<CameraActor> mLeftCamera;

  Integration::SystemOverlay* mSystemOverlay; ///< SystemOverlay stage access

  // The key event signal
  Dali::Stage::KeyEventSignalV2                 mKeyEventSignalV2;

  // The event processing finished signal
  Dali::Stage::EventProcessingFinishedSignalV2  mEventProcessingFinishedSignalV2;

  // The touched signal
  Dali::Stage::TouchedSignalV2                  mTouchedSignalV2;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Stage& GetImplementation(Dali::Stage& stage)
{
  DALI_ASSERT_ALWAYS( stage && "Stage handle is empty" );

  BaseObject& handle = stage.GetBaseObject();

  return static_cast<Internal::Stage&>(handle);
}

inline const Internal::Stage& GetImplementation(const Dali::Stage& stage)
{
  DALI_ASSERT_ALWAYS( stage && "Stage handle is empty" );

  const BaseObject& handle = stage.GetBaseObject();

  return static_cast<const Internal::Stage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_STAGE_H__
