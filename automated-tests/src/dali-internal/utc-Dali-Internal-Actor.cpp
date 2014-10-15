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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

// Internal headers are allowed here

#include <dali/internal/event/actors/actor-impl.h>

using namespace Dali;


int UtcDaliActorInternalSetGetLayoutDimensionDirty(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSetGetLayoutDimensionDirty");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Width dirty
  actorImpl.SetLayoutDimensionDirty( Internal::Actor::Width, true );

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Width and height dirty
  actorImpl.SetLayoutDimensionDirty( Internal::Actor::Height, true );

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Height dirty
  actorImpl.SetLayoutDimensionDirty( Internal::Actor::Width, false );

  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // None dirty
  actorImpl.SetLayoutDimensionDirty( Internal::Actor::Height, false );

  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliActorInternalSetGetLayoutDimensionNegotiated(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSetGetLayoutDimensionNegotiated");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Height ) );

  // Width dirty
  actorImpl.SetLayoutDimensionNegotiated( Internal::Actor::Width, true );

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Height ) );

  // Width and height Negotiated
  actorImpl.SetLayoutDimensionNegotiated( Internal::Actor::Height, true );

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Height ) );

  // Height Negotiated
  actorImpl.SetLayoutDimensionNegotiated( Internal::Actor::Width, false );

  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Height ) );

  // None Negotiated
  actorImpl.SetLayoutDimensionNegotiated( Internal::Actor::Height, false );

  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionNegotiated( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliActorInternalSetGetDoesRelayout(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSetGetDoesRelayout");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Default
  DALI_TEST_CHECK( actorImpl.DoesRelayout() );

  // False
  actorImpl.SetDoesRelayout( false );

  DALI_TEST_CHECK( !actorImpl.DoesRelayout() );

  // True
  actorImpl.SetDoesRelayout( true );

  DALI_TEST_CHECK( actorImpl.DoesRelayout() );

  END_TEST;
}

int UtcDaliActorInternalSetGetResizePolicy(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSetGetResizePolicy");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Default
  DALI_TEST_CHECK( actorImpl.GetResizePolicy( Internal::Actor::Width )== Actor::UseNaturalSize );
  DALI_TEST_CHECK( actorImpl.GetResizePolicy( Internal::Actor::Height ) == Actor::UseNaturalSize );

  // Swap to something else
  actorImpl.SetResizePolicy( Internal::Actor::Width, Actor::Fixed );
  actorImpl.SetResizePolicy( Internal::Actor::Height, Actor::FitToChildren );

  DALI_TEST_CHECK( actorImpl.GetResizePolicy( Internal::Actor::Width )== Actor::Fixed );
  DALI_TEST_CHECK( actorImpl.GetResizePolicy( Internal::Actor::Height ) == Actor::FitToChildren );

  END_TEST;
}

int UtcDaliActorInternalIsRelayoutDependentOnChildren(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalIsRelayoutDependentOnChildren");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults are natural size (not dependent)
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Height ) );

  // Swap to positive
  actorImpl.SetResizePolicy( Internal::Actor::Width, Actor::Fixed );
  actorImpl.SetResizePolicy( Internal::Actor::Height, Actor::FitToChildren );

  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Height ) );

  // Swap to positive
  actorImpl.SetResizePolicy( Internal::Actor::Width, Actor::FitToChildren );
  actorImpl.SetResizePolicy( Internal::Actor::Height, Actor::FillToParent );

  DALI_TEST_CHECK( actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnChildren( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliActorInternalIsRelayoutDependentOnParent(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalIsRelayoutDependentOnParent");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults are natural size (not dependent)
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Height ) );

  // Swap to positive
  actorImpl.SetResizePolicy( Internal::Actor::Width, Actor::Fixed );
  actorImpl.SetResizePolicy( Internal::Actor::Height, Actor::FillToParent );

  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Width ));
  DALI_TEST_CHECK( actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Height ) );

  // Swap to positive
  actorImpl.SetResizePolicy( Internal::Actor::Width, Actor::FillToParent );
  actorImpl.SetResizePolicy( Internal::Actor::Height, Actor::DimensionDependent );

  DALI_TEST_CHECK( actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Width ));
  DALI_TEST_CHECK( !actorImpl.IsRelayoutDependentOnParent( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliActorInternalCollectChildrenForRelayout(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalCollectChildrenForRelayout");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );
  Actor child1 = Actor::New();
  Actor child2 = Actor::New();
  Actor child3 = Actor::New();

  child1.SetName( "child1" );
  child2.SetName( "child2" );
  child3.SetName( "child3" );

  actor.Add( child1 );
  actor.Add( child2 );
  actor.Add( child3 );

  // Collect children
  ActorContainer children;
  actorImpl.CollectChildrenForRelayout( children );

  DALI_TEST_CHECK( children.size() == 3 );

  DALI_TEST_CHECK( children[0].GetName() == "child1" );
  DALI_TEST_CHECK( children[1].GetName() == "child2" );
  DALI_TEST_CHECK( children[2].GetName() == "child3" );

  END_TEST;
}

int UtcDaliActorInternalSwapDimension(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSwapDimension");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  DALI_TEST_CHECK( actorImpl.SwapDimension( Internal::Actor::Width )== Internal::Actor::Height );
  DALI_TEST_CHECK( actorImpl.SwapDimension( Internal::Actor::Height ) == Internal::Actor::Width );
  DALI_TEST_CHECK( actorImpl.SwapDimension( Internal::Actor::NoDimension ) == Internal::Actor::NoDimension );
  DALI_TEST_CHECK( actorImpl.SwapDimension( Internal::Actor::AllDimensions ) == Internal::Actor::NoDimension );

  END_TEST;
}

int UtcDaliActorInternalSetGetNegotiatedDimension(void)
{
  TestApplication app;
  tet_infoline(" UtcDaliActorInternalSetGetNegotiatedDimension");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Default
  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Width )== 0.0f );
  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Height ) == 0.0f );

  // Width
  actorImpl.SetNegotiatedDimension( Internal::Actor::Width, 1.0f );

  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Width )== 1.0f );
  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Height ) == 0.0f );

  // Height
  actorImpl.SetNegotiatedDimension( Internal::Actor::Height, 2.0f );

  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Width )== 1.0f );
  DALI_TEST_CHECK( actorImpl.GetNegotiatedDimension( Internal::Actor::Height ) == 2.0f );

  END_TEST;
}

