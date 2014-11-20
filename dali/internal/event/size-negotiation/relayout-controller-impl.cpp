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

// FILE HEADER

#include "relayout-controller-impl.h"

// EXTERNAL INCLUDES
#if defined(DEBUG_ENABLED)
#include <sstream>
#include <dali/internal/event/common/system-overlay-impl.h>
#endif // defined(DEBUG_ENABLED)

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace Internal
{

namespace
{
#if defined(DEBUG_ENABLED)

Integration::Log::Filter* gLogFilter( Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_RELAYOUT_CONTROLLER") );

/**
 * Prints out all the children of the given actor when debug is enabled.
 *
 * @param[in]  actor  The actor whose children to print.
 * @param[in]  level  The number of " | " to put in front of the children.
 */
void PrintChildren( Dali::Actor actor, int level )
{
  std::ostringstream output;

  for ( int t = 0; t < level; ++t )
  {
    output << " | ";
  }

  output << actor.GetTypeName();

  output << ", " << actor.GetName();

  output << " - Pos: " << actor.GetCurrentPosition() << " Size: " << actor.GetTargetSize();

  output << ", Dirty: (" << ( GetImplementation( actor ).IsLayoutDirty( WIDTH ) ? "TRUE" : "FALSE" ) << "," << ( GetImplementation( actor ).IsLayoutDirty( HEIGHT ) ? "TRUE" : "FALSE" ) << ")";
  output << ", Negotiated: (" << ( GetImplementation( actor ).IsLayoutNegotiated( WIDTH ) ? "TRUE" : "FALSE" ) << "," << ( GetImplementation( actor ).IsLayoutNegotiated( HEIGHT ) ? "TRUE" : "FALSE" ) << ")";
  output << ", Enabled: " << ( actor.IsRelayoutEnabled() ? "TRUE" : "FALSE" );

  output << ", (" << actor.GetObjectPtr() << ")" << std::endl;

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, output.str().c_str() );

  ++level;
  unsigned int numChildren = actor.GetChildCount();
  for( unsigned int i=0; i<numChildren; ++i )
  {
    PrintChildren( actor.GetChildAt(i), level );
  }
  --level;
}

/**
 * Prints the entire hierarchy of the scene.
 */
void PrintHierarchy()
{
  if ( gLogFilter->IsEnabledFor( Debug::Verbose ) )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "---------- ROOT LAYER ----------\n" );
    PrintChildren( Dali::Stage().GetCurrent().GetRootLayer(), 0 );
  }
}

#define PRINT_HIERARCHY PrintHierarchy()

#else // defined(DEBUG_ENABLED)

#define PRINT_HIERARCHY

#endif // defined(DEBUG_ENABLED)

/**
 * Sets the target to source if the individual elements of source are NOT zero.
 *
 * @param[out] target The Vector2 elements to set if the source Vector2 elements are not 0.
 * @param[in] source The Vector2 elements that are to be set to the target Vector2.
 */
void SetIfNotZero( Vector2& target, const Vector2& source )
{
  // Only set the width if it is non zero.
  if ( !EqualsZero( source.width ) )
  {
    target.width = source.width;
  }

  // Only set the height if it is non zero.
  if ( !EqualsZero( source.height ) )
  {
    target.height = source.height;
  }
}

} // unnamed namespace

RelayoutController* RelayoutController::Get()
{
  return &ThreadLocalStorage::Get().GetRelayoutController();
}

RelayoutController::RelayoutController()
: mRelayoutInfoAllocator(),
  mRelayoutStack( new MemoryPoolRelayoutContainer( mRelayoutInfoAllocator ) ),
  mRelayoutConnection( false ),
  mRelayoutFlag( false ),
  mEnabled( false )
{
  // Make space for 32 controls to avoid having to copy construct a lot in the beginning
  mRelayoutStack->Reserve( 32 );
}

RelayoutController::~RelayoutController()
{
  delete mRelayoutStack;
}

