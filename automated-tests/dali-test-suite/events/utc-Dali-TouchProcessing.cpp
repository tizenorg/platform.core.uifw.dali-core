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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/system-overlay.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliTouchNormalProcessing, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchOutsideCameraNearFarPlanes, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchEmitEmpty, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchInterrupted, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchParentConsumer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchInterruptedParentConsumer, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchLeave, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchLeaveParentConsumer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchActorBecomesInsensitive, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchActorBecomesInsensitiveParentConsumer, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchMultipleLayers, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchMultipleRenderTasks, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchMultipleRenderTasksWithChildLayer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchOffscreenRenderTasks, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchMultipleRenderableActors, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchActorRemovedInSignal, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchActorSignalNotConsumed, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchActorUnStaged, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTouchSystemOverlayActor, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

///////////////////////////////////////////////////////////////////////////////

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled( false ),
    touchEvent(),
    touchedActor()
  {
  }

  void Reset()
  {
    functorCalled = false;

    touchEvent.time = 0u;
    touchEvent.points.clear();

    touchedActor = NULL;
  }

  bool functorCalled;
  TouchEvent touchEvent;
  Actor touchedActor;
};

// Functor that sets the data when called
struct TouchEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  TouchEventFunctor( SignalData& data, bool returnValue = true )
  : signalData( data ),
    returnValue( returnValue )
  {
  }

  bool operator()( Actor actor, const TouchEvent& touchEvent )
  {
    signalData.functorCalled = true;
    signalData.touchedActor = actor;
    signalData.touchEvent = touchEvent;

    return returnValue;
  }

  SignalData& signalData;
  bool returnValue;
};

// Functor that removes the actor when called.
struct RemoveActorFunctor : public TouchEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  RemoveActorFunctor( SignalData& data, bool returnValue = true )
  : TouchEventFunctor( data, returnValue )
  {
  }

  bool operator()( Actor actor, const TouchEvent& touchEvent )
  {
    Actor parent( actor.GetParent() );
    if ( parent )
    {
      parent.Remove( actor );
    }

    return TouchEventFunctor::operator()( actor, touchEvent );
  }
};

Integration::TouchEvent GenerateSingleTouch( TouchPoint::State state, Vector2 screenPosition )
{
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, state, screenPosition.x, screenPosition.y ) );
  return touchEvent;
}

///////////////////////////////////////////////////////////////////////////////

void UtcDaliTouchNormalProcessing()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Up, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Up, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where the actor is not present
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}

void UtcDaliTouchOutsideCameraNearFarPlanes()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::CENTER);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  stage.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the camera's near and far planes
  RenderTaskList taskList = stage.GetRenderTaskList();
  Dali::RenderTask task = taskList.GetTask(0);
  CameraActor camera = task.GetCameraActor();
  float nearPlane = camera.GetNearClippingPlane();
  float farPlane = camera.GetFarClippingPlane();

  // Calculate the current distance of the actor from the camera
  float tanHalfFov = tanf(camera.GetFieldOfView() * 0.5f);
  float distance = (stageSize.y * 0.5f) / tanHalfFov;

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  Vector2 screenCoordinates( stageSize.x * 0.5f, stageSize.y * 0.5f );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is just at the camera's near plane
  actor.SetZ(distance - nearPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is closer than the camera's near plane
  actor.SetZ((distance - nearPlane) + 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is just at the camera's far plane
  actor.SetZ(distance - farPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is further than the camera's far plane
  actor.SetZ((distance - farPlane) - 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchEmitEmpty()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  try
  {
    // Emit an empty TouchEvent
    Integration::TouchEvent event;
    core.SendEvent( event );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!event.points.empty()", TEST_LOCATION );
  }
}

void UtcDaliTouchInterrupted()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit an interrupted signal, we should be signalled regardless of whether there is a hit or not.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit another interrupted signal, our signal handler should not be called.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}

void UtcDaliTouchParentConsumer()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data, false );
  actor.TouchedSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchedSignal().Connect( &application, rootFunctor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 actorCoordinates, rootCoordinates;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Up, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Up, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Up, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a down signal where the actor is not present, will hit the root actor though
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchEvent.points[0].hitActor );
}

void UtcDaliTouchInterruptedParentConsumer()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data, false );
  actor.TouchedSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchedSignal().Connect( &application, rootFunctor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit an interrupted signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit an interrupted signal, only root actor's signal should be called.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another interrupted state, none of the signal's should be called.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, rootData.functorCalled, TEST_LOCATION );
}

void UtcDaliTouchLeave()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Another motion outside of actor, no signalling
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Another motion event inside actor, signalled with motion
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // We do not want to listen to leave events anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of actor, no signalling
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchLeaveParentConsumer()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data, false );
  actor.TouchedSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchedSignal().Connect( &application, rootFunctor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );
  rootActor.SetLeaveRequired( true );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion outside of actor, only rootActor signalled
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion event inside actor, signalled with motion
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // We do not want to listen to leave events of actor anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of root actor, only root signalled
  Vector2 stageSize( Stage::GetCurrent().GetSize() );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( stageSize.width + 10.0f, stageSize.height + 10.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, rootData.touchEvent.points[0].state, TEST_LOCATION );
}

