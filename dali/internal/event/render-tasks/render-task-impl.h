#ifndef __DALI_INTERNAL_RENDER_TASK_H__
#define __DALI_INTERNAL_RENDER_TASK_H__

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace Internal
{

class Actor;
class CameraActor;
class EventThreadServices;

namespace SceneGraph
{
class RenderTask;
class Camera;
}

class RenderTask : public Object
{
public:

  typedef Dali::RenderTask::ScreenToFrameBufferFunction ScreenToFrameBufferFunction;

  /**
   * Creates a new RenderTask.
   * @param[in] isSystemLevel Whether the render-task is on the system level task list.
   */
  static RenderTask* New( bool isSystemLevel );

  /**
   * @copydoc Dali::RenderTask::SetSourceActor()
   */
  void SetSourceActor( Actor* actor );

  /**
   * @copydoc Dali::RenderTask::GetSourceActor()
   */
  Actor* GetSourceActor() const;

  /**
   * @copydoc Dali::RenderTask::SetExclusive()
   */
  void SetExclusive( bool exclusive );

  /**
   * @copydoc Dali::RenderTask::IsExclusive()
   */
  bool IsExclusive() const;

  /**
   * @copydoc Dali::RenderTask::SetInputEnabled()
   */
  void SetInputEnabled( bool enabled );

  /**
   * @copydoc Dali::RenderTask::GetInputEnabled()
   */
  bool GetInputEnabled() const;

  /**
   * @copydoc Dali::RenderTask::SetCameraActor()
   */
  void SetCameraActor( CameraActor* cameraActor );

  /**
   * @copydoc Dali::RenderTask::GetCameraActor()
   */
  CameraActor* GetCameraActor() const;

  /**
   * @copydoc Dali::RenderTask::SetTargetFrameBuffer()
   */
  void SetTargetFrameBuffer( Dali::FrameBufferImage frameBuffer );

  /**
   * @copydoc Dali::RenderTask::GetTargetFrameBuffer
   */
  Dali::FrameBufferImage GetTargetFrameBuffer() const;

  /**
   * @copydoc Dali::RenderTask::SetScreenToFrameBufferFunction
   */
  void SetScreenToFrameBufferFunction( ScreenToFrameBufferFunction conversionFunction );

  /**
   * @copydoc Dali::RenderTask::GetScreenToFrameBufferFunction
   */
  ScreenToFrameBufferFunction GetScreenToFrameBufferFunction() const;

  /**
   * copydoc Dali::RenderTask::SetScreenToFrameBufferMappingActor
   */
  void SetScreenToFrameBufferMappingActor( Actor* mappingActor );

  /**
   * copydoc Dali::RenderTask::GetScreenToFrameBufferMAppingActor
   */
  Actor* GetScreenToFrameBufferMappingActor() const;

  /**
   * @copydoc Dali::RenderTask::SetViewportPosition
   */
  void SetViewportPosition(const Vector2& value);

  /**
   * @copydoc Dali::RenderTask::GetCurrentViewportPosition
   */
  Vector2 GetCurrentViewportPosition() const;

  /**
   * @copydoc Dali::RenderTask::SetViewportSize
   */
  void SetViewportSize(const Vector2& value);

  /**
   * @copydoc Dali::RenderTask::GetCurrentViewportSize
   */
  Vector2 GetCurrentViewportSize() const;

  /**
   * @copydoc Dali::RenderTask::SetViewport()
   */
  void SetViewport( const Viewport& viewport );

  /**
   * @param[out] viewPort instance to copy the values into
   */
  void GetViewport( Viewport& viewPort ) const;

  /**
   * @copydoc Dali::RenderTask::SetClearColor()
   */
  void SetClearColor( const Vector4& color );

  /**
   * @copydoc Dali::RenderTask::GetClearColor()
   */
  const Vector4& GetClearColor() const;

  /**
   * Set whether whether GL sync is required for native render target.
   * @param[in] requiresSync whether whether GL sync is required.
   */
  void SetSyncRequired( bool requiresSync );

  /**
   * Query whether the sync object is required for native render target.
   * @return True if the sync object is required, false otherwise.
   */
 bool IsSyncRequired() const;

  /**
   * @copydoc Dali::RenderTask::SetClearEnabled()
   */
  void SetClearEnabled( bool enabled );

  /**
   * @copydoc Dali::RenderTask::GetClearEnabled()
   */
  bool GetClearEnabled() const;

  /**
   * @copydoc Dali::RenderTask::SetCullMode()
   */
  void SetCullMode( bool mode );

  /**
   * @copydoc Dali::RenderTask::GetCullMode()
   */
  bool GetCullMode() const;

  /**
   * @copydoc Dali::RenderTask::SetRefreshRate()
   */
  void SetRefreshRate( unsigned int refreshRate );

  /**
   * @copydoc Dali::RenderTask::GetRefreshRate()
   */
  unsigned int GetRefreshRate() const;

  /**
   * Check if the render-task is hittable. If render task is offscreen, the screen coordinates may be translated.
   * @param[in,out] screenCoords The screen coordinate, which may be converted (for hit-testing actors which are rendered off-screen).
   * @return True the render-task can be used for input-handling; otherwise the output parameters are not valid.
   */
  bool IsHittable( Vector2& screenCoords ) const;

  /**
   * Translates screen coordinates to render task coordinates for offscreen render tasks
   * @param[in,out] screenCoords The screen coordinates, which may be converted (for off-screen).
   * @return false if the conversion function decides the coordinates are not inside. returns true if there is no conversion function
   */
  bool TranslateCoordinates( Vector2& screenCoords ) const;

  /**
   * Query whether the RenderTask is on the system level render-task list.
   * @return true, if on the system level task list, false otherwise.
   */
  bool IsSystemLevel() const;

  /**
   * @copydoc Dali::RenderTask::WorldToViewport()
   */
  bool WorldToViewport(const Vector3 &position, float& viewportX, float& viewportY) const;

  /**
   * @copydoc Dali::RenderTask::ViewportToLocal()
   */
  bool ViewportToLocal(Actor* actor, float viewportX, float viewportY, float &localX, float &localY) const;

public: // Used by RenderTaskList, which owns the SceneGraph::RenderTasks

  /**
   * Create the scene-graph RenderTask object.
   * @pre CreateSceneObject has not already been called.
   * @return A newly allocated scene-graph object; the caller takes ownership.
   */
  SceneGraph::RenderTask* CreateSceneObject();

  /**
   * Retrieve the scene-graph RenderTask object.
   * @return The scene-graph object, or NULL if this has been discarded.
   */
  SceneGraph::RenderTask* GetRenderTaskSceneObject();

  /**
   * Discard the scene-graph RenderTask object.
   */
  void DiscardSceneObject();

public: // Implementation of Object

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

public: //signals

  /**
   * Query whether a Finished signal should be emitted for this render-task.
   * This should only be called by NotificationManager, before signals are emitted.
   * @pre The refresh rate must be REFRESH_ONCE.
   * @post HasFinished() will return false on subsequent calls, until the render-task is processed again.
   * @return \e true if the refresh once render task has finished. Otherwise it returns \e false.
   */
  bool HasFinished();

  /**
   * Emit the Finished signal
   */
  void EmitSignalFinish();

  /**
   * @copydoc Dali::RenderTask::FinishedSignal()
   */
  Dali::RenderTask::RenderTaskSignalType& FinishedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

protected:

  /**
   * Construct a new RenderTask.
   * @param[in] isSystemLevel Whether the RenderTask is on the system level task list.
   */
  RenderTask( bool isSystemLevel );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderTask();

  /**
   * Helper class for connecting Nodes to the scene-graph RenderTask
   */
  class Connector : public Object::Observer
  {
  public:

    enum Type
    {
      SOURCE_CONNECTOR,
      CAMERA_CONNECTOR,
      MAPPING_CONNECTOR
    };

    /**
     * Create the helper class
     */
    Connector( Type type, RenderTask& renderTask );

    /**
     * Non-virtual destructor; not suitable as a base object.
     */
    ~Connector();

    /**
     * Set the actor to be observed.
     * @param[in] actor The actor to be observed.
     */
    void SetActor( Actor* actor );

    /**
     * Update the scene-graph RenderTask with a new source/camera Node.
     */
    void UpdateRenderTask();

  public: // From Object::Observer

    /**
     * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded
     */
    virtual void SceneObjectAdded( Object& object );

    /**
     * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded
     */
    virtual void SceneObjectRemoved( Object& object );

    /**
     * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed
     */
    virtual void ObjectDestroyed( Object& object );

  private:

    // Undefined
    Connector(const Connector&);

    // Undefined
    Connector& operator=(const Connector& rhs);

  public:

    const Type mType;

    RenderTask& mRenderTask;

    Actor* mActor; ///< Raw-pointer to the actor; not owned.
  };

private:

  SceneGraph::RenderTask* mSceneObject; ///< Raw-pointer to the scene-graph object; not owned.
  const SceneGraph::Camera* mCamera;    ///< Raw-pointer to camera scene-graph object; not owned.

  Connector mSourceConnector; ///< Responsible for connecting/disconnecting source Nodes
  Connector mCameraConnector; ///< Responsible for connecting/disconnecting camera Nodes
  Connector mMappingConnector; /// Responsible for connecting/disconnecting actor node, which used to mapping screen to frame buffer coordinate

  Vector4 mClearColor;       ///< Optional clear color

  unsigned int mRefreshRate; ///< Determines how often the task is processed.

  unsigned int mRefreshOnceCounter;

  Dali::FrameBufferImage mFrameBufferImage;  ///< Optional off-screen render target.

  Dali::RenderTask::ScreenToFrameBufferFunction mScreenToFrameBufferFunction; ///< Used to convert screen to frame-buffer coordinates

  bool mExclusive     : 1; ///< True if the render-task has exclusive access to the source Nodes.
  bool mInputEnabled  : 1; ///< True if the render-task should be considered for input handling.
  bool mClearEnabled  : 1; ///< True if the render-task should be clear the color buffer.
  bool mCullMode      : 1; ///< True if the render-task's actors should be culled
  bool mIsSystemLevel : 1; ///< True if the render-task is on the system level task list.
  bool mRequiresSync  : 1; ///< True if the GL sync is required to track the render of

  //Signals
  Dali::RenderTask::RenderTaskSignalType  mSignalFinished; ///< Signal emmited when the render task has been processed.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RenderTask& GetImplementation(Dali::RenderTask& task)
{
  DALI_ASSERT_ALWAYS(task && "RenderTask handle is empty");

  BaseObject& handle = task.GetBaseObject();

  return static_cast<Internal::RenderTask&>(handle);
}

inline const Internal::RenderTask& GetImplementation(const Dali::RenderTask& task)
{
  DALI_ASSERT_ALWAYS(task && "RenderTask handle is empty");

  const BaseObject& handle = task.GetBaseObject();

  return static_cast<const Internal::RenderTask&>(handle);
}

} // namespace Dali

#endif //__DALI_INTERNAL_RENDER_TASK_H__