void RelayoutController::QueueActor( Dali::Actor& actor, RelayoutContainer& actors, Vector2 size )
{
  if( GetImplementation( actor ).RelayoutRequired() )
  {
    actors.Add( actor, size );
  }
}

void RelayoutController::RequestRelayout( Dali::Actor& actor, Dimension dimension )
{
  if( !mEnabled )
  {
    return;
  }

  Dali::ActorContainer potentialRedundantSubRoots;
  Dali::ActorContainer topOfSubTreeStack;

  topOfSubTreeStack.push_back( actor );

  // Propagate on all dimensions
  for( unsigned int i = 0; i < DIMENSION_COUNT; ++i )
  {
    if( dimension & ( 1 << i ) )
    {
      // Do the propagation
      PropagateAll( actor, static_cast< Dimension >( 1 << i ), topOfSubTreeStack, potentialRedundantSubRoots );
    }
  }

  // Request this actor as head of sub-tree if it is not dependent on a parent that is dirty
  Dali::Actor subTreeActor = topOfSubTreeStack.back();
  Dali::Actor parent = subTreeActor.GetParent();
  if( !parent || !( GetImplementation( subTreeActor ).RelayoutDependentOnParent() && GetImplementation( parent ).RelayoutRequired() ) )
  {
    // Add sub tree root to relayout list
    AddRequest( subTreeActor );

    // Flag request for end of frame
    Request();
  }
  else
  {
    potentialRedundantSubRoots.push_back( subTreeActor );
  }

  // Remove any redundant sub-tree heads
  for( ActorContainer::iterator it = potentialRedundantSubRoots.begin(), itEnd = potentialRedundantSubRoots.end(); it != itEnd; ++it )
  {
    Dali::Actor subRoot = *it;

    RemoveRequest( subRoot );
  }
}

void RelayoutController::OnApplicationSceneCreated()
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "[Internal::RelayoutController::OnApplicationSceneCreated]\n" );

  // Open relayout controller to receive relayout requests
  mEnabled = true;

  // Spread the dirty flag through whole tree - don't need to explicity
  // add request on rootLayer as it will automatically be added below.
  Dali::Actor rootLayer = Dali::Stage::GetCurrent().GetRootLayer();
  RequestRelayoutTree( rootLayer );

  // Flag request for end of frame
  Request();
}

void RelayoutController::RequestRelayoutTree( Dali::Actor& actor )
{
  if( !mEnabled )
  {
    return;
  }

  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.RelayoutPossible() )
  {
    // If parent is not in relayout we are at the top of a new sub-tree
    Dali::Actor parent = actor.GetParent();
    if( !parent || !parent.IsRelayoutEnabled() )
    {
      AddRequest( actor );
    }

    // Set dirty flag on actors that are enabled
    actorImpl.SetLayoutDirty( true );
    actorImpl.SetLayoutNegotiated( false );    // Reset this flag ready for next relayout
  }

  // Propagate down to children
  for( unsigned int i = 0; i < actor.GetChildCount(); ++i )
  {
    Dali::Actor child = actor.GetChildAt( i );

    RequestRelayoutTree( child );
  }
}