int UtcDaliInternalActorRequestRelayoutBoth(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorRequestRelayoutBoth");

  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Both
  actorImpl.RequestRelayout();

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorRequestRelayoutAlternate(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorRequestRelayoutAlternate");

  {
    Actor actor = Actor::New();
    Internal::Actor& actorImpl = GetImplementation( actor );

    // Defaults
    DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
    DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

    // Only width
    actorImpl.RequestRelayout( Internal::Actor::Width );

    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
    DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

    // Try width a second time
    actorImpl.RequestRelayout( Internal::Actor::Width );

    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
    DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

    // Only height
    actorImpl.RequestRelayout( Internal::Actor::Height );

    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

    // Try height a second time
    actorImpl.RequestRelayout( Internal::Actor::Height );

    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
    DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );
  }

  END_TEST;
}

int UtcDaliInternalActorActorDoesRelayoutTrue(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorActorDoesRelayoutTrue");

  // True case
  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  // Defaults
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Both
  actorImpl.RequestRelayout();

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorActorDoesRelayoutFalse(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorActorDoesRelayoutFalse");

  // False case
  Actor actor = Actor::New();
  Internal::Actor& actorImpl = GetImplementation( actor );

  actorImpl.SetDoesRelayout( false );

  // Defaults
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Both
  actorImpl.RequestRelayout();

  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Now true
  actorImpl.SetDoesRelayout( true );

  // Both
  actorImpl.RequestRelayout();

  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( actorImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateParentNone(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateParentNone");

  Actor child = Actor::New();
  Internal::Actor& childImpl = GetImplementation( child );

  // Defaults
  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just child
  childImpl.RequestRelayout();

  DALI_TEST_CHECK( childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateParentOne(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateParentOne");

  // True case
  Actor parent = Actor::New();
  Internal::Actor& parentImpl = GetImplementation( parent );
  Actor child1 = Actor::New();
  Internal::Actor& child1Impl = GetImplementation( child1 );
  Actor child2 = Actor::New();
  Internal::Actor& child2Impl = GetImplementation( child2 );

  parent.Add( child1 );
  parent.Add( child2 );

  // Defaults
  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just child1
  child1Impl.RequestRelayout();

  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // child2 and parent
  parentImpl.SetResizePolicy( Internal::Actor::Width, Actor::FitToChildren );
  parentImpl.SetResizePolicy( Internal::Actor::Height, Actor::FitToChildren );

  child2Impl.RequestRelayout();

  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateParentOneMixed(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateParentOneMixed");

  // True case
  Actor parent = Actor::New();
  Internal::Actor& parentImpl = GetImplementation( parent );
  Actor child1 = Actor::New();
  Internal::Actor& child1Impl = GetImplementation( child1 );
  Actor child2 = Actor::New();
  Internal::Actor& child2Impl = GetImplementation( child2 );

  parent.Add( child1 );
  parent.Add( child2 );

  // Defaults
  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just child1
  parentImpl.SetResizePolicy( Internal::Actor::Width, Actor::FillToParent );
  parentImpl.SetResizePolicy( Internal::Actor::Height, Actor::FitToChildren );

  child1Impl.RequestRelayout( Internal::Actor::Width );

  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Now try height
  child1Impl.RequestRelayout( Internal::Actor::Height );

  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( parentImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );


  END_TEST;
}

int UtcDaliInternalActorPropagateParentBigTree(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateParentBigTree");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );
  Actor parent1 = Actor::New();
  Internal::Actor& parent1Impl = GetImplementation( parent1 );
  Actor parent2 = Actor::New();
  Internal::Actor& parent2Impl = GetImplementation( parent2 );
  Actor child1 = Actor::New();
  Internal::Actor& child1Impl = GetImplementation( child1 );
  Actor child2 = Actor::New();
  Internal::Actor& child2Impl = GetImplementation( child2 );
  Actor leaf1 = Actor::New();
  Internal::Actor& leaf1Impl = GetImplementation( leaf1 );
  Actor leaf2 = Actor::New();
  Internal::Actor& leaf2Impl = GetImplementation( leaf2 );
  Actor leaf3 = Actor::New();
  Internal::Actor& leaf3Impl = GetImplementation( leaf3 );

  root.Add( parent1 );
  root.Add( parent2 );
  parent1.Add( child1 );
  parent1.Add( child2 );
  child1.Add( leaf1 );
  child1.Add( leaf2 );
  child2.Add( leaf3 );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just child1
  parent1Impl.SetResizePolicy( Internal::Actor::Width, Actor::FitToChildren );
  rootImpl.SetResizePolicy( Internal::Actor::Width, Actor::FitToChildren );

  child1Impl.RequestRelayout();

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateChildrenNone(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateChildrenNone");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just child1
  rootImpl.RequestRelayout();

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateChildrenOneFalse(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateChildrenOneFalse");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );
  Actor child = Actor::New();
  Internal::Actor& childImpl = GetImplementation( child );

  root.Add( child );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just root
  rootImpl.RequestRelayout();

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateChildrenOneTrue(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateChildrenOneTrue");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );
  Actor child = Actor::New();
  Internal::Actor& childImpl = GetImplementation( child );

  root.Add( child );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Just root
  childImpl.SetResizePolicy( Internal::Actor::Width, Actor::FillToParent );
  childImpl.SetResizePolicy( Internal::Actor::Height, Actor::FillToParent );

  rootImpl.RequestRelayout();

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( childImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( childImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateChildrenBigTree(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateChildrenBigTree");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );
  Actor parent1 = Actor::New();
  Internal::Actor& parent1Impl = GetImplementation( parent1 );
  Actor parent2 = Actor::New();
  Internal::Actor& parent2Impl = GetImplementation( parent2 );
  Actor child1 = Actor::New();
  Internal::Actor& child1Impl = GetImplementation( child1 );
  Actor child2 = Actor::New();
  Internal::Actor& child2Impl = GetImplementation( child2 );
  Actor leaf1 = Actor::New();
  Internal::Actor& leaf1Impl = GetImplementation( leaf1 );
  Actor leaf2 = Actor::New();
  Internal::Actor& leaf2Impl = GetImplementation( leaf2 );
  Actor leaf3 = Actor::New();
  Internal::Actor& leaf3Impl = GetImplementation( leaf3 );

  root.Add( parent1 );
  root.Add( parent2 );
  parent1.Add( child1 );
  parent1.Add( child2 );
  child1.Add( leaf1 );
  child1.Add( leaf2 );
  child2.Add( leaf3 );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Set resize policies
  child1Impl.SetResizePolicy( Internal::Actor::Width, Actor::FillToParent );
  child1Impl.SetResizePolicy( Internal::Actor::Height, Actor::FillToParent );

  leaf2Impl.SetResizePolicy( Internal::Actor::Width, Actor::FillToParent );
  leaf2Impl.SetResizePolicy( Internal::Actor::Height, Actor::FillToParent );

  // Just child1

  parent1Impl.RequestRelayout();

  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( parent1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !parent2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( child1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !child2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf1Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( leaf2Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !leaf3Impl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}

int UtcDaliInternalActorPropagateDimensionDependencyOneTrue(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateDimensionDependencyOneTrue");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Test a swap
  rootImpl.SetResizePolicy( Internal::Actor::Width, Actor::DimensionDependent );
  rootImpl.SetResizePolicy( Internal::Actor::Height, Actor::UseNaturalSize );

  rootImpl.RequestRelayout( Internal::Actor::Width );

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}


int UtcDaliInternalActorPropagateDimensionDependencyOneLoop(void)
{
  TestApplication application;
  tet_infoline(" UtcDaliInternalActorPropagateDimensionDependencyOneLoop");

  // True case
  Actor root = Actor::New();
  Internal::Actor& rootImpl = GetImplementation( root );

  // Defaults
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( !rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  // Test a swap
  rootImpl.SetResizePolicy( Internal::Actor::Width, Actor::DimensionDependent );
  rootImpl.SetResizePolicy( Internal::Actor::Height, Actor::DimensionDependent );

  rootImpl.RequestRelayout( Internal::Actor::Width );

  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Width ) );
  DALI_TEST_CHECK( rootImpl.IsLayoutDimensionDirty( Internal::Actor::Height ) );

  END_TEST;
}
