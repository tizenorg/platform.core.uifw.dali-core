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
#include <dali/integration-api/events/pan-gesture-event.h>
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

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliPanGestureDetectorConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureDetectorNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureDetectorDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSetMinimumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSetMaximumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetMinimumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetMaximumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionDownMotionLeave, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionDownMotionUp, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionCancelled, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionDetach, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionDetachWhilePanning, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionRotatedActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionChildHit, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionAttachDetachMany, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionActorBecomesUntouchable, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionMultipleGestureDetectors, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionMultipleStarted, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSignalReceptionDifferentPossible, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureEmitIncorrectState, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureDetectorTypeRegistry, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureActorUnstaged, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureActorStagedAndDestroyed, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSystemOverlay, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureAngleHandling, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureAngleOutOfRange, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureAngleProcessing, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureDirectionHandling, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureDirectionProcessing, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSetProperties, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureSetPropertiesAlreadyPanning, NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

typedef Dali::PanGestureDetector::AngleContainer::size_type AngleSizeType;

///////////////////////////////////////////////////////////////////////////////

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture(Gesture::Clear)
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.state = Gesture::Clear;
    receivedGesture.velocity = Vector2(0.0f, 0.0f);
    receivedGesture.displacement = Vector2(0.0f, 0.0f);
    receivedGesture.position = Vector2(0.0f, 0.0f);
    receivedGesture.screenPosition = Vector2(0.0f, 0.0f);
    receivedGesture.numberOfTouches = 0;

    pannedActor = NULL;
  }

  bool functorCalled;
  bool voidFunctorCalled;
  PanGesture receivedGesture;
  Actor pannedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, PanGesture pan)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = pan;
    signalData.pannedActor = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

// Functor that removes the gestured actor from stage
struct UnstageActorFunctor : public GestureReceivedFunctor
{
  UnstageActorFunctor( SignalData& data, Gesture::State& stateToUnstage )
  : GestureReceivedFunctor( data ),
    stateToUnstage( stateToUnstage )
  {
  }

  void operator()( Actor actor, PanGesture pan )
  {
    GestureReceivedFunctor::operator()( actor, pan );

    if ( pan.state == stateToUnstage )
    {
      Stage::GetCurrent().Remove( actor );
    }
  }

  Gesture::State& stateToUnstage;
};

// Functor for receiving a touch event
struct TouchEventFunctor
{
  bool operator()(Actor actor, const TouchEvent& touch)
  {
    return false;
  }
};

// Data for constraints
struct ConstraintData
{
  ConstraintData()
  : called(false)
  {
  }

  Vector2 screenPosition;
  Vector2 screenDisplacement;
  Vector2 localPosition;
  Vector2 localDisplacement;
  bool called;

  void Reset()
  {
    screenPosition = screenDisplacement = localPosition = localDisplacement = Vector2::ZERO;
    called = false;
  }
};

// Constraint used with panning properties
struct PanConstraint
{
  PanConstraint( ConstraintData& data ) : constraintData(data) { }

  Vector3 operator()(const Vector3&       current,
                     const PropertyInput& screenPositionProperty,
                     const PropertyInput& screenDisplacementProperty,
                     const PropertyInput& localPositionProperty,
                     const PropertyInput& localDisplacementProperty)
  {
    constraintData.screenPosition = screenPositionProperty.GetVector2();
    constraintData.screenDisplacement = screenDisplacementProperty.GetVector2();
    constraintData.localPosition = localPositionProperty.GetVector2();
    constraintData.localDisplacement = localDisplacementProperty.GetVector2();
    constraintData.called = true;
    return Vector3::ZERO;
  }

  ConstraintData& constraintData;
};

// Generate a PanGestureEvent to send to Core
Integration::PanGestureEvent GeneratePan(
    Gesture::State state,
    Vector2 previousPosition,
    Vector2 currentPosition,
    unsigned long timeDelta,
    unsigned int numberOfTouches = 1,
    unsigned int time = 1u)
{
  Integration::PanGestureEvent pan(state);

  pan.previousPosition = previousPosition;
  pan.currentPosition = currentPosition;
  pan.timeDelta = timeDelta;
  pan.numberOfTouches = numberOfTouches;
  pan.time = time;

  return pan;
}