void UtcDaliTouchActorBecomesInsensitive()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Change actor to insensitive
  actor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.touchEvent.points[0].state, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchActorBecomesInsensitiveParentConsumer()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data, false );
  actor.TouchedSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchedSignal().Connect( &application, rootFunctor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, data.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Down, rootData.touchEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Make root actor insensitive
  rootActor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted (should get interrupted even if within root actor)
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.touchEvent.points[0].state, TEST_LOCATION );
}

void UtcDaliTouchMultipleLayers()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );

  Layer layer1 ( Layer::New() );
  layer1.SetSize(100.0f, 100.0f);
  layer1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add( layer1 );

  Actor actor1 ( Actor::New() );
  actor1.SetSize( 100.0f, 100.0f );
  actor1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor1.SetZ( 1.0f ); // Should hit actor1 in this layer
  layer1.Add( actor1 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer1 and actor1
  layer1.TouchedSignal().Connect( &application, functor );
  actor1.TouchedSignal().Connect( &application, functor );

  // Hit in hittable area, actor1 should be hit
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make layer1 insensitive, nothing should be hit
  layer1.SetSensitive( false );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Make layer1 sensitive again, again actor1 will be hit
  layer1.SetSensitive( true );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make rootActor insensitive, nothing should be hit
  rootActor.SetSensitive( false );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Make rootActor sensitive
  rootActor.SetSensitive( true );

  // Add another layer
  Layer layer2 ( Layer::New() );
  layer2.SetSize(100.0f, 100.0f );
  layer2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  layer2.SetZ( 10.0f ); // Should hit layer2 in this layer rather than actor2
  Stage::GetCurrent().Add( layer2 );

  Actor actor2 ( Actor::New() );
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  layer2.Add( actor2 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer2 and actor2
  layer2.TouchedSignal().Connect( &application, functor );
  actor2.TouchedSignal().Connect( &application, functor );

  // Emit an event, should hit layer2
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 insensitive, should hit actor1
  layer2.SetSensitive( false );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make layer2 sensitive again, should hit layer2
  layer2.SetSensitive( true );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 invisible, render and notify
  layer2.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should hit actor1
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make rootActor invisible, render and notify
  rootActor.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should not hit anything
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchMultipleRenderTasks()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Create render task
  Viewport viewport( stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f );
  RenderTask renderTask ( Stage::GetCurrent().GetRenderTaskList().CreateTask() );
  renderTask.SetViewport( viewport );
  renderTask.SetInputEnabled( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchMultipleRenderTasksWithChildLayer()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  Layer layer = Layer::New();
  layer.SetSize(100.0f, 100.0f);
  layer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.Add(layer);

  // Create render task
  Viewport viewport( stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f );
  RenderTask renderTask ( Stage::GetCurrent().GetRenderTaskList().CreateTask() );
  renderTask.SetViewport( viewport );
  renderTask.SetInputEnabled( true );
  renderTask.SetSourceActor( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );
  layer.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchOffscreenRenderTasks()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  // FrameBufferImage for offscreen RenderTask
  FrameBufferImage frameBufferImage( FrameBufferImage::New( stageSize.width, stageSize.height, Pixel::RGBA8888 ) );

  // Create an image actor to display the FrameBufferImage
  ImageActor imageActor ( ImageActor::New( frameBufferImage ) );
  imageActor.SetParentOrigin(ParentOrigin::CENTER);
  imageActor.SetSize( stageSize.x, stageSize.y );
  imageActor.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
  stage.Add( imageActor );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add( actor );
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE ); // Ensure framebuffer connects

  stage.GetRenderTaskList().GetTask( 0u ).SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );

  // Create a RenderTask
  RenderTask renderTask = stage.GetRenderTaskList().CreateTask();
  renderTask.SetSourceActor( actor );
  renderTask.SetTargetFrameBuffer( frameBufferImage );
  renderTask.SetInputEnabled( true );

  // Create another RenderTask
  RenderTask renderTask2( stage.GetRenderTaskList().CreateTask() );
  renderTask2.SetInputEnabled( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchMultipleRenderableActors()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor parent = ImageActor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(parent);

  Actor actor = ImageActor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  parent.TouchedSignal().Connect( &application, functor );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchedActor );
}

void UtcDaliTouchActorRemovedInSignal()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  RemoveActorFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Register for leave events
  actor.SetLeaveRequired( true );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add actor back to stage, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another down event
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Completely delete the actor
  actor = NULL;

  // Emit event, should not crash and should not receive an event.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}

void UtcDaliTouchActorSignalNotConsumed()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data, false );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
}

void UtcDaliTouchActorUnStaged()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchEventFunctor functor( data );
  actor.TouchedSignal().Connect( &application, functor );

  // Emit a down signal
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Remove actor from stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Motion, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}

void UtcDaliTouchSystemOverlayActor()
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  // Create an actor and add it to the system overlay.
  Actor systemActor = Actor::New();
  systemActor.SetSize(100.0f, 100.0f);
  systemActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add( systemActor );

  // Create an actor and add it to the stage as per normal, same position and size as systemActor
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Connect to the touch signals.
  SignalData data;
  TouchEventFunctor functor( data );
  systemActor.TouchedSignal().Connect( &application, functor );
  actor.TouchedSignal().Connect( &application, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a down signal, the system overlay is drawn last so is at the top, should hit the systemActor.
  core.SendEvent( GenerateSingleTouch( TouchPoint::Down, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( systemActor == data.touchedActor );
}
