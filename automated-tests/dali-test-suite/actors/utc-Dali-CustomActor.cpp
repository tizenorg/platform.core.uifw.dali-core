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
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/integration-api/events/key-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C"
{
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum
{
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliCustomActorDestructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorImplDestructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorDownCastNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnStageConnectionDisconnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnStageConnectionOrder, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnStageDisconnectionOrder, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorAddDuringOnStageConnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorRemoveDuringOnStageConnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorAddDuringOnStageDisconnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorRemoveDuringOnStageDisconnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorRemoveParentDuringOnStageConnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorAddParentDuringOnStageDisconnection, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnChildAddRemove, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorReparentDuringOnChildAdd, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorRemoveDuringOnChildRemove, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnPropertySet, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnSizeSet, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnSizeAnimation, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnTouchEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorOnMouseWheelEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliActorFindChildByAlias, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorDoAction, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorImplOnPropertySet, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliCustomActorGetImplementation, POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

std::vector< std::string > MasterCallStack;

// TypeRegistry needs custom actor Implementations to have the same name (namespaces are ignored so we use one here)
namespace Impl
{

struct TestCustomActor : public CustomActorImpl
{
  /**
   * Constructor
   */
  TestCustomActor()
  : CustomActorImpl( true ), // requires touch
    mDaliProperty( Property::INVALID_INDEX ),
    mSizeSet( Vector3::ZERO ),
    mTargetSize( Vector3::ZERO )
  {
    SetRequiresMouseWheelEvents(true);
  }

  /**
   * Destructor
   */
  virtual ~TestCustomActor()
  {
  }

  void Initialize( const char* name = NULL )
  {
    mDaliProperty = Self().RegisterProperty( "Dali", std::string("no"), Property::READ_WRITE);

    OnInitialize( name );
  }

  virtual void OnInitialize( const char* name ) {}

  /**
   * Resets the call stack
   */
  void ResetCallStack()
  {
    mSizeSet = Vector3();
    mTargetSize = Vector3();
    mMethodsCalled.clear();
  }

  void AddToCallStacks( const char* method )
  {
    mMethodsCalled.push_back( method );

    // Combine Actor name with method string
    std::string nameAndMethod( Self().GetName() );
    if ( 0 == nameAndMethod.size() )
    {
      nameAndMethod = "Unknown: ";
    }
    else
    {
      nameAndMethod += ": ";
    }
    nameAndMethod += method;

    MasterCallStack.push_back( nameAndMethod );
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
    AddToCallStacks("OnStageConnection");
  }
  virtual void OnStageDisconnection()
  {
    AddToCallStacks("OnStageDisconnection");
  }
  virtual void OnChildAdd(Actor& child)
  {
    AddToCallStacks("OnChildAdd");
  }
  virtual void OnChildRemove(Actor& child)
  {
    AddToCallStacks("OnChildRemove");
  }
  virtual void OnPropertySet( Property::Index index, Property::Value propertyValue )
  {
    AddToCallStacks("OnPropertySet");
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
    mSizeSet = targetSize;
    AddToCallStacks("OnSizeSet");
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
    mTargetSize = targetSize;
    AddToCallStacks("OnSizeAnimation");
  }
  virtual bool OnTouchEvent(const TouchEvent& event)
  {
    AddToCallStacks("OnTouchEvent");
    return true;
  }
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    AddToCallStacks("OnMouseWheelEvent");
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
  {
    AddToCallStacks("OnKeyEvent");
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
    AddToCallStacks("OnKeyInputFocusGained");
  }
  virtual void OnKeyInputFocusLost()
  {
    AddToCallStacks("OnKeyInputFocusLost");
  }
  virtual Actor GetChildByAlias(const std::string& actorAlias)
  {
    AddToCallStacks("GetChildByAlias");

    if ("found" == actorAlias)
    {
      return Actor::New();
    }
    else
    {
      return Actor();
    }
  }

  void SetDaliProperty(std::string s)
  {
    Self().SetProperty(mDaliProperty, s) ;
  }

  Property::Index mDaliProperty;
  std::vector< std::string > mMethodsCalled;
  Vector3 mSizeSet;
  Vector3 mTargetSize;
};

/**
 * Variant which adds a new child during OnStageConnection
 */
struct TestCustomActorVariant1 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant1( Actor childToAdd )
  : mChildToAdd( childToAdd )
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
    // Chain up first
    TestCustomActor::OnStageConnection();

    // Add the child
    Self().Add( mChildToAdd );
  }

  Actor mChildToAdd;
};

/**
 * Variant which removes children during OnStageConnection
 */
struct TestCustomActorVariant2 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant2()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
    // Chain up first
    TestCustomActor::OnStageConnection();

    // Remove all the children
    for( unsigned int i=0, num=Self().GetChildCount(); i<num; ++i )
    {
      Self().Remove( Self().GetChildAt(0) );
    }
  }
};

