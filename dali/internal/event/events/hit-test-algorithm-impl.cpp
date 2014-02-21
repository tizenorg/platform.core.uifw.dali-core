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

// CLASS HEADER
#include <dali/internal/event/events/hit-test-algorithm-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/system-overlay.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/common/system-overlay-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

namespace Dali
{

namespace Internal
{

namespace HitTestAlgorithm
{

namespace
{

struct HitActor
{
  HitActor()
  : x( 0 ),
    y( 0 ),
    distance( std::numeric_limits<float>::max() ),
    overlay( false )
  {

  }

  Dali::Actor actor;                    ///< the actor hit. (if actor hit, then initialised)
  float x;                              ///< x position of hit (only valid if actor valid)
  float y;                              ///< y position of hit (only valid if actor valid)
  float distance;                       ///< distance from ray origin to hit actor
  bool overlay;                         ///< true if the hit actor is an overlay

};

/**
 * The function to be used in the hit-test algorithm to check whether the actor is touchable.
 * It is used by the touch event and gesture processor.
 */
bool IsActorTouchableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if( GetImplementation(actor).GetTouchRequired() && // Does the Application or derived actor type require a touch event?
          GetImplementation(actor).IsHittable() ) // Is actor sensitive, visible and on the scene?
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() &&     // Actor is visible, if not visible then none of its children are visible.
          actor.IsSensitive() )    // Actor is sensitive, if insensitive none of its children should be hittable either.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
}

/**
 * Recursively hit test all the actors, without crossing into other layers.
 * This algorithm performs a Depth-First-Search (DFS) on all Actors within Layer.
 * Hit-Testing each Actor, noting the distance from the Ray-Origin (3D origin
 * of touch vector). The closest Hit-Tested Actor is that which is returned.
 * Exceptions to this rule are:
 * - If the Actor is an overlay then it is considered closer than all previous
 * overlays encountered in the hit test traversal.
 * - When comparing against renderable parents, if Actor is the same distance
 * or closer than it's renderable parent, then it takes priority.
 */
HitActor HitTestWithinLayer( Actor& actor,
                             const Vector4& rayOrigin,
                             const Vector4& rayDir,
                             bool worldOverlay,
                             float& nearClippingPlane,
                             float& farClippingPlane,
                             Dali::HitTestAlgorithm::HitTestFunction func,
                             bool& stencilOnLayer,
                             bool& stencilHit,
                             bool parentIsStencil )
{
  worldOverlay |= actor.IsOverlay();

  HitActor hit;

  // Children should inherit the stencil draw mode
  bool isStencil = parentIsStencil;

  if ( actor.GetDrawMode() == DrawMode::STENCIL && actor.IsVisible() )
  {
    isStencil = true;
    stencilOnLayer = true;
  }

  // If we are a stencil or hittable...
  if ( isStencil || func(Dali::Actor(&actor), Dali::HitTestAlgorithm::CHECK_ACTOR) ) // Is actor hittable
  {
    Vector3 size( actor.GetCurrentSize() );

    if ( size.x > 0.0f && size.y > 0.0f &&              // Ensure the actor has a valid size.
         actor.RaySphereTest( rayOrigin, rayDir ) ) // Perform quicker ray sphere test to see if our ray is close to the actor.
    {
      Vector4 hitPointLocal;
      float distance;

      // Finally, perform a more accurate ray test to see if our ray actually hits the actor.
      if( actor.RayActorTest( rayOrigin, rayDir, hitPointLocal, distance ) )
      {
        if( distance >= nearClippingPlane && distance <= farClippingPlane )
        {
          // If the hit has happened on a stencil then register, but don't record as hit result
          if ( isStencil )
          {
            stencilHit = true;
          }
          else
          {
            hit.actor = Dali::Actor(&actor);
            hit.x = hitPointLocal.x;
            hit.y = hitPointLocal.y;
            hit.distance = distance;
            hit.overlay = worldOverlay;
          }
        }
      }
    }
  }

  // If there is a stencil on this layer and we've also registered a hit, then don't both searching any children
  if ( stencilHit && hit.actor )
  {
    return hit;
  }

  // Find a child hit, until we run out of actors in the current layer.
  HitActor childHit;
  if( actor.GetChildCount() > 0 )
  {
    childHit.distance = std::numeric_limits<float>::max();
    Dali::ActorContainer& children = actor.GetChildrenInternal();

    // Hit test ALL children and calculate their distance.
    bool parentIsRenderable = actor.IsRenderable();

    for (Dali::ActorIter iter = children.begin(), endIter = children.end(); iter != endIter; ++iter)
    {
      // Descend tree only if...
      if ( !iter->IsLayer() &&    // Child is NOT a layer, hit testing current layer only or Child is not a layer and we've inherited the stencil draw mode
           ( isStencil || func(*iter, Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE ) ) ) // Child is visible and sensitive, otherwise none of its children should be hittable.
      {
        HitActor currentHit( HitTestWithinLayer( GetImplementation(*iter), rayOrigin, rayDir, worldOverlay, nearClippingPlane, farClippingPlane, func, stencilOnLayer, stencilHit, isStencil ) );

        // If Current child is an overlay, then it takes priority.
        // If it is not an overlay, and the previously hit sibling is also not an overlay, then closest takes priority.
        // (last overlay sibling has priority as is rendered on top)
        if ( currentHit.distance >= 0.f && (currentHit.overlay || (!childHit.overlay && currentHit.distance < childHit.distance) ) )
        {
          if ( !parentIsRenderable )
          {
            // If our parent is not renderable, then child should be hit regardless of distance.
            childHit = currentHit;
          }
          else if ( currentHit.overlay || (!hit.overlay && currentHit.distance <= hit.distance) )
          {
            // If our parent is renderable, then child should only be hit if it is an overlay, or if it is closer than a non-overlay.
            // (child overlay has priority as is rendered on top of it's parent)
            childHit = currentHit;
          }
        }
      }
    }
  }
  return ( childHit.actor ) ? childHit : hit;
}

/**
 * Return true if actor is sourceActor or a descendent of sourceActor
 */
bool IsWithinSourceActors( const Actor& sourceActor, const Actor& actor )
{
  if ( &sourceActor == &actor )
  {
    return true;
  }
  else
  {
    Actor* parent = actor.GetParent();
    if ( parent )
    {
      return IsWithinSourceActors( sourceActor, *parent );
    }
  }

  // Not within source actors
  return false;
}

/**
 * Returns true if the layer and all of the layer's parents are visible and sensitive.
 */
inline bool IsActuallyHittable( Layer& layer, const Vector2& screenCoordinates, const Vector2& stageSize, Dali::HitTestAlgorithm::HitTestFunction func )
{
  bool hittable( true );

  if(layer.IsClipping())
  {
    ClippingBox box = layer.GetClippingBox();

    if( screenCoordinates.x < box.x ||
        screenCoordinates.x > box.x + box.width ||
        screenCoordinates.y < stageSize.y - (box.y + box.height) ||
        screenCoordinates.y > stageSize.y - box.y)
    {
      // Not touchable if clipping is enabled in the layer and the screen coordinate is outside the clip region.
      hittable = false;
    }
  }

  if(hittable)
  {
    Actor* actor( &layer );
    while ( actor && hittable )
    {
      if ( !(func(Dali::Actor(actor), Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE)) ) // Layer (or its Parent) is NOT visible and sensitive, so our layer is not either.
      {
        hittable = false;
        break;
      }
      actor = actor->GetParent();
    }
  }

  return hittable;
}

/**
 * Gets the near and far clipping planes of the camera from which the scene is viewed in the render task.
 */
void GetCameraClippingPlane( RenderTask& renderTask, float& nearClippingPlane, float& farClippingPlane )
{
  CameraActor* cameraActor = renderTask.GetCameraActor();
  nearClippingPlane = cameraActor->GetNearClippingPlane();
  farClippingPlane = cameraActor->GetFarClippingPlane();
}

/**
 * Hit test a RenderTask
 */
bool HitTestRenderTask( LayerList& layers,
                        RenderTask& renderTask,
                        Vector2 screenCoordinates,
                        Results& results,
                        Dali::HitTestAlgorithm::HitTestFunction func )
{
  if ( renderTask.IsHittable( screenCoordinates ) )
  {
    Viewport viewport;
    renderTask.GetViewport( viewport );
    if( screenCoordinates.x < viewport.x ||
        screenCoordinates.x > viewport.x + viewport.width ||
        screenCoordinates.y < viewport.y ||
        screenCoordinates.y > viewport.y + viewport.height )
    {
      // The screen coordinate is outside the viewport of render task. The viewport clips all layers.
      return false;
    }

    float nearClippingPlane, farClippingPlane;
    GetCameraClippingPlane(renderTask, nearClippingPlane, farClippingPlane);

    // Determine the layer depth of the source actor
    Actor* sourceActor( renderTask.GetSourceActor() );
    if ( sourceActor )
    {
      Dali::Layer layer( sourceActor->GetLayer() );
      if ( layer )
      {
        const unsigned int sourceActorDepth( layer.GetDepth() );

        CameraActor* cameraActor = renderTask.GetCameraActor();
        bool pickingPossible = cameraActor->BuildPickingRay( screenCoordinates, viewport, results.rayOrigin, results.rayDirection );
        if( !pickingPossible )
        {
          return false;
        }

        // Hit test starting with the top layer, working towards the bottom layer.
        HitActor hit;
        bool stencilOnLayer = false;
        bool stencilHit = false;
        const Vector2& stageSize = Stage::GetCurrent()->GetSize();

        for (int i=layers.GetLayerCount()-1; i>=0 && !(hit.actor); --i)
        {
          Layer* layer( layers.GetLayer(i) );

          HitActor previousHit = hit;
          stencilOnLayer = false;
          stencilHit = false;

          // Ensure layer is touchable (also checks whether ancestors are also touchable)
          if ( IsActuallyHittable ( *layer, screenCoordinates, stageSize, func ) )
          {
            // Always hit-test the source actor; otherwise test whether the layer is below the source actor in the hierarchy
            if ( sourceActorDepth == static_cast<unsigned int>(i) )
            {
              // Recursively hit test the source actor & children, without crossing into other layers.
              hit = HitTestWithinLayer( *sourceActor, results.rayOrigin, results.rayDirection, false, nearClippingPlane, farClippingPlane, func, stencilOnLayer, stencilHit, false );
            }
            else if ( IsWithinSourceActors( *sourceActor, *layer ) )
            {
              // Recursively hit test all the actors, without crossing into other layers.
              hit = HitTestWithinLayer( *layer, results.rayOrigin, results.rayDirection, false, nearClippingPlane, farClippingPlane, func, stencilOnLayer, stencilHit, false );
            }
            // If a stencil on this layer hasn't been hit, then discard hit results for this layer
            if ( stencilOnLayer && !stencilHit )
            {
             hit = previousHit;
            }
          }
        }
        if ( hit.actor )
        {
          results.renderTask = Dali::RenderTask(&renderTask);
          results.actor = hit.actor;
          results.actorCoordinates.x = hit.x;
          results.actorCoordinates.y = hit.y;
          return true; // Success
        }
      }
    }
  }
  return false;
}

/**
 * Iterate through RenderTaskList and perform hit test.
 */

void HitTestForEachRenderTask( LayerList& layers,
                               RenderTaskList& taskList,
                               const Vector2& screenCoordinates,
                               Results& results,
                               Dali::HitTestAlgorithm::HitTestFunction func )
{
  RenderTaskList::RenderTaskContainer& tasks = taskList.GetTasks();
  RenderTaskList::RenderTaskContainer::reverse_iterator endIter = tasks.rend();

  // Check onscreen tasks before offscreen ones, hit test order should be reverse of draw order (see ProcessRenderTasks() where offscreen tasks are drawn first).

  // on screen
  for ( RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = GetImplementation( *iter );
    Dali::FrameBufferImage frameBufferImage = renderTask.GetTargetFrameBuffer();

    // Note that if frameBufferImage is NULL we are using the default (on screen) render target
    if(frameBufferImage)
    {
      ResourceId id = GetImplementation(frameBufferImage).GetResourceId();

      // on screen only
      if(0 != id)
      {
        // Skip to next task
        continue;
      }
    }

    if ( HitTestRenderTask( layers, renderTask, screenCoordinates, results, func ) )
    {
      // Exit when an actor is hit
      return; // don't bother checking off screen tasks
    }
  }

  // off screen
  for ( RenderTaskList::RenderTaskContainer::reverse_iterator iter = tasks.rbegin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = GetImplementation( *iter );
    Dali::FrameBufferImage frameBufferImage = renderTask.GetTargetFrameBuffer();

    // Note that if frameBufferImage is NULL we are using the default (on screen) render target
    if(frameBufferImage)
    {
      ResourceId id = GetImplementation(frameBufferImage).GetResourceId();

      // off screen only
      if(0 == id)
      {
        // Skip to next task
        continue;
      }

      if ( HitTestRenderTask( layers, renderTask, screenCoordinates, results, func ) )
      {
        // Exit when an actor is hit
        break;
      }
    }
  }
}

} // unnamed namespace

void HitTest( Stage& stage, const Vector2& screenCoordinates, Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func )
{
  // Hit-test the regular on-stage actors
  RenderTaskList& taskList = stage.GetRenderTaskList();
  LayerList& layerList = stage.GetLayerList();

  Results hitTestResults;
  HitTestForEachRenderTask( layerList, taskList, screenCoordinates, hitTestResults, func );

  results.actor = hitTestResults.actor;
  results.actorCoordinates = hitTestResults.actorCoordinates;
}

void HitTest( Stage& stage, const Vector2& screenCoordinates, Results& results )
{
  // Hit-test the system-overlay actors first
  SystemOverlay* systemOverlay = stage.GetSystemOverlayInternal();

  if ( systemOverlay )
  {
    RenderTaskList& overlayTaskList = systemOverlay->GetOverlayRenderTasks();
    LayerList& overlayLayerList = systemOverlay->GetLayerList();

    HitTestForEachRenderTask( overlayLayerList, overlayTaskList, screenCoordinates, results, IsActorTouchableFunction );
  }

  // Hit-test the regular on-stage actors
  if ( !results.actor )
  {
    RenderTaskList& taskList = stage.GetRenderTaskList();
    LayerList& layerList = stage.GetLayerList();

    HitTestForEachRenderTask( layerList, taskList, screenCoordinates, results, IsActorTouchableFunction );
  }
}

void HitTest( Stage& stage, RenderTask& renderTask, const Vector2& screenCoordinates,
              Dali::HitTestAlgorithm::Results& results, Dali::HitTestAlgorithm::HitTestFunction func )
{
  Results hitTestResults;
  HitTestRenderTask( stage.GetLayerList(), renderTask, screenCoordinates, hitTestResults, func );
  results.actor = hitTestResults.actor;
  results.actorCoordinates = hitTestResults.actorCoordinates;
}

} // namespace HitTestAlgorithm

} // namespace Internal

} // namespace Dali
