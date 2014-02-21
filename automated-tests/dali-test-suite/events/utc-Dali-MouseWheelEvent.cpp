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
#include <dali/integration-api/events/mouse-wheel-event-integ.h>

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

// Key Event Test references
const static unsigned int SHIFT_MODIFIER  = 0x1;
const static unsigned int CTRL_MODIFIER  = 0x2;
const static unsigned int ALT_MODIFIER  = 0x4;
const static unsigned int SHIFT_AND_CTRL_MODIFIER  = SHIFT_MODIFIER | CTRL_MODIFIER;

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliMouseWheelEventConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsShiftModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsCtrlModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsAltModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsNotShiftModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsNotCtrlModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventIsNotAltModifier, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventANDModifer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventORModifer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliMouseWheelEventSignalling, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;

    receivedMouseWheelEvent.direction = 0;
    receivedMouseWheelEvent.modifiers = 0;
    receivedMouseWheelEvent.point = Vector2::ZERO;
    receivedMouseWheelEvent.z = 0;
    receivedMouseWheelEvent.timeStamp = 0;

    mouseWheeledActor = NULL;
  }

  bool functorCalled;
  MouseWheelEvent receivedMouseWheelEvent;
  Actor mouseWheeledActor;
};

// Functor that sets the data when called
struct MouseWheelEventReceivedFunctor
{
  MouseWheelEventReceivedFunctor( SignalData& data ) : signalData( data ) { }

  bool operator()( Actor actor, const MouseWheelEvent& mouseWheelEvent )
  {
    signalData.functorCalled = true;
    signalData.receivedMouseWheelEvent = mouseWheelEvent;
    signalData.mouseWheeledActor = actor;

    return true;
  }

  SignalData& signalData;
};


static void UtcDaliMouseWheelEventConstructor()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, SHIFT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);  // coustruct a mouse wheel event

  DALI_TEST_EQUALS(1, event.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(Vector2(1.0f, 1.0f), event.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, event.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, event.timeStamp, TEST_LOCATION); // check modifier
}

// Positive test case for a method
static void UtcDaliMouseWheelEventIsShiftModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER; // Set to Shift Modifier

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsShiftModifier(), TEST_LOCATION); // check IsShiftModifier

}

// Positive test case for a method
static void UtcDaliMouseWheelEventIsCtrlModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = CTRL_MODIFIER; // Set to Ctrl Modifier

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsCtrlModifier(), TEST_LOCATION); // check IsCtrlModifier
}

// Positive test case for a method
static void UtcDaliMouseWheelEventIsAltModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event;
  DALI_TEST_EQUALS(0u, event.modifiers, TEST_LOCATION);

  event.modifiers = ALT_MODIFIER; // Set to Alt Modifier

  DALI_TEST_EQUALS(ALT_MODIFIER, event.modifiers, TEST_LOCATION); // check able to set

  DALI_TEST_EQUALS(true, event.IsAltModifier(), TEST_LOCATION);  // IsAltModifier
}

// Positive fail test case for a method
static void UtcDaliMouseWheelEventIsNotShiftModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(CTRL_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsShiftModifier(), TEST_LOCATION);
}

// Positive fail test case for a method
static void UtcDaliMouseWheelEventIsNotCtrlModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, ALT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(ALT_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsCtrlModifier(), TEST_LOCATION);
}

// Positive fail test case for a method
static void UtcDaliMouseWheelEventIsNotAltModifier()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, SHIFT_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);

  DALI_TEST_EQUALS(SHIFT_MODIFIER, event.modifiers, TEST_LOCATION);  // check different modifier used

  DALI_TEST_EQUALS(false, event.IsAltModifier(), TEST_LOCATION);
}

// Positive test case for a method
static void UtcDaliMouseWheelEventANDModifer()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, SHIFT_AND_CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() && event.IsShiftModifier(), TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() && event.IsShiftModifier(), TEST_LOCATION);
}

// Positive test case for a method
static void UtcDaliMouseWheelEventORModifer()
{
  TestApplication application; // Reset all test adapter return codes

  MouseWheelEvent event(1, SHIFT_AND_CTRL_MODIFIER, Vector2(1.0f, 1.0f), 1, 1000u);
  DALI_TEST_EQUALS(true, event.IsCtrlModifier() || event.IsAltModifier(), TEST_LOCATION);

  event.modifiers = SHIFT_MODIFIER;

  DALI_TEST_EQUALS(false, event.IsCtrlModifier() && event.IsAltModifier(), TEST_LOCATION);
}

void UtcDaliMouseWheelEventSignalling()
{
  TestApplication application; // Reset all test adapter return codes
  Dali::Integration::Core& core ( application.GetCore() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's mouse wheel event signal
  SignalData data;
  MouseWheelEventReceivedFunctor functor( data );
  actor.MouseWheelEventSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Integration::MouseWheelEvent event(0, SHIFT_MODIFIER, screenCoordinates, 1, 1000u);

  // Emit a mouse wheel signal
  core.SendEvent( event );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.mouseWheeledActor );
  DALI_TEST_EQUALS(0, data.receivedMouseWheelEvent.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, data.receivedMouseWheelEvent.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(screenCoordinates, data.receivedMouseWheelEvent.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, data.receivedMouseWheelEvent.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, data.receivedMouseWheelEvent.timeStamp, TEST_LOCATION); // check modifier
  data.Reset();

  // Emit a mouse wheel signal where the actor is not present, will hit the root actor though
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  // Connect to root actor's mouse wheel event signal
  SignalData rootData;
  MouseWheelEventReceivedFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.MouseWheelEventSignal().Connect( &application, rootFunctor );

  screenCoordinates.x = screenCoordinates.y = 300.0f;
  Integration::MouseWheelEvent newEvent(0, SHIFT_MODIFIER, screenCoordinates, 1, 1000u);
  core.SendEvent( newEvent );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.mouseWheeledActor );
  DALI_TEST_EQUALS(0, rootData.receivedMouseWheelEvent.direction, TEST_LOCATION); // check direction
  DALI_TEST_EQUALS(SHIFT_MODIFIER, rootData.receivedMouseWheelEvent.modifiers, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedMouseWheelEvent.point, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1, rootData.receivedMouseWheelEvent.z, TEST_LOCATION); // check modifier
  DALI_TEST_EQUALS(1000u, rootData.receivedMouseWheelEvent.timeStamp, TEST_LOCATION); // check modifier

  // Remove actor from stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  core.SendEvent( event );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
}