/**
 * Variant which adds a new child during OnStageDisconnection
 */
struct TestCustomActorVariant3 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant3( Actor childToAdd )
  : mChildToAdd( childToAdd )
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Add the child
    Self().Add( mChildToAdd );
  }

  Actor mChildToAdd;
};

/**
 * Variant which removes children during OnStageDisconnection
 */
struct TestCustomActorVariant4 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant4()
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Remove all the children
    for( unsigned int i=0, num=Self().GetChildCount(); i<num; ++i )
    {
      Self().Remove( Self().GetChildAt(0) );
    }
  }
};

/**
 * Variant which removes its parent from Stage during OnStageConnection
 */
struct TestCustomActorVariant5 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant5()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
    // Chain up first
    TestCustomActor::OnStageConnection();

    // Take parent off-stage
    Actor parent = Self().GetParent();
    if ( parent )
    {
      Stage::GetCurrent().Remove( parent );
    }
  }
};

/**
 * Variant which adds its parent to Stage during OnStageDisconnection
 */
struct TestCustomActorVariant6 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant6()
  {
  }

  // From CustomActorImpl
  virtual void OnStageDisconnection()
  {
    // Chain up first
    TestCustomActor::OnStageDisconnection();

    // Put parent on-stage
    Actor parent = Self().GetParent();
    if ( parent )
    {
      Stage::GetCurrent().Add( parent );
    }
  }
};

/**
 * Variant which reparents its children into a separate container
 */
struct TestCustomActorVariant7 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant7()
  {
  }

  virtual void OnInitialize( const char* name )
  {
    // We need to do this early, before the OnChildAdd is recorded
    Self().SetName( name );

    mContainer = Actor::New();
    mContainer.SetName( "Container" );
    Self().Add( mContainer );
  }

  // From CustomActorImpl
  virtual void OnChildAdd(Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildAdd(child);

    // Reparent child
    if ( child != mContainer )
    {
      mContainer.Add( child );
    }
  }

  Actor mContainer;
};

/**
 * Variant which attempts to interfere with the reparenting of a child to another container
 */
struct TestCustomActorVariant8 : public TestCustomActor
{
  /**
   * Constructor
   */
  TestCustomActorVariant8( Actor rival )
  : mRivalContainer( rival )
  {
  }

  // From CustomActorImpl
  virtual void OnChildRemove(Actor& child)
  {
    // Chain up first
    TestCustomActor::OnChildRemove(child);

    mRivalContainer.Remove( child ); // attempt to block reparenting to rival (should be a NOOP)
  }

  Actor mRivalContainer;
};

// Need a class that doesn't override virtual methods
class SimpleTestCustomActor : public CustomActorImpl
{
public:

  /**
   * Constructor
   */
  SimpleTestCustomActor()
  : CustomActorImpl( true ) // requires touch
  {
  }

  /**
   * Destructor
   */
  virtual ~SimpleTestCustomActor()
  {
  }

