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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

using namespace Dali;

void utc_dali_internal_relayout_controller_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_relayout_controller_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

// Properties to attach to actors for testing
const std::string REQUEST_WIDTH( "REQUEST_WIDTH" );
const std::string REQUEST_HEIGHT( "REQUEST_HEIGHT" );
const std::string EXPECTED_WIDTH_DIRTY( "EXPECTED_WIDTH_DIRTY" );
const std::string EXPECTED_HEIGHT_DIRTY( "EXPECTED_HEIGHT_DIRTY" );

/**
 * Check to see that the desired test results were achieved
 */
void CheckResults( Actor root )
{
  const bool expectedWidthDirty = root.GetProperty( root.GetPropertyIndex( EXPECTED_WIDTH_DIRTY ) ).Get< bool >();
  const bool expectedHeightDirty = root.GetProperty( root.GetPropertyIndex( EXPECTED_HEIGHT_DIRTY ) ).Get< bool >();

  Internal::Actor& rootImpl = GetImplementation( root );

  DALI_TEST_CHECK( rootImpl.IsLayoutDirty( WIDTH ) == expectedWidthDirty );
  DALI_TEST_CHECK( rootImpl.IsLayoutDirty( HEIGHT ) == expectedHeightDirty );

  for( unsigned int i = 0, numChildren = root.GetChildCount(); i < numChildren; ++i )
  {
    Actor child = root.GetChildAt( i );

    CheckResults( child );
  }
}

/**
 * Create a new actor and enable relayout on it
 *
 * @return Return the new actor
 */
Actor NewRelayoutActor( bool expectedWidthDirty, bool expectedHeightDirty, ResizePolicy widthPolicy, ResizePolicy heightPolicy )
{
  Actor actor = Actor::New();

  actor.SetRelayoutEnabled( true );

  actor.SetResizePolicy( widthPolicy, WIDTH );
  actor.SetResizePolicy( heightPolicy, HEIGHT );

  // Expected results for this actor
  actor.RegisterProperty( EXPECTED_WIDTH_DIRTY, expectedWidthDirty );
  actor.RegisterProperty( EXPECTED_HEIGHT_DIRTY, expectedHeightDirty );

  return actor;
}

/**
 * Create a new root actor and enable relayout on it
 *
 * @return Return the new actor
 */
Actor NewRelayoutRootActor( bool requestWidth, bool requestHeight, bool expectedWidthDirty, bool expectedHeightDirty, ResizePolicy widthPolicy, ResizePolicy heightPolicy )
{
  Actor actor = NewRelayoutActor( expectedWidthDirty, expectedHeightDirty, widthPolicy, heightPolicy );

  // Add properties to configure testing
  actor.RegisterProperty( REQUEST_WIDTH, requestWidth );
  actor.RegisterProperty( REQUEST_HEIGHT, requestHeight );

  return actor;
}

void TestTree( Actor root, Actor entryPoint = Actor() )
{
  RelayoutController controller = RelayoutController::Get();
  GetImpl( controller ).SetEnabled( true );

  const bool widthRequest = root.GetProperty( root.GetPropertyIndex( REQUEST_WIDTH ) ).Get< bool >();
  const bool heightRequest = root.GetProperty( root.GetPropertyIndex( REQUEST_HEIGHT ) ).Get< bool >();

  const Dimension dimensions = Dimension( ( ( widthRequest ) ? WIDTH : 0 ) | ( ( heightRequest ) ? HEIGHT : 0 ) );

  RelayoutController::RequestRelayout( ( entryPoint ) ? entryPoint : root, dimensions );

  CheckResults( root );
}

} // anonymous namespace

int UtcDaliRelayoutControllerGet(void)
{
  TestApplication application;

  RelayoutController relayoutController;

  DALI_TEST_CHECK( !relayoutController );

  relayoutController = RelayoutController::Get();

  DALI_TEST_CHECK( relayoutController );

  END_TEST;
}