void RelayoutController::PropagateAll( Dali::Actor& actor, Dimension dimension, Dali::ActorContainer& topOfSubTreeStack, Dali::ActorContainer& potentialRedundantSubRoots )
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.RelayoutPossible( dimension ) )
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty( true, dimension );
    actorImpl.SetLayoutNegotiated( false, dimension );    // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for( unsigned int i = 0; i < DIMENSION_COUNT; ++i )
    {
      Dimension dimensionToCheck = static_cast< Dimension >( 1 << i );

      if( actorImpl.RelayoutDependentOnDimension( dimension, dimensionToCheck ) &&
          !actorImpl.IsLayoutDirty( dimensionToCheck ) )
      {
        PropagateAll( actor, dimensionToCheck, topOfSubTreeStack, potentialRedundantSubRoots );
      }
    }

    // Propagate up to parent
    Dali::Actor parent = actor.GetParent();
    if( parent )
    {
      Actor& parentImpl = GetImplementation( parent );
      if( parentImpl.RelayoutDependentOnChildren( dimension ) && !parentImpl.IsLayoutDirty( dimension ) )
      {
        // Store the highest parent reached
        bool found = false;
        for( unsigned int i = 0, count = topOfSubTreeStack.size(); i < count; ++i )
        {
          if( topOfSubTreeStack[ i ] == parent )
          {
            found = true;
            break;
          }
        }

        if( !found )
        {
          topOfSubTreeStack.push_back( parent );
        }

        // Propagate up
        PropagateAll( parent, dimension, topOfSubTreeStack, potentialRedundantSubRoots );
      }
    }

    // Propagate down to children
    for( unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
    {
      Dali::Actor child = actor.GetChildAt( i );
      Actor& childImpl = GetImplementation( child );

      if( childImpl.IsRelayoutEnabled() && childImpl.RelayoutDependentOnParent( dimension ) )
      {
        if( childImpl.IsLayoutDirty( dimension ) )
        {
          // We have found a child that could potentially have already been collected for relayout
          potentialRedundantSubRoots.push_back( child );
        }
        else
        {
          PropagateAll( child, dimension, topOfSubTreeStack, potentialRedundantSubRoots );
        }
      }
    }
  }
}


void RelayoutController::PropagateFlags( Dali::Actor& actor, Dimension dimension )
{
  // Only set dirty flag if doing relayout and not already marked as dirty
  Actor& actorImpl = GetImplementation( actor );
  if( actorImpl.IsRelayoutEnabled() )
  {
    // Set dirty and negotiated flags
    actorImpl.SetLayoutDirty( true, dimension );
    actorImpl.SetLayoutNegotiated( false, dimension );    // Reset this flag ready for next relayout

    // Check for dimension dependecy: width for height/height for width etc
    // Check each possible dimension and see if it is dependent on the input one
    for( unsigned int i = 0; i < DIMENSION_COUNT; ++i )
    {
      Dimension dimensionToCheck = static_cast< Dimension >( 1 << i );

      if( actorImpl.RelayoutDependentOnDimension( dimension, dimensionToCheck ) )
      {
        PropagateFlags( actor, dimensionToCheck );
      }
    }

    // Propagate up to parent
    Dali::Actor parent = actor.GetParent();
    if( parent )
    {
      Actor& parentImpl = GetImplementation( parent );
      if( parentImpl.RelayoutDependentOnChildren( dimension ) )
      {
        // Propagate up
        PropagateFlags( parent, dimension );
      }
    }

    // Propagate down to children
    for( unsigned int i = 0, childCount = actor.GetChildCount(); i < childCount; ++i )
    {
      Dali::Actor child = actor.GetChildAt( i );
      Actor& childImpl = GetImplementation( child );

      if( childImpl.RelayoutDependentOnParent( dimension ) )
      {
        PropagateFlags( child, dimension );
      }
    }
  }
}

void RelayoutController::AddRequest( Dali::Actor& actor )
{
  BaseObject* actorPtr = &GetImplementation( actor );

  // Only add the rootActor if it is not already recorded
  bool found = false;
  for( unsigned int i = 0, count = mDirtyLayoutSubTrees.Size(); i < count; ++i )
  {
    if( mDirtyLayoutSubTrees[ i ] == actorPtr )
    {
      found = true;
      break;
    }
  }

  if( !found )
  {
    mDirtyLayoutSubTrees.PushBack( actorPtr );
  }
}

void RelayoutController::RemoveRequest( Dali::Actor& actor )
{
  BaseObject* actorPtr = &GetImplementation( actor );

  // Remove actor from dirty sub trees
  for( RawActorList::Iterator it = mDirtyLayoutSubTrees.Begin(), itEnd = mDirtyLayoutSubTrees.End(); it != itEnd; ++it )
  {
    if( *it == actorPtr )
    {
      mDirtyLayoutSubTrees.Erase( it );
      break;
    }
  }
}