  // From CustomActorImpl
  virtual void OnStageConnection()
  {
  }
  virtual void OnStageDisconnection()
  {
  }
  virtual void OnChildAdd(Actor& child)
  {
  }
  virtual void OnChildRemove(Actor& child)
  {
  }
  virtual void OnSizeSet(const Vector3& targetSize)
  {
  }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize)
  {
  }
  virtual bool OnTouchEvent(const TouchEvent& event)
  {
    return true;
  }
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    return true;
  }
  virtual bool OnKeyEvent(const KeyEvent& event)
  {
    return true;
  }
  virtual void OnKeyInputFocusGained()
  {
  }
  virtual void OnKeyInputFocusLost()
  {
  }

  virtual Actor GetChildByAlias(const std::string& actorAlias)
  {
    return Actor();
  }
};

} ; // namespace Impl

/**
 * Test custom actor handle
 */
class TestCustomActor : public CustomActor
{
public:

  static TestCustomActor New()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActor;
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant1( Actor childToAdd )
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant1( childToAdd );
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant2()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant2();
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant3( Actor childToAdd )
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant3( childToAdd );
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant4()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant4();
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant5()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant5();
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant6()
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant6();
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  static TestCustomActor NewVariant7( const char* name )
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant7();
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize( name );

    return custom;
  }

  static TestCustomActor NewVariant8( Actor rival )
  {
    Impl::TestCustomActor* impl = new Impl::TestCustomActorVariant8( rival );
    TestCustomActor custom( *impl ); // takes ownership

    impl->Initialize();

    return custom;
  }

  virtual ~TestCustomActor()
  {
  }

  Impl::TestCustomActor& GetImpl()
  {
    return static_cast<Impl::TestCustomActor&>(GetImplementation());
  }

  std::vector< std::string >& GetMethodsCalled()
  {
    return GetImpl().mMethodsCalled;
  }

  void ResetCallStack()
  {
    GetImpl().ResetCallStack();
  }

  void SetDaliProperty(std::string s)
  {
    GetImpl().SetDaliProperty(s);
  }

  Vector3 GetSize()
  {
    return GetImpl().mSizeSet;
  }

  Vector3 GetTargetSize()
  {
    return GetImpl().mTargetSize;
  }

private:

  TestCustomActor( Impl::TestCustomActor& impl ) : CustomActor( impl )
  {
  }
};

namespace
{

using namespace Dali;

BaseHandle CreateActor()
{
  return TestCustomActor::New();
}

TypeRegistration mType( typeid(TestCustomActor), typeid(Dali::CustomActor), CreateActor );

}


static void UtcDaliCustomActorDestructor()
{
  TestApplication application;

  CustomActor* actor = new CustomActor();
  delete actor;

  DALI_TEST_CHECK( true );
}

static void UtcDaliCustomActorImplDestructor()
{
  TestApplication application;
  CustomActorImpl* actor = new Impl::TestCustomActor();
  delete actor;

  DALI_TEST_CHECK( true );
}

// Positive test case for a method
static void UtcDaliCustomActorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  TestCustomActor custom = TestCustomActor::New();

  Actor anActor = Actor::New();
  anActor.Add( custom );

  Actor child = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast( child );
  DALI_TEST_CHECK( customActor );

  customActor = NULL;
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( child );
  DALI_TEST_CHECK( customActor );
}

// Negative test case for a method
static void UtcDaliCustomActorDownCastNegative()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  CustomActor customActor = CustomActor::DownCast( child );
  DALI_TEST_CHECK( !customActor );

  Actor unInitialzedActor;
  customActor = CustomActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );

  customActor = DownCast< CustomActor >( unInitialzedActor );
  DALI_TEST_CHECK( !customActor );
}