// Generate a PanGesture
PanGesture GeneratePan( unsigned int time,
                        Gesture::State state,
                        Vector2 screenPosition,
                        Vector2 localPosition,
                        Vector2 screenDisplacement = Vector2::ONE,
                        Vector2 localDisplacement = Vector2::ONE,
                        Vector2 velocity = Vector2::ONE,
                        unsigned int numberOfTouches = 1 )
{
  PanGesture pan( state );

  pan.time = time;

  pan.screenPosition = screenPosition;
  pan.position = localPosition;

  pan.screenDisplacement = screenDisplacement;
  pan.displacement = localDisplacement;

  pan.screenVelocity = pan.velocity = velocity;
  pan.numberOfTouches = numberOfTouches;

  return pan;
}

///////////////////////////////////////////////////////////////////////////////

// Positive test case for a method
static void UtcDaliPanGestureDetectorConstructor()
{
  TestApplication application;

  PanGestureDetector detector;
  DALI_TEST_CHECK(!detector);
}


// Negative test case for a method
static void UtcDaliPanGestureDetectorNew()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  DALI_TEST_CHECK(detector);

  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  detector.Attach(actor);

  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  Integration::TouchEvent touchEvent(1);
  TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
  touchEvent.AddPoint(point);
  application.GetCore().SendEvent(touchEvent);
}

static void UtcDaliPanGestureDetectorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::GestureDetector::DownCast()");

  PanGestureDetector detector = PanGestureDetector::New();

  BaseHandle object(detector);

  PanGestureDetector detector2 = PanGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  PanGestureDetector detector3 = DownCast< PanGestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  PanGestureDetector detector4 = PanGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  PanGestureDetector detector5 = DownCast< PanGestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector detector6 = PanGestureDetector::New();
  PanGestureDetector detector7 = PanGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
}

static void UtcDaliPanGestureSetMinimumTouchesRequired()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  unsigned int min = 2;

  DALI_TEST_CHECK(min != detector.GetMinimumTouchesRequired());

  detector.SetMinimumTouchesRequired(min);

  DALI_TEST_EQUALS(min, detector.GetMinimumTouchesRequired(), TEST_LOCATION);

  // Attach an actor and change the minimum touches

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  TestGestureManager& gestureManager = application.GetGestureManager();
  gestureManager.Initialize();

  detector.SetMinimumTouchesRequired(3);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Reset values
  gestureManager.Initialize();

  // Create a second gesture detector that requires even less minimum touches
  PanGestureDetector secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

static void UtcDaliPanGestureSetMaximumTouchesRequired()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  unsigned int max = 3;

  DALI_TEST_CHECK(max != detector.GetMaximumTouchesRequired());

  detector.SetMaximumTouchesRequired(max);

  DALI_TEST_EQUALS(max, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  // Attach an actor and change the maximum touches

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  TestGestureManager& gestureManager = application.GetGestureManager();
  gestureManager.Initialize();

  detector.SetMaximumTouchesRequired(4);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Reset values
  gestureManager.Initialize();

  // Create a second gesture detector that requires even less maximum touches
  PanGestureDetector secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should NOT have been updated
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

static void UtcDaliPanGestureGetMinimumTouchesRequired()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
}

