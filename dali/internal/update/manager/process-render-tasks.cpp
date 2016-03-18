/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/manager/process-render-tasks.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/integration-api/debug.h>
//todor
#include <iostream>

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gRenderTaskLogFilter;
#endif

namespace Dali
{

namespace Internal
{


namespace SceneGraph
{

namespace //Unnamed namespace
{

// Return false if the node or it's parents are exclusive to another render-task
bool CheckExclusivity( const Node& node, const RenderTask& task )
{
  const RenderTask* exclusiveTo = node.GetExclusiveRenderTask();
  if ( exclusiveTo )
  {
    if ( exclusiveTo == &task )
    {
      // Exclusive to the same render-task
      return true;
    }
    else
    {
      // Exclusive to another render-task
      return false;
    }
  }

  const Node* parent = node.GetParent();
  if ( parent )
  {
    return CheckExclusivity( *parent, task );
  }

  // No exclusive flags set
  return true;
}

Layer* FindLayer( Node& node )
{
  Node* currentNode( &node );
  Layer* layer(NULL);
  while( currentNode )
  {
    if( (layer = currentNode->GetLayer()) != NULL )
    {
      return layer;
    }

    currentNode = currentNode->GetParent();
  }

  return NULL;
}

/**
 * Rebuild the Layer::colorRenderables, stencilRenderables and overlayRenderables members,
 * including only renderers which are included in the current render-task.
 * Returns true if all renderers have finished acquiring resources.
 */
//todor this walks tree, so good place to set clipping pre-calc.
//todor Note: ClippingId is passed by reference, so it is permanently modified when traversing back up the tree.
bool AddRenderablesForTask( BufferIndex updateBufferIndex,
                            Node& node,
                            Layer& currentLayer,
                            RenderTask& renderTask,
                            int inheritedDrawMode,
                            int& currentClippingId,
                            int clippingDepth )
{
  bool resourcesFinished = true;

  // Short-circuit for invisible nodes
  if ( !node.IsVisible( updateBufferIndex ) )
  {
    return resourcesFinished;
  }

  // Check whether node is exclusive to a different render-task
  const RenderTask* exclusiveTo = node.GetExclusiveRenderTask();
  if ( exclusiveTo &&
      exclusiveTo != &renderTask )
  {
    return resourcesFinished;
  }

  // Assume all children go to this layer (if this node is a layer).
  Layer* layer = node.GetLayer();
  if ( layer )
  {
    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = node.GetDrawMode();
  }
  else
  {
    // This node is not a layer.
    layer = &currentLayer;
    inheritedDrawMode |= node.GetDrawMode();
  }

  //todor
#if 0
  Layer* layer = &currentLayer;
  Layer* nodeLayer( node.GetLayer() );
  if ( nodeLayer )
  {
    // All children go to this layer
    layer = nodeLayer;

    // Layers do not inherit the DrawMode from their parents
    inheritedDrawMode = DrawMode::NORMAL;
  }
  //inheritedDrawMode |= node.GetDrawMode();
#endif

  DALI_ASSERT_DEBUG( NULL != layer );

  //bool clippingInfoUpdated = false;//todor
#if 1
  // todor change comment: This must only happen once per node.
  if( node.GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED )
  {
    std::cout << "found clipping mode -------------------------------------------------------------------------------------------------------------" << std::endl;
    ++currentClippingId;
    ++clippingDepth;
  }

  node.SetClippingSortModifier( currentClippingId, clippingDepth );
#endif

  const unsigned int count = node.GetRendererCount();
  for( unsigned int i = 0; i < count; ++i )
  {
    SceneGraph::Renderer* renderer = node.GetRendererAt( i );
    bool ready = false;
    bool complete = false;
    renderer->GetReadyAndComplete( ready, complete );

    DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "Testing renderable:%p ready:%s complete:%s\n", renderer, ready?"T":"F", complete?"T":"F");

    resourcesFinished &= complete;

    if( ready ) // i.e. should be rendered (all resources are available)
    {
      if( DrawMode::STENCIL == inheritedDrawMode )
      {
        layer->stencilRenderables.PushBack( Renderable( &node, renderer ) );
      }
      else if( DrawMode::OVERLAY_2D == inheritedDrawMode )
      {
        layer->overlayRenderables.PushBack( Renderable( &node, renderer ) );
      }
      else
      {
        //todor
        std::cout << "todor #### Building renderlist: node: x  rendererNum:" << i << "  curClipId: " << currentClippingId << "  curClipDepth: " << clippingDepth << "  name: " << renderer->GetName() << std::endl;

        //todor
#if 0
        // This must only happen once per node.
        if( ( node.GetClippingMode() != Dali::CLIPPING_DISABLED ) && ( !clippingInfoUpdated ) )
        //if( renderer->GetClippingMode() != Dali::Renderer::CLIPPING_DISABLED && !clippingInfoUpdated ) //todor
        {
          //todor MOVE THIS OUTSIDE LOOP - IF - STAYING AS ONCE PER NODE.
          std::cout << "found clipping mode -------------------------------------------------------------------------------------------------------------" << std::endl;
          ++currentClippingId;
          ++clippingDepth;
          //node.GetRendererAt( i )->SetClippingMode( Dali::Renderer::CLIPPING_DISABLED );
          //todorscnow check
          //node.SetClippingSortModifier( currentClippingId, clippingDepth );

          // todor: mode set is redundant.
          clippingInfoUpdated = true;
        }
#endif
        //todorscnow delete
        //renderer->SetClippingInformation( renderer->GetClippingMode(), currentClippingId, clippingDepth );

        layer->colorRenderables.PushBack( Renderable( &node, renderer ) );
      }
    }
  }