static void UtcDaliCustomActorOnStageConnectionDisconnection()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageConnection() & OnStageDisconnection");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  Stage::GetCurrent().Remove( custom );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorOnStageConnectionOrder()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageConnection() order");

  MasterCallStack.clear();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnStageConnection should be received for A, B, D, E, C, and finally F
   */

  TestCustomActor actorA = TestCustomActor::New();
  actorA.SetName( "ActorA" );

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  TestCustomActor actorC = TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  TestCustomActor actorD = TestCustomActor::New();
  actorD.SetName( "ActorD" );
  actorB.Add( actorD );

  TestCustomActor actorE = TestCustomActor::New();
  actorE.SetName( "ActorE" );
  actorB.Add( actorE );

  TestCustomActor actorF = TestCustomActor::New();
  actorF.SetName( "ActorF" );
  actorC.Add( actorF );

  // add the custom actor to stage
  Stage::GetCurrent().Add( actorA );

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorB.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorC.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorC.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorD.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorE.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorF.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS( 3+3+2+1+1+1, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd", MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnChildAdd", MasterCallStack[ 4 ], TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 5 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection", MasterCallStack[ 6 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorD: OnStageConnection", MasterCallStack[ 7 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorE: OnStageConnection", MasterCallStack[ 8 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnStageConnection", MasterCallStack[ 9 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorF: OnStageConnection", MasterCallStack[ 10 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorOnStageDisconnectionOrder()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnStageDisconnection() order");

  Stage stage = Stage::GetCurrent();

  /* Build tree of actors:
   *
   *       A (parent)
   *      / \
   *     B   C
   *    / \   \
   *   D   E   F
   *
   * OnStageDisconnection should be received for D, E, B, F, C, and finally A.
   */

  TestCustomActor actorA = TestCustomActor::New();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  TestCustomActor actorC = TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  TestCustomActor actorD = TestCustomActor::New();
  actorD.SetName( "ActorD" );
  actorB.Add( actorD );

  TestCustomActor actorE = TestCustomActor::New();
  actorE.SetName( "ActorE" );
  actorB.Add( actorE );

  TestCustomActor actorF = TestCustomActor::New();
  actorF.SetName( "ActorF" );
  actorC.Add( actorF );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  actorC.ResetCallStack();
  actorD.ResetCallStack();
  actorE.ResetCallStack();
  actorF.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove( actorA );

  DALI_TEST_EQUALS( 1, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorC.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorD.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorD.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorE.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorE.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorF.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorF.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  // Check sequence is correct in MasterCallStack

  DALI_TEST_EQUALS( 6, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorD: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorE: OnStageDisconnection", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorF: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorC: OnStageDisconnection", MasterCallStack[ 4 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 5 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorAddDuringOnStageConnection()
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which adds a child to itself during OnStageConnection()
   * The actorB is provided as the child
   */

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );

  TestCustomActor actorA = TestCustomActor::NewVariant1( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION ); // Called from within OnStageConnection()

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection", MasterCallStack[ 1 ], TEST_LOCATION ); // Occurs during Actor::Add from within from within OnStageConnection()
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 2 ], TEST_LOCATION ); // Occurs after Actor::Add from within from within OnStageConnection()

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after Actors are removed

  stage.Remove( actorA );
  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorRemoveDuringOnStageConnection()
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which removes its children during OnStageConnection()
   * Actors B & C are provided as the children
   */

  TestCustomActor actorA = TestCustomActor::NewVariant2();
  actorA.SetName( "ActorA" );

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  TestCustomActor actorC = TestCustomActor::New();
  actorC.SetName( "ActorC" );
  actorA.Add( actorC );

  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 5, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",     actorA.GetMethodsCalled()[ 3 ], TEST_LOCATION ); // Called from within OnStageConnection()
  DALI_TEST_EQUALS( "OnChildRemove",     actorA.GetMethodsCalled()[ 4 ], TEST_LOCATION ); // Called from within OnStageConnection()

  DALI_TEST_EQUALS( 5, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",        MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection", MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",     MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",     MasterCallStack[ 4 ], TEST_LOCATION );

  /* Actors B & C should be removed before the point where they could receive an OnStageConnection callback
   * Therefore they shouldn't receive either OnStageConnection or OnStageDisconnection
   */
  DALI_TEST_EQUALS( 0, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, (int)(actorC.GetMethodsCalled().size()), TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Check everything is ok after last actor is removed

  stage.Remove( actorA );
  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorAddDuringOnStageDisconnection()
{
  TestApplication application;
  tet_infoline("Testing Actor::Add behaviour during Dali::CustomActor::OnStageDisonnection() callback");

  Stage stage = Stage::GetCurrent();

  /* The actorA is a special variant which adds a child to itself during OnStageDisconnection()
   * The actorB is provided as the child
   */

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );

  TestCustomActor actorA = TestCustomActor::NewVariant3( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  // Child was added after parent disconnection, so should not receive OnStageConnection()
  DALI_TEST_EQUALS( 0, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 1 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorRemoveDuringOnStageDisconnection()
{
  TestApplication application;
  tet_infoline("Testing Actor::Remove behaviour during Dali::CustomActor::OnStageDisconnection() callback");

  Stage stage = Stage::GetCurrent();

  /* The actorA is a special variant which removes its children during OnStageDisconnection()
   * The actorB is provided as the child
   */

  TestCustomActor actorA = TestCustomActor::NewVariant4();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();

  // Clear call stacks before disconnection
  actorA.ResetCallStack();
  actorB.ResetCallStack();
  MasterCallStack.clear();

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 3, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 2 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorRemoveParentDuringOnStageConnection()
{
  TestApplication application;
  tet_infoline("Weird test where child removes its own parent from Stage during Dali::CustomActor::OnStageConnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which removes its own parent during OnStageConnection()
   * The child actor is interrupting the parent's connection to stage, therefore the parent should not get an OnStageDisconnection()
   */

  TestCustomActor actorA = TestCustomActor::New();
  actorA.SetName( "ActorA" );

  TestCustomActor actorB = TestCustomActor::NewVariant5();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  stage.Add( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 1, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection", actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 4, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorAddParentDuringOnStageDisconnection()
{
  TestApplication application;
  tet_infoline("Weird test where child adds its own parent to Stage during Dali::CustomActor::OnStageDisconnection() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is the parent of actorB
   * The actorB is a special variant which (weirdly) adds its own parent during OnStageDisconnection()
   * The child actor is interrupting the disconnection, such that parent should not get a OnStageDisconnection()
   */

  TestCustomActor actorA = TestCustomActor::New();
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  TestCustomActor actorB = TestCustomActor::NewVariant6();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  stage.Remove( actorA );

  // Check callback sequence

  DALI_TEST_EQUALS( 2, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 2, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  // Disconnect was interrupted, so we should only get one OnStageConnection() for actorB

  DALI_TEST_EQUALS( 4, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 3 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorOnChildAddRemove()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnChildAdd() & OnChildRemove()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  Actor aChild = Actor::New();
  custom.Add( aChild );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );

  custom.Remove( aChild );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );
}

static void UtcDaliCustomActorReparentDuringOnChildAdd()
{
  TestApplication application;
  tet_infoline("Testing Actor:Add (reparenting) behaviour during Dali::CustomActor::OnChildAdd() callback");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The actorA is a special variant which reparents children added into a separate container child
   * The actorB is the child of actorA
   */

  TestCustomActor actorA = TestCustomActor::NewVariant7( "ActorA" );
  stage.Add( actorA );

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  actorA.Add( actorB );

  // Check hierarchy is as follows:
  //  A
  //  |
  //  Container
  //  |
  //  B

  DALI_TEST_EQUALS( 1, (int)(actorA.GetChildCount()), TEST_LOCATION );

  Actor container = actorA.GetChildAt(0);
  Actor containerChild;

  DALI_TEST_CHECK( container );
  if ( container )
  {
    DALI_TEST_EQUALS( "Container", container.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( 1, (int)(container.GetChildCount()), TEST_LOCATION );
    containerChild = container.GetChildAt(0);
  }

  DALI_TEST_CHECK( containerChild );
  if ( containerChild )
  {
    DALI_TEST_EQUALS( "ActorB", containerChild.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( 0, (int)(containerChild.GetChildCount()), TEST_LOCATION );
  }

  // Check callback sequence

  DALI_TEST_EQUALS( 4, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION ); // The mContainer added to actorA
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION ); // The actorB added to actorA
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 3 ], TEST_LOCATION );
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS( 3, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageDisconnection", actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 2 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 7, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageDisconnection", MasterCallStack[ 4 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 5 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 6 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

/**
 * Test that Remove can be called (a NOOP) during the OnChildRemove
 * triggered when reparenting an actor
 */
static void UtcDaliCustomActorRemoveDuringOnChildRemove()
{
  TestApplication application;
  tet_infoline("Testing Actor:Remove behaviour during OnChildRemove() callback triggered when reparenting");

  Stage stage = Stage::GetCurrent();

  MasterCallStack.clear();

  /* The childActor will be reparented from actorA to actorB
   * The actorA is a special variant which attempts to remove a child from actorB, during the OnChildRemove callback()
   * This should be a NOOP since the reparenting has not occured yet
   */

  TestCustomActor actorB = TestCustomActor::New();
  actorB.SetName( "ActorB" );
  stage.Add( actorB );

  TestCustomActor actorA = TestCustomActor::NewVariant8( actorB );
  actorA.SetName( "ActorA" );
  stage.Add( actorA );

  Actor childActor = Actor::New();
  childActor.SetName( "Child" );
  // Reparent from actorA to actorB
  actorA.Add( childActor );
  actorB.Add( childActor );

  // Check hierarchy is as follows:
  //  A    B
  //       |
  //       Child

  DALI_TEST_EQUALS( 0, (int)(actorA.GetChildCount()), TEST_LOCATION );
  DALI_TEST_EQUALS( 1, (int)(actorB.GetChildCount()), TEST_LOCATION );
  DALI_TEST_EQUALS( 0, (int)(childActor.GetChildCount()), TEST_LOCATION );

  Actor child = actorB.GetChildAt(0);

  DALI_TEST_CHECK( child );
  if ( child )
  {
    DALI_TEST_EQUALS( "Child", child.GetName(), TEST_LOCATION );
  }

  // Check callback sequence

  DALI_TEST_EQUALS( 3, (int)(actorA.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorA.GetMethodsCalled()[ 0 ], TEST_LOCATION ); // The mContainer added to actorA
  DALI_TEST_EQUALS( "OnChildAdd",           actorA.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildRemove",        actorA.GetMethodsCalled()[ 2 ], TEST_LOCATION ); // The actorB added to actorA
  // mContainer will then receive OnChildAdd

  DALI_TEST_EQUALS( 2, (int)(actorB.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnStageConnection",    actorB.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "OnChildAdd",           actorB.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_EQUALS( 5, (int)(MasterCallStack.size()), TEST_LOCATION );

  DALI_TEST_EQUALS( "ActorB: OnStageConnection",    MasterCallStack[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnStageConnection",    MasterCallStack[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildAdd",           MasterCallStack[ 2 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorA: OnChildRemove",        MasterCallStack[ 3 ], TEST_LOCATION );
  DALI_TEST_EQUALS( "ActorB: OnChildAdd",           MasterCallStack[ 4 ], TEST_LOCATION );

  // Excercise the message passing to Update thread

  application.SendNotification();
  application.Render();
  application.Render();
}

static void UtcDaliCustomActorOnPropertySet()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnPropertySet()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  custom.SetDaliProperty("yes") ;

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnPropertySet", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
}

static void UtcDaliCustomActorOnSizeSet()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeSet()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  custom.SetSize( Vector2( 9.0f, 10.0f ) );
  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeSet", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 9.0f, custom.GetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, custom.GetSize().height, TEST_LOCATION );

  custom.SetSize( Vector3( 4.0f, 5.0f, 6.0f ) );
  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeSet", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 4.0f, custom.GetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 5.0f, custom.GetSize().height, TEST_LOCATION );
  DALI_TEST_EQUALS( 6.0f, custom.GetSize().depth, TEST_LOCATION );
}

static void UtcDaliCustomActorOnSizeAnimation()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnSizeAnimation()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  Animation anim = Animation::New( 1.0f );
  anim.Resize( custom, Vector3( 8.0f, 9.0f, 10.0f ) );
  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeAnimation", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 8.0f, custom.GetTargetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 9.0f, custom.GetTargetSize().height, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, custom.GetTargetSize().depth, TEST_LOCATION );

  anim.Resize( custom, 1.0f, 2.0f );
  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnSizeAnimation", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );
  DALI_TEST_EQUALS( 1.0f, custom.GetTargetSize().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 2.0f, custom.GetTargetSize().height, TEST_LOCATION );
}

static void UtcDaliCustomActorOnTouchEvent()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnTouchEvent()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // set size for custom actor
  custom.SetSize( 100, 100 );
  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );
  custom.ResetCallStack();

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // simulate a touch event
  Dali::TouchPoint point( 0, TouchPoint::Down, 1, 1 );
  Dali::Integration::TouchEvent event;
  event.AddPoint( point );
  application.GetCore().SendEvent( event );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnTouchEvent", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
}

static void UtcDaliCustomActorOnMouseWheelEvent()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::OnMouseWheelEvent()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  // set size for custom actor
  custom.SetSize( 100, 100 );
  // add the custom actor to stage
  Stage::GetCurrent().Add( custom );
  custom.ResetCallStack();

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // simulate a mouse wheel event
  Vector2 screenCoordinates( 10.0f, 10.0f );
  Integration::MouseWheelEvent event(0, 0u, screenCoordinates, 1, 1000u);
  application.GetCore().SendEvent( event );

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "OnMouseWheelEvent", custom.GetMethodsCalled()[ 0 ], TEST_LOCATION );
}

static void UtcDaliActorFindChildByAlias()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::GetChildByAlias()");

  TestCustomActor custom = TestCustomActor::New();
  DALI_TEST_EQUALS( 0, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  custom.Add(Actor::New());

  DALI_TEST_EQUALS( 1, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );

  DALI_TEST_CHECK( !custom.FindChildByAlias("not-found") );

  DALI_TEST_EQUALS( 2, (int)(custom.GetMethodsCalled().size()), TEST_LOCATION );
  DALI_TEST_EQUALS( "GetChildByAlias", custom.GetMethodsCalled()[ 1 ], TEST_LOCATION );

  DALI_TEST_CHECK( custom.FindChildByAlias("found") );
}

static void UtcDaliCustomActorDoAction()
{
  TestApplication application;
  tet_infoline("Testing Dali::CustomActor::DoAction()");

  TestCustomActor custom = TestCustomActor::New();

  BaseHandle customActorObject = custom;

  DALI_TEST_CHECK(customActorObject);

  std::vector<Property::Value> attributes;

  // Check that an invalid command is not performed
  DALI_TEST_CHECK(customActorObject.DoAction("invalidCommand", attributes) == false);

  // Check that the custom actor is visible
  custom.SetVisible(true);
  DALI_TEST_CHECK(custom.IsVisible() == true);

  // Check the custom actor performed an action to hide itself
  DALI_TEST_CHECK(customActorObject.DoAction("hide", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now invisible
  DALI_TEST_CHECK(custom.IsVisible() == false);

  // Check the custom actor performed an action to show itself
  DALI_TEST_CHECK(customActorObject.DoAction("show", attributes) == true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check that the custom actor is now visible
  DALI_TEST_CHECK(custom.IsVisible() == true);
}

static void UtcDaliCustomActorImplOnPropertySet()
{
  TestApplication application;
  CustomActorImpl* impl = new Impl::SimpleTestCustomActor();

  impl->OnPropertySet( 0, 0 );

  DALI_TEST_CHECK( true );

  delete impl;
}

static void UtcDaliCustomActorGetImplementation()
{
  TestApplication application;

  TestCustomActor custom = TestCustomActor::New();
  CustomActorImpl& impl = custom.GetImplementation();
  impl.GetOwner();  // Test

  const TestCustomActor constCustom = TestCustomActor::New();
  const CustomActorImpl& constImpl = constCustom.GetImplementation();
  constImpl.GetOwner();  // Test

  DALI_TEST_CHECK( true );
}
