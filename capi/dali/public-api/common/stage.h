#ifndef __DALI_STAGE_H__
#define __DALI_STAGE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Stage;
}

class Actor;
class Layer;
class ObjectRegistry;
class RenderTaskList;
struct Vector2;
struct Vector3;
struct Vector4;
class DynamicsWorld;
class DynamicsWorldConfig;
struct KeyEvent;
struct TouchEvent;

/**
 * The Stage is a top-level object used for displaying a tree of Actors.
 * Multiple stage/window support is not currently provided.
 */
class Stage : public BaseHandle
{
public:

  typedef SignalV2< void (const KeyEvent&)> KeyEventSignalV2;
  typedef SignalV2< void () > EventProcessingFinishedSignalV2;
  typedef SignalV2< void (const TouchEvent&)> TouchedSignalV2;

  static const Vector4 DEFAULT_BACKGROUND_COLOR; ///< Default black background.
  static const Vector4 DEBUG_BACKGROUND_COLOR;   ///< Green background, useful when debugging.

  // Signal Names
  static const char* const SIGNAL_KEY_EVENT;
  static const char* const SIGNAL_EVENT_PROCESSING_FINISHED;
  static const char* const SIGNAL_TOUCHED;

  /**
   * Allows the creation of an empty stage handle.  To retrieve the current stage,
   * this handle can be set using Stage::GetCurrent().
   */
  Stage();

  /**
   * Get the current Stage.
   * @return The current stage or an empty handle if Core has not been created or has been already destroyed.
   */
  static Stage GetCurrent();

  /**
   * Query whether the Stage exists; this should only return false during or after destruction of Dali core.
   * @return True when it's safe to call Stage::GetCurrent().
   */
  static bool IsInstalled();

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Stage();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  // Containment

  /**
   * Adds a child Actor to the Stage. The child will be referenced.
   * @pre The actor has been initialized.
   * @pre The actor does not have a parent.
   * @param [in] actor The child.
   */
  void Add(Actor& actor);

  /**
   * Removes a child Actor from the Stage. The child will be unreferenced.
   * @pre The actor has been added to the stage.
   * @param [in] actor The child.
   */
  void Remove(Actor& actor);

  /**
   * Returns the size of the Stage in pixels as a Vector.
   * The x component will be the width of the Stage in pixels
   * The y component will be the height of the Stage in pixels
   * The z component will be the distance between far and near planes
   * @return The size of the Stage as a Vector.
   */
  Vector2 GetSize() const;

  // Render Tasks

  /**
   * Retrieve the list of render-tasks.
   * @return A valid handle to a RenderTaskList.
   */
  RenderTaskList GetRenderTaskList() const;

  // Layers

  /**
   * Query the number of on-stage layers.
   * Note that a default layer is always provided (count >= 1).
   * @return The number of layers.
   */
  unsigned int GetLayerCount() const;

  /**
   * Retrieve the layer at a specified depth.
   * @pre depth is less than layer count; see GetLayerCount().
   * @param[in] depth The depth.
   * @return The layer found at the given depth.
   */
  Layer GetLayer(unsigned int depth) const;

  /**
   * Returns the Stage's Root Layer.
   * @return The root layer.
   */
  Layer GetRootLayer() const;

  // Background color

  /**
   * Set the background color of the stage.
   * @param[in] color The new background color.
   */
  void SetBackgroundColor(Vector4 color);

  /**
   * Retrieve the background color of the stage.
   * @return The background color.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * Retrieve the DPI of the display device to which the stage is connected.
   * @return the horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

  /**
   * Get the Object registry.
   * @return The object registry.
   */
  ObjectRegistry GetObjectRegistry() const;

  // Dynamics

  /**
   * Initialise the dynamics simulation and create a DynamicsWorld object.
   * Only one instance of DynamicsWorld will be created, so calling this method multiple times
   * will return the same DynamicsWorld object.
   * @param[in] config A DynamicsWorldConfig object describing the required capabilities of the dynamics world.
   * @return A handle to the world object of the dynamics simulation, or an empty handle if Dynamics capable
   *         of supporting the requirement in config is not available on the platform.
   */
  DynamicsWorld InitializeDynamics(DynamicsWorldConfig config);

  /**
   * Get a handle to the world object of the dynamics simulation.
   * @return A handle to the world object of the dynamics simulation
   */
  DynamicsWorld GetDynamicsWorld();

  /**
   * Terminate the dynamics simulation.
   * Calls Actor::DisableDynamics on all dynamics enabled actors,
   * all handles to any DynamicsBody or DynamicsJoint objects held by applications
   * will become detached from their actors and the simulation therefore should be discarded.
   */
  void TerminateDynamics();

  // Rendering

  /**
   * Keep rendering for at least the given amount of time.
   * By default Dali will stop rendering when no Actor positions are being set, and when no animations are running etc.
   * This method is useful to force screen refreshes e.g. when updating a NativeImage.
   * @param durationSeconds to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering( float durationSeconds );

  // Signals

  /**
   * This signal is emitted when key event is received.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const KeyEvent& event);
   * @endcode
   * @return The signal to connect to.
   */
  KeyEventSignalV2& KeyEventSignal();

  /**
   * This signal is emitted just after the event processing is finished.
   * @return The signal to connect to.
   */
  EventProcessingFinishedSignalV2& EventProcessingFinishedSignal();

  /**
   * This signal is emitted when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
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
  TouchedSignalV2& TouchedSignal();

public: // Not intended for application developers

  /**
   * This constructor is used by Dali GetCurrent() methods
   * @param [in] stage A pointer to a Dali resource
   */
  explicit DALI_INTERNAL Stage(Internal::Stage* stage);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_STAGE_H__