int UtcDaliRelayoutControllerRequestRelayout(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRelayoutEnabled( true );

  Internal::Actor& actorImpl = GetImplementation( actor );

  // Default
  DALI_TEST_CHECK( !actorImpl.IsLayoutDirty() );

  // Request default enable (false)
  RelayoutController::RequestRelayout( actor );

  DALI_TEST_CHECK( !actorImpl.IsLayoutDirty() );

  // Request enabled
  RelayoutController controller = RelayoutController::Get();
  GetImpl( controller ).SetEnabled( true );

  RelayoutController::RequestRelayout( actor );

  DALI_TEST_CHECK( actorImpl.IsLayoutDirty() );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_SingleActor(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIXED, FIXED );

  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_FixedParent(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIXED, FIXED );

  // Add a child
  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );

  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_NaturalParent(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, USE_NATURAL_SIZE, USE_NATURAL_SIZE );

  // Add a child
  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_FillParent(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FILL_TO_PARENT, FILL_TO_PARENT );

  // Add a child
  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_FitParent(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIT_TO_CHILDREN, FIT_TO_CHILDREN );

  // Add a child
  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_DepParent1(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, DIMENSION_DEPENDENCY, FIT_TO_CHILDREN );

  // Add a child
  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_DepParent2(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIT_TO_CHILDREN, DIMENSION_DEPENDENCY );

  Actor child = NewRelayoutActor( false, false, FIXED, FIXED );
  parent.Add( child );


  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Child1(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIT_TO_CHILDREN, FIT_TO_CHILDREN );

  // Add a child
  Actor child = NewRelayoutActor( true, true, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent, child );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Child2(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, false, false, FIXED, FIXED );

  // Add a child
  Actor child = NewRelayoutActor( true, true, FIXED, FIXED );
  parent.Add( child );

  // Run the test
  TestTree( parent, child );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Complex1(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, true, true, FIXED, FIXED );

  // Add children
  Actor child1 = NewRelayoutActor( true, false, FILL_TO_PARENT, FIXED );
  parent.Add( child1 );

  Actor child2 = NewRelayoutActor( false, true, FIXED, FILL_TO_PARENT );
  parent.Add( child2 );

  Actor child3 = NewRelayoutActor( false, false, USE_NATURAL_SIZE, FIXED );
  parent.Add( child3 );

  // Grand children 1
  Actor grandChild1_1 = NewRelayoutActor( true, false, FILL_TO_PARENT, FIXED );
  child1.Add( grandChild1_1 );

  Actor grandChild1_2 = NewRelayoutActor( false, false, FIXED, FILL_TO_PARENT );
  child1.Add( grandChild1_2 );

  // Grand children 2
  Actor grandChild2_1 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_1 );

  Actor grandChild2_2 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_2 );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Complex2(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, false, true, false, FIXED, FIXED );

  // Add children
  Actor child1 = NewRelayoutActor( true, false, FILL_TO_PARENT, FIXED );
  parent.Add( child1 );

  Actor child2 = NewRelayoutActor( false, false, FIXED, FILL_TO_PARENT );
  parent.Add( child2 );

  Actor child3 = NewRelayoutActor( false, false, USE_NATURAL_SIZE, FIXED );
  parent.Add( child3 );

  // Grand children 1
  Actor grandChild1_1 = NewRelayoutActor( true, false, FILL_TO_PARENT, FIXED );
  child1.Add( grandChild1_1 );

  Actor grandChild1_2 = NewRelayoutActor( false, false, FIXED, FILL_TO_PARENT );
  child1.Add( grandChild1_2 );

  // Grand children 2
  Actor grandChild2_1 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_1 );

  Actor grandChild2_2 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_2 );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Complex3(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( false, true, false, true, FIXED, FIXED );

  // Add children
  Actor child1 = NewRelayoutActor( false, false, FILL_TO_PARENT, FIXED );
  parent.Add( child1 );

  Actor child2 = NewRelayoutActor( false, true, FIXED, FILL_TO_PARENT );
  parent.Add( child2 );

  Actor child3 = NewRelayoutActor( false, false, USE_NATURAL_SIZE, FIXED );
  parent.Add( child3 );

  // Grand children 1
  Actor grandChild1_1 = NewRelayoutActor( false, false, FILL_TO_PARENT, FIXED );
  child1.Add( grandChild1_1 );

  Actor grandChild1_2 = NewRelayoutActor( false, false, FIXED, FILL_TO_PARENT );
  child1.Add( grandChild1_2 );

  // Grand children 2
  Actor grandChild2_1 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_1 );

  Actor grandChild2_2 = NewRelayoutActor( false, false, FIXED, FIXED );
  child2.Add( grandChild2_2 );

  // Run the test
  TestTree( parent );

  END_TEST;
}

int UtcDaliRelayoutController_Relayout_Dependency(void)
{
  TestApplication application;

  // Construct scene
  Actor parent = NewRelayoutRootActor( true, true, false, false, FIXED, FIXED );

  // Add a child
  Actor child = NewRelayoutActor( true, true, FILL_TO_PARENT, FIXED );
  child.SetDimensionDependency( HEIGHT, WIDTH );
  parent.Add( child );

  // Run the test
  TestTree( parent, child );

  END_TEST;
}

int UtcDaliRelayoutControllerRequestRelayoutTree(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetRelayoutEnabled( true );

  Internal::Actor& actorImpl = GetImplementation( actor );

  // Check if flag is set
  DALI_TEST_CHECK( !actorImpl.IsLayoutDirty() );

  RelayoutController controller = RelayoutController::Get();
  GetImpl( controller ).SetEnabled( true );

  // Request default enable (false)
  RelayoutController::Get().RequestRelayoutTree( actor );

  DALI_TEST_CHECK( actorImpl.IsLayoutDirty() );

  END_TEST;
}

