#ifndef __DALI_STAGE_H__
#define __DALI_STAGE_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Stage;
}

class Actor;
class Layer;
class RenderTaskList;
struct Vector2;
struct Vector3;
struct Vector4;
class DynamicsWorld;
class DynamicsWorldConfig;
struct KeyEvent;
struct TouchEvent;

/**
 * @brief The Stage is a top-level object used for displaying a tree of Actors.
 *
 * Multiple stage/window support is not currently provided.
 */
class DALI_IMPORT_API Stage : public BaseHandle
{
public:

  typedef Signal< void (const KeyEvent&)> KeyEventSignalType;  ///< Key event signal type
  typedef Signal< void () > EventProcessingFinishedSignalType; ///< Event Processing finished signal type
  typedef Signal< void (const TouchEvent&)> TouchedSignalType; ///< Touched signal type
  typedef Signal< void () > ContextStatusSignal;             ///< Context status signal type
  typedef Signal< void () > SceneCreatedSignalType;            ///< Scene created signal type

  static const Vector4 DEFAULT_BACKGROUND_COLOR; ///< Default black background.
  static const Vector4 DEBUG_BACKGROUND_COLOR;   ///< Green background, useful when debugging.

  // Signal Names
  static const char* const SIGNAL_KEY_EVENT; ///< name "key-event"
  static const char* const SIGNAL_EVENT_PROCESSING_FINISHED; ///< name "event-processing-finished"
  static const char* const SIGNAL_TOUCHED; ///< name "touched"
  static const char* const SIGNAL_CONTEXT_LOST; ///< name "context-lost"
  static const char* const SIGNAL_CONTEXT_REGAINED; ///< name "context-regained"

  /**
   * @brief Allows the creation of an empty stage handle.
   *
   * To retrieve the current stage, this handle can be set using Stage::GetCurrent().
   */
  Stage();

  /**
   * @brief Get the current Stage.
   *
   * @return The current stage or an empty handle if Core has not been created or has been already destroyed.
   */
  static Stage GetCurrent();

  /**
   * @brief Query whether the Stage exists; this should only return false during or after destruction of Dali core.
   *
   * @return True when it's safe to call Stage::GetCurrent().
   */
  static bool IsInstalled();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Stage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Stage(const Stage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Stage& operator=(const Stage& rhs);

  // Containment

  /**
   * @brief Adds a child Actor to the Stage.
   *
   * The child will be referenced.
   * @pre The actor has been initialized.
   * @pre The actor does not have a parent.
   * @param [in] actor The child.
   */
  void Add(Actor& actor);

  /**
   * @brief Removes a child Actor from the Stage.
   *
   * The child will be unreferenced.
   * @pre The actor has been added to the stage.
   * @param [in] actor The child.
   */
  void Remove(Actor& actor);

  /**
   * @brief Returns the size of the Stage in pixels as a Vector.
   *
   * The x component will be the width of the Stage in pixels
   * The y component will be the height of the Stage in pixels
   * The z component will be the distance between far and near planes
   * @return The size of the Stage as a Vector.
   */
  Vector2 GetSize() const;

  // Render Tasks

  /**
   * @brief Retrieve the list of render-tasks.
   *
   * @return A valid handle to a RenderTaskList.
   */
  RenderTaskList GetRenderTaskList() const;

  // Layers

  /**
   * @brief Query the number of on-stage layers.
   *
   * Note that a default layer is always provided (count >= 1).
   * @return The number of layers.
   */
  unsigned int GetLayerCount() const;

  /**
   * @brief Retrieve the layer at a specified depth.
   *
   * @pre depth is less than layer count; see GetLayerCount().
   * @param[in] depth The depth.
   * @return The layer found at the given depth.
   */
  Layer GetLayer(unsigned int depth) const;

  /**
   * @brief Returns the Stage's Root Layer.
   *
   * @return The root layer.
   */
  Layer GetRootLayer() const;

  // Background color

  /**
   * @brief Set the background color of the stage.
   *
   * @param[in] color The new background color.
   */
  void SetBackgroundColor(Vector4 color);

  /**
   * @brief Retrieve the background color of the stage.
   *
   * @return The background color.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Retrieve the DPI of the display device to which the stage is connected.
   *
   * @return the horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

  // Dynamics

  /**
   * @brief Initialise the dynamics simulation and create a DynamicsWorld object.
   *
   * Only one instance of DynamicsWorld will be created, so calling this method multiple times
   * will return the same DynamicsWorld object.
   * @param[in] config A DynamicsWorldConfig object describing the required capabilities of the dynamics world.
   * @return A handle to the world object of the dynamics simulation, or an empty handle if Dynamics capable
   *         of supporting the requirement in config is not available on the platform.
   */
  DynamicsWorld InitializeDynamics(DynamicsWorldConfig config);

  /**
   * @brief Get a handle to the world object of the dynamics simulation.
   *
   * @return A handle to the world object of the dynamics simulation
   */
  DynamicsWorld GetDynamicsWorld();

  /**
   * @brief Terminate the dynamics simulation.
   *
   * Calls Actor::DisableDynamics on all dynamics enabled actors,
   * all handles to any DynamicsBody or DynamicsJoint objects held by applications
   * will become detached from their actors and the simulation therefore should be discarded.
   */
  void TerminateDynamics();

  // Rendering

  /**
   * @brief Keep rendering for at least the given amount of time.
   *
   * By default Dali will stop rendering when no Actor positions are being set, and when no animations are running etc.
   * This method is useful to force screen refreshes e.g. when updating a NativeImage.
   * @param durationSeconds to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering( float durationSeconds );

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const KeyEvent& event);
   * @endcode
   * @return The signal to connect to.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted just after the event processing is finished.
   *
   * @return The signal to connect to.
   */
  EventProcessingFinishedSignalType& EventProcessingFinishedSignal();

  /**
   * @brief This signal is emitted when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   *
   * If there are multiple touch points, then this will be emitted when the first touch occurs and
   * then when the last finger is lifted.
   * An interrupted event will also be emitted.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const TouchEvent& event);
   * @endcode
   *
   * @note Motion events are not emitted.
   * @return The touch signal to connect to.
   */
  TouchedSignalType& TouchedSignal();

  /**
   * @brief This signal is emitted when the GL context is lost (Platform specific behaviour).
   *
   * If the application is responsible for handling context loss, it should listen to
   * this signal and tear down UI components when recieved.
   * @return The ContextLost signal to connect to.
   */
  ContextStatusSignal& ContextLostSignal();

  /**
   * @brief This signal is emitted when the GL context is regained (Platform specific
   * behaviour).
   *
   * If the application is responsible for handling context loss, it should listen to
   * this signal and rebuild UI components on receipt.
   * @return The ContextRegained signal to connect to.
   */
  ContextStatusSignal& ContextRegainedSignal();

  /**
   * @brief This signal is emitted after the initial scene is created. It will be triggered after the
   * application init signal.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName();
   * @endcode
   * @return The signal to connect to.
   */
  SceneCreatedSignalType& SceneCreatedSignal();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali GetCurrent() methods.
   *
   * @param [in] stage A pointer to a Dali resource
   */
  explicit DALI_INTERNAL Stage(Internal::Stage* stage);
};

} // namespace Dali

#endif // __DALI_STAGE_H__