static void UtcDaliPanGestureGetMaximumTouchesRequired()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionNegative()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a pan outside actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(110.0f, 110.0f), Vector2(112.0f, 112.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(110.0f, 110.0f), Vector2(112.0f, 112.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pan into actor's area - we should still not receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(112.0f, 112.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop panning - we should still not receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 20.0f), Vector2(12.0f, 12.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionDownMotionLeave()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(1.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -10.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Pan Gesture leaves actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 10.0f), Vector2(320.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(300.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(30.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(300.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(30.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(320.0f, 10.0f), Vector2(310.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-10.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-1.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionDownMotionUp()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(1.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -10.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-10.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-1.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionCancelled()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // The gesture is cancelled
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Cancelled, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Cancelled, data.receivedGesture.state, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionDetach()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionDetachWhilePanning()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor during the pan, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Attach a temporary actor to stop detector being removed from PanGestureProcessor when main actor
  // is destroyed.
  Actor tempActor = Actor::New();
  tempActor.SetSize(100.0f, 100.0f);
  tempActor.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(tempActor);
  detector.Attach(tempActor);

  // Actor lifetime is scoped
  {
    Actor actor = Actor::New();
    actor.SetSize(100.0f, 100.0f);
    actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(actor);

    // Render and notify
    application.SendNotification();
    application.Render();

    detector.Attach(actor);

    // Start pan within the actor's area
    core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
    core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Continue the pan within the actor's area - we should still receive the signal
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  // Gesture ends within the area where the actor used to be
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionRotatedActor()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(22.0f, 12.0f), Vector2(27.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(8.0f, -5.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetRotation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(22.0f, 12.0f), Vector2(27.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-5.0f, -8.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetRotation(Dali::Degree(270.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(22.0f, 12.0f), Vector2(27.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-8.0f, 5.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative
}

static void UtcDaliPanGestureSignalReceptionChildHit()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  // Set child to completely cover parent.
  // Change rotation of child to be different from parent so that we can check if our local coordinate
  // conversion of the parent actor is correct.
  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::CENTER);
  child.SetParentOrigin(ParentOrigin::CENTER);
  child.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
  parent.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(22.0f, 12.0f), Vector2(27.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(5.0f, 8.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Attach child and generate same touch points to yield a different displacement
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire pan, only check finished value
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(11.0f, 12.0f), Vector2(22.0f, 12.0f), 10));
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(22.0f, 12.0f), Vector2(27.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(8.0f, -5.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative
}

static void UtcDaliPanGestureSignalReceptionAttachDetachMany()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor first = Actor::New();
  first.SetSize(100.0f, 100.0f);
  first.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  second.SetSize(100.0f, 100.0f);
  second.SetX(100.0f);
  second.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within second actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(110.0f, 20.0f), Vector2(120.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(110.0f, 20.0f), Vector2(120.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Pan moves into first actor's area - second actor should receive the pan
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(120.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Detach the second actor during the pan, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionActorBecomesUntouchable()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan in actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 10.0f), Vector2(10.0f, 10.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPanGestureSignalReceptionMultipleGestureDetectors()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
  Dali::TestGestureManager& gestureManager = application.GetGestureManager();

  Actor first = Actor::New();
  first.SetSize(100.0f, 100.0f);
  first.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  second.SetSize(100.0f, 100.0f);
  second.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  first.Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector firstDetector = PanGestureDetector::New();
  firstDetector.Attach(first);
  firstDetector.DetectedSignal().Connect(&application, functor);

  // secondDetector is scoped
  {
    // Reset gestureManager statistics
    gestureManager.Initialize();

    PanGestureDetector secondDetector = PanGestureDetector::New();
    secondDetector.SetMinimumTouchesRequired(2);
    secondDetector.SetMaximumTouchesRequired(2);
    secondDetector.Attach(second);
    secondDetector.DetectedSignal().Connect(&application, functor);

    DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

    // Start pan within second actor's area
    core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10, 2));
    core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10, 2));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

    // Two touch pan changes to single touch - we should receive a finished state
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

    // Pan continues as single touch gesture - we should not receive any gesture
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 10.0f), Vector2(30.0f, 10.0f), 10));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Pan ends - still no signal
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Finished, Vector2(30.0f, 10.0f), Vector2(30.0f, 20.0f), 10));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Single touch pan starts - first actor should be panned
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
    core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.pannedActor, TEST_LOCATION);

    // Pan changes to double-touch - we should receive a finished state
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10, 2));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.pannedActor, TEST_LOCATION);

    // Pan continues as double touch gesture - we should not receive any gesture
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 10.0f), Vector2(30.0f, 10.0f), 10));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Pan ends - still no signal
    data.Reset();
    core.SendEvent(GeneratePan(Gesture::Finished, Vector2(30.0f, 10.0f), Vector2(30.0f, 20.0f), 10));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Reset gesture manager statistics
    gestureManager.Initialize();
  }

  // secondDetector has now been deleted.  Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

void UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  PanGestureDetector firstDetector = PanGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  PanGestureDetector secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Add second actor to second detector, when we remove the actor, this will make sure that this
  // gesture detector is not removed from the GestureDetectorProcessor.  In this scenario, the
  // functor should still not be called (which is what we're also testing).
  secondDetector.Attach(actor2);

  // Pan in actor's area - both detector's functors should be called
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pan continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(10.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pan on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(20.0f, 20.0f), Vector2(10.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pan on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(20.0f, 20.0f), Vector2(10.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureSignalReceptionMultipleStarted()
{
  // Should handle two started events gracefully.

  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor's area
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Add a child actor to overlap actor and send another start in actor's area
  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::CENTER);
  child.SetParentOrigin(ParentOrigin::CENTER);
  actor.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Send another possible and start in actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor1 = Actor::New();
  actor1.SetSize(100.0f, 100.0f);
  actor1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor1);
  SignalData data1;
  GestureReceivedFunctor functor1(data1);
  PanGestureDetector detector1 = PanGestureDetector::New();
  detector1.Attach(actor1);
  detector1.DetectedSignal().Connect(&application, functor1);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  actor2.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(actor2);
  SignalData data2;
  GestureReceivedFunctor functor2(data2);
  PanGestureDetector detector2 = PanGestureDetector::New();
  detector2.Attach(actor2);
  detector2.DetectedSignal().Connect(&application, functor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor1's area, only data1 should be set
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureSignalReceptionDifferentPossible()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Gesture possible in actor's area.
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetPosition( 100.0f, 100.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LongPress possible in empty area.
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetPosition( 0.0f, 0.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal long press in actor's area for completeness.
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureEmitIncorrectState()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Clear state
  try
  {
    core.SendEvent(GeneratePan(Gesture::Clear, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
}

void UtcDaliPanGestureDetectorTypeRegistry()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "PanGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  PanGestureDetector detector = PanGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, PanGestureDetector::SIGNAL_PAN_DETECTED, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureActorUnstaged()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  Gesture::State stateToUnstage( Gesture::Started );

  // Attach actor to detector
  SignalData data;
  UnstageActorFunctor functor( data, stateToUnstage );
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit signals
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to Gesture::Continuing to remove
  stateToUnstage = Gesture::Continuing;

  // Emit signals
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to Gesture::Finished to remove
  stateToUnstage = Gesture::Finished;

  // Emit signals
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result( TET_PASS ); // If we get here then we have handled actor stage removal gracefully.
}

void UtcDaliPanGestureActorStagedAndDestroyed()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Create and add a second actor so that GestureDetector destruction does not come into play.
  Actor dummyActor( Actor::New() );
  dummyActor.SetSize( 100.0f, 100.0f );
  dummyActor.SetPosition( 100.0f, 100.0f );
  dummyActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(dummyActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  Gesture::State stateToUnstage( Gesture::Started );

  // Attach actor to detector
  SignalData data;
  UnstageActorFunctor functor( data, stateToUnstage );
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect( &application, functor );

  // Here we are testing a Started actor which is removed in the Started callback, but then added back
  // before we get a continuing state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re add to the stage, we should not be signalled
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  core.SendEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  core.SendEvent(GeneratePan(Gesture::Started, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Delete actor as well
  actor = NULL;

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  core.SendEvent(GeneratePan(Gesture::Continuing, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePan(Gesture::Finished, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPanGestureSystemOverlay()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoordsStart( 10.0f, 20.0f );
  Vector2 screenCoordsEnd( 20.0f, 20.0f );

  // Start pan within the actor's area
  core.SendEvent( GeneratePan( Gesture::Possible, screenCoordsStart, screenCoordsEnd, 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started, screenCoordsStart, screenCoordsEnd, 10 ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}

void UtcDaliPanGestureAngleHandling()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  const PanGestureDetector::AngleContainer& angles( detector.GetAngles() );
  DALI_TEST_EQUALS( angles.empty(), true, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_LEFT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(1), TEST_LOCATION );
  for ( PanGestureDetector::AngleContainer::const_iterator iter = angles.begin(), endIter = angles.end(); iter != endIter; ++iter )
  {
    if ( iter->first == PanGestureDetector::DIRECTION_LEFT )
    {
      tet_result( TET_PASS );
      break;
    }

    if ( iter == endIter )
    {
      tet_printf("%s, angle not added\n", TEST_LOCATION );
      tet_result( TET_FAIL );
    }
  }

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(2), TEST_LOCATION );

  // Remove something not in the container.
  detector.RemoveAngle( PanGestureDetector::DIRECTION_UP );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(2), TEST_LOCATION );

  detector.RemoveAngle( PanGestureDetector::DIRECTION_RIGHT );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(1), TEST_LOCATION );
  for ( PanGestureDetector::AngleContainer::const_iterator iter = angles.begin(), endIter = angles.end(); iter != endIter; ++iter )
  {
    if ( iter->first == PanGestureDetector::DIRECTION_RIGHT )
    {
      tet_printf("%s, angle not removed\n", TEST_LOCATION );
      tet_result( TET_FAIL );
      break;
    }
  }

  detector.ClearAngles();
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(0), TEST_LOCATION );
}

inline float RadiansToDegrees( float radian )
{
  return radian * 180.0f / Math::PI;
}

void UtcDaliPanGestureAngleOutOfRange()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  const PanGestureDetector::AngleContainer& angles( detector.GetAngles() );
  DALI_TEST_EQUALS( angles.empty(), true, TEST_LOCATION );

  //
  // Angle
  //

  detector.AddAngle( Degree(180.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(-180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(190.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(-170.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-190.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(170.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(350.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(-10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-350.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(370.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-370.0f) );
  DALI_TEST_EQUALS( angles.begin()->first, Radian( Degree(-10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  //
  // Threshold
  //

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 0.0f ) );
  DALI_TEST_EQUALS( angles.begin()->second, Radian( Degree(0.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( -10.0f ) );
  DALI_TEST_EQUALS( angles.begin()->second, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( -181.0f ) );
  DALI_TEST_EQUALS( angles.begin()->second, Radian( Degree(180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 181.0f ) );
  DALI_TEST_EQUALS( angles.begin()->second, Radian( Degree(180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();
}

void UtcDaliPanGestureAngleProcessing()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires up pans
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach( parent );
  parentDetector.AddAngle( PanGestureDetector::DIRECTION_UP, Degree( 30.0f ) );
  SignalData parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires right pans
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach( child );
  childDetector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 30.0f ) );
  SignalData childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10 ) );
  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(30.0f, 20.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(20.0f, 30.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(10.0f, 20.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();
}

void UtcDaliPanGestureDirectionHandling()
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  const PanGestureDetector::AngleContainer& angles( detector.GetAngles() );
  DALI_TEST_EQUALS( angles.empty(), true, TEST_LOCATION );

  detector.AddDirection( PanGestureDetector::DIRECTION_LEFT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(2), TEST_LOCATION );
  for ( PanGestureDetector::AngleContainer::const_iterator iter = angles.begin(), endIter = angles.end(); iter != endIter; ++iter )
  {
    if ( iter->first == PanGestureDetector::DIRECTION_LEFT )
    {
      tet_result( TET_PASS );
      break;
    }

    if ( iter == endIter )
    {
      tet_printf("%s, angle not added\n", TEST_LOCATION );
      tet_result( TET_FAIL );
    }
  }

  for ( PanGestureDetector::AngleContainer::const_iterator iter = angles.begin(), endIter = angles.end(); iter != endIter; ++iter )
  {
    if ( iter->first == PanGestureDetector::DIRECTION_RIGHT )
    {
      tet_result( TET_PASS );
      break;
    }

    if ( iter == endIter )
    {
      tet_printf("%s, angle not added\n", TEST_LOCATION );
      tet_result( TET_FAIL );
    }
  }

  // Remove something not in the container.
  detector.RemoveDirection( PanGestureDetector::DIRECTION_UP );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(2), TEST_LOCATION );

  detector.RemoveDirection( PanGestureDetector::DIRECTION_RIGHT );
  DALI_TEST_EQUALS( angles.size(), static_cast<AngleSizeType>(0), TEST_LOCATION );
}

void UtcDaliPanGestureDirectionProcessing()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires vertical panning
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach( parent );
  parentDetector.AddDirection( PanGestureDetector::DIRECTION_VERTICAL, Degree( 30.0f ) );
  SignalData parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires horizontal panning
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach( child );
  childDetector.AddDirection( PanGestureDetector::DIRECTION_HORIZONTAL, Degree( 30.0f ) );
  SignalData childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(20.0f, 10.0f), 10 ) );
  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(30.0f, 20.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, only parent should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(20.0f, 30.0f), 10 ) );
  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, only child should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(10.0f, 20.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -45 degrees, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(10.0f, 30.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a pan at 45 degrees, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(30.0f, 30.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a pan at 135 degrees, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(10.0f, 30.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -135 degrees, no one should receive it.
  core.SendEvent( GeneratePan( Gesture::Possible, Vector2(20.0f, 20.0f), Vector2(20.0f, 20.0f), 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  Vector2(20.0f, 20.0f), Vector2(10.0f, 10.0f), 10 ) );
  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );
  core.SendEvent( GeneratePan( Gesture::Finished,  Vector2(20.0f, 30.0f), Vector2(20.0f, 20.0f), 10 ) );
  parentData.Reset();
  childData.Reset();
}

void UtcDaliPanGestureSetProperties()
{
  TestApplication application;
  TestRenderController& renderController( application.GetRenderController() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  actor.ApplyConstraint( Constraint::New<Vector3>( property, Source( detector, PanGestureDetector::SCREEN_POSITION ),
                                                             Source( detector, PanGestureDetector::SCREEN_DISPLACEMENT ),
                                                             Source( detector, PanGestureDetector::LOCAL_POSITION ),
                                                             Source( detector, PanGestureDetector::LOCAL_DISPLACEMENT ),
                                                             PanConstraint( constraintData ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  renderController.Initialize();
  DALI_TEST_EQUALS( renderController.WasCalled( TestRenderController::RequestUpdateFunc ), false, TEST_LOCATION );

  Vector2 screenPosition( 20.0f, 20.0f );
  Vector2 screenDisplacement( 1.0f, 1.0f );
  Vector2 localPosition( 21.0f, 21.0f );
  Vector2 localDisplacement( 0.5f, 0.5f );

  PanGestureDetector::SetPanGestureProperties( GeneratePan( 1u, Gesture::Started, screenPosition, localPosition, screenDisplacement, localDisplacement ) );
  DALI_TEST_EQUALS( renderController.WasCalled( TestRenderController::RequestUpdateFunc ), true, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition,     screenPosition,     TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,      localPosition,      TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenDisplacement, screenDisplacement, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localDisplacement,  localDisplacement,  TEST_LOCATION );
  constraintData.Reset();
}

void UtcDaliPanGestureSetPropertiesAlreadyPanning()
{
  TestApplication application;
  Integration::Core& core( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  actor.ApplyConstraint( Constraint::New<Vector3>( property, Source( detector, PanGestureDetector::SCREEN_POSITION ),
                                                             Source( detector, PanGestureDetector::SCREEN_DISPLACEMENT ),
                                                             Source( detector, PanGestureDetector::LOCAL_POSITION ),
                                                             Source( detector, PanGestureDetector::LOCAL_DISPLACEMENT ),
                                                             PanConstraint( constraintData ) ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 previousPosition( 20.0f, 20.0f );
  Vector2 currentPosition( 20.0f, 10.0f );
  core.SendEvent( GeneratePan( Gesture::Possible, previousPosition, previousPosition, 10 ) );
  core.SendEvent( GeneratePan( Gesture::Started,  previousPosition, currentPosition, 10 ) );
  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );

  Vector2 screenPosition( 100.0f, 20.0f );
  Vector2 localPosition( 110.0f, 110.0f );

  PanGestureDetector::SetPanGestureProperties( GeneratePan( 1u, Gesture::Started, screenPosition, localPosition ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, currentPosition, 0.1, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  currentPosition, 0.1f, TEST_LOCATION );
  constraintData.Reset();
}