  // Recurse children
  NodeContainer& children = node.GetChildren();
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    bool childResourcesComplete = AddRenderablesForTask( updateBufferIndex, child, *layer, renderTask, inheritedDrawMode, currentClippingId, clippingDepth );
    resourcesFinished &= childResourcesComplete;
  }

  return resourcesFinished;
}

} //Unnamed namespace

void ProcessRenderTasks( BufferIndex updateBufferIndex,
                         RenderTaskList& renderTasks,
                         Layer& rootNode,
                         SortedLayerPointers& sortedLayers,
                         RendererSortingHelper& sortingHelper,
                         RenderInstructionContainer& instructions )
{
  std::cout << "todor: ProcessRenderTasks: START" << std::endl;
  RenderTaskList::RenderTaskContainer& taskContainer = renderTasks.GetTasks();

  if ( taskContainer.IsEmpty() )
  {
    // Early-exit if there are no tasks to process
    return;
  }

  // For each render-task:
  //   1) Prepare the render-task
  //   2) Clear the layer-stored lists of attachments (TODO check if the layer is not changed and don't clear in this case)
  //   3) Traverse the scene-graph, filling the lists for the current render-task
  //   4) Prepare render-instructions

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "ProcessRenderTasks() Offscreens first\n");

  // First process off screen render tasks - we may need the results of these for the on screen renders
  int clippingId = 0; //todor unsigned?
  RenderTaskList::RenderTaskContainer::ConstIterator endIter = taskContainer.End();
  for ( RenderTaskList::RenderTaskContainer::Iterator iter = taskContainer.Begin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = **iter;

    // off screen only
    if(0 == renderTask.GetFrameBufferId())
    {
      // Skip to next task
      continue;
    }

    std::cout << "todor: ProcessRenderTasks: OFFSCREEN:" << std::endl;
    if ( !renderTask.ReadyToRender( updateBufferIndex ) )
    {
      // Skip to next task
      continue;
    }

    Node* sourceNode = renderTask.GetSourceNode();
    DALI_ASSERT_DEBUG( NULL != sourceNode ); // otherwise Prepare() should return false

    // Check that the source node is not exclusive to another task
    if ( ! CheckExclusivity( *sourceNode, renderTask ) )
    {
      continue;
    }

    Layer* layer = FindLayer( *sourceNode );
    if( !layer )
    {
      // Skip to next task as no layer
      continue;
    }

    bool resourcesFinished = false;
    if( renderTask.IsRenderRequired() )
    {
      size_t layerCount( sortedLayers.size() );
      for( size_t i(0); i<layerCount; ++i )
      {
        sortedLayers[i]->ClearRenderables();
      }

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode(),
                                                 clippingId,
                                                 0 );

      renderTask.SetResourcesFinished( resourcesFinished );
      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                renderTask.GetCullMode(),
                                instructions );
    }
    else
    {
      renderTask.SetResourcesFinished( resourcesFinished );
    }
  }

  DALI_LOG_INFO(gRenderTaskLogFilter, Debug::General, "ProcessRenderTasks() Onscreen\n");

  // Now that the off screen renders are done we can process on screen render tasks
  clippingId = 0;
  for ( RenderTaskList::RenderTaskContainer::Iterator iter = taskContainer.Begin(); endIter != iter; ++iter )
  {
    RenderTask& renderTask = **iter;

    // on screen only
    if(0 != renderTask.GetFrameBufferId())
    {
      // Skip to next task
      continue;
    }
    std::cout << "todor: ProcessRenderTasks: ONSCREEN:" << std::endl;
    if ( !renderTask.ReadyToRender( updateBufferIndex ) )
    {
      // Skip to next task
      continue;
    }

    Node* sourceNode = renderTask.GetSourceNode();
    DALI_ASSERT_DEBUG( NULL != sourceNode ); // otherwise Prepare() should return false

    // Check that the source node is not exclusive to another task
    if ( ! CheckExclusivity( *sourceNode, renderTask ) )
    {
      continue;
    }

    Layer* layer = FindLayer( *sourceNode );
    if( !layer )
    {
      // Skip to next task as no layer
      continue;
    }

    bool resourcesFinished = false;
    if( renderTask.IsRenderRequired() )
    {
      size_t layerCount( sortedLayers.size() );
      for( size_t i(0); i<layerCount; ++i )
      {
        sortedLayers[i]->ClearRenderables();
      }

      resourcesFinished = AddRenderablesForTask( updateBufferIndex,
                                                 *sourceNode,
                                                 *layer,
                                                 renderTask,
                                                 sourceNode->GetDrawMode(),
                                                 clippingId,
                                                 0 );

      PrepareRenderInstruction( updateBufferIndex,
                                sortedLayers,
                                renderTask,
                                sortingHelper,
                                renderTask.GetCullMode(),
                                instructions );
    }

    renderTask.SetResourcesFinished( resourcesFinished );
  }
}


} // SceneGraph

} // Internal

} // Dali