void RelayoutController::Request()
{
  mRelayoutFlag = true;

  if( !mRelayoutConnection )
  {
    Dali::Stage stage = Dali::Stage::GetCurrent();
    stage.GetObjectRegistry().ObjectDestroyedSignal().Connect( this, &RelayoutController::OnObjectDestroyed );

    mRelayoutConnection = true;
  }
}

void RelayoutController::OnObjectDestroyed( const Dali::RefObject* object )
{
  // Search for and null the object if found in the following lists
  FindAndZero( mDirtyLayoutSubTrees, object );
}

void RelayoutController::Relayout()
{
  // Only do something when requested
  if( mRelayoutFlag )
  {
    // Clear the flag as we're now doing the relayout
    mRelayoutFlag = false;

    // 1. Finds all top-level controls from the dirty list and allocate them the size of the stage
    //    These controls are paired with the parent/stage size and added to the stack.
    const Vector2 stageSize = Dali::Stage::GetCurrent().GetSize();

    for( RawActorList::Iterator it = mDirtyLayoutSubTrees.Begin(), itEnd = mDirtyLayoutSubTrees.End(); it != itEnd; ++it )
    {
      BaseObject* dirtyActor = *it;

      // Need to test if actor is valid (could have been deleted and had the pointer cleared)
      if( dirtyActor )
      {
        // We know that BaseObject is a base class of Internal::Actor but need to instruct the compiler to do the cast
        Dali::Actor actor = Dali::Actor( reinterpret_cast<Dali::Internal::Actor*>( dirtyActor ) );

        // Only negotiate actors that are on stage
        if( actor.OnStage() )
        {
          Dali::Actor parent = actor.GetParent();
          QueueActor( actor, *mRelayoutStack, ( parent ) ? Vector2( parent.GetTargetSize() ) : stageSize );
        }
      }
    }

    mDirtyLayoutSubTrees.Clear();

    // 2. Iterate through the stack until it's empty.
    if( mRelayoutStack->Size() > 0 )
    {
      PRINT_HIERARCHY;

      while( mRelayoutStack->Size() > 0 )
      {
        Dali::Actor actor;
        Vector2 size;
        mRelayoutStack->Get( mRelayoutStack->Size() - 1, actor, size );
        Actor& actorImpl = GetImplementation( actor );
        mRelayoutStack->PopBack();

        if( actorImpl.RelayoutRequired() )
        {
          DALI_LOG_INFO( gLogFilter, Debug::General, "[Internal::RelayoutController::Relayout] Negotiating %p %s %s (%.2f, %.2f)\n", &actorImpl, actor.GetTypeName().c_str(), actor.GetName().c_str(), size.width, size.height );

          // 3. Negotiate the size with the current actor. Pass it an empty container which the actor
          //    has to fill with all the actors it has not done any size negotiation for.
          actorImpl.NegotiateSize( size, *mRelayoutStack );
        }
      }

      // We are done with the RelayoutInfos now so delete the pool
      mRelayoutInfoAllocator.ResetMemoryPool();

      PRINT_HIERARCHY;
    }
  }
  // should not disconnect the signal as that causes some control size negotiations to not work correctly
  // this algorithm needs more optimization as well
}

void RelayoutController::SetEnabled( bool enabled )
{
  mEnabled = enabled;
}

void RelayoutController::FindAndZero( const RawActorList& list, const Dali::RefObject* object )
{
  // Object has been destroyed so clear it from this list
  for( RawActorList::Iterator it = list.Begin(), itEnd = list.End(); it != itEnd; ++it )
  {
    BaseObject* actor = *it;

    if( actor && ( actor == object ) )
    {
      *it = NULL;    // Reset the pointer in the list. We don't want to remove it in case something is iterating over the list.
    }
  }
}

} // namespace Internal

} // namespace Dali
