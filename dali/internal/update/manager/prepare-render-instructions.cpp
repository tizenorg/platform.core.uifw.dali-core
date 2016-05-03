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

// CLASS HEADER
#include <dali/internal/update/manager/prepare-render-instructions.h>

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/layer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderListLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_LISTS");
#endif
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     Renderable& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool isLayer3d,
                                     bool cull )
{
  bool inside( true );

  const Matrix& worldMatrix = renderable.mNode->GetWorldMatrix( updateBufferIndex );
  const Vector3& size = renderable.mNode->GetSize( updateBufferIndex );
  if ( cull && !renderable.mRenderer->GetShader().HintEnabled( Dali::Shader::HINT_MODIFIES_GEOMETRY ) )
  {
    const Vector3& position = worldMatrix.GetTranslation3();
    float radius( size.Length() * 0.5f );

    inside = (radius > Math::MACHINE_EPSILON_1000) &&
        (cameraAttachment.CheckSphereInFrustum( updateBufferIndex, position, radius ) );
  }

  if ( inside )
  {
    Renderer::Opacity opacity = renderable.mRenderer->GetOpacity( updateBufferIndex, *renderable.mNode );
    if( opacity != Renderer::TRANSPARENT )
    {
      // Get the next free RenderItem
      RenderItem& item = renderList.GetNextFreeItem();
      item.SetRenderer( &renderable.mRenderer->GetRenderer() );
      item.SetNode( renderable.mNode );
      item.SetIsOpaque( opacity == Renderer::OPAQUE );

      if( isLayer3d )
      {
        item.SetDepthIndex( renderable.mRenderer->GetDepthIndex() );
      }
      else
      {
        item.SetDepthIndex( renderable.mRenderer->GetDepthIndex() + static_cast<int>( renderable.mNode->GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER );
      }
      // save MV matrix onto the item
      Matrix::Multiply( item.GetModelViewMatrix(), worldMatrix, viewMatrix );
      item.SetSize( size );
    }
  }
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the items to
 * @param renderable attachments
 * NodeRendererContainer Node-Renderer pairs
 * @param viewMatrix used to calculate modelview matrix for the items
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableContainer& renderables,
                                      const Matrix& viewMatrix,
                                      SceneGraph::CameraAttachment& cameraAttachment,
                                      bool isLayer3d,
                                      bool cull)
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  unsigned int rendererCount( renderables.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    AddRendererToRenderList( updateBufferIndex, renderList, renderables[i], viewMatrix, cameraAttachment, isLayer3d, cull );
  }
}

/**
 * Try to reuse cached renderitems from the renderlist
 * This avoids recalculating the model view matrices in case this part of the scene was static
 * An example case is a toolbar layer that rarely changes or a popup on top of the rest of the stage
 * @param layer that is being processed
 * @param renderList that is cached from frame N-1
 * @param renderables list of renderables
 */
inline bool TryReuseCachedRenderers( Layer& layer,
                                     RenderList& renderList,
                                     RenderableContainer& renderables )
{
  bool retValue = false;
  size_t renderableCount = renderables.Size();
  // check that the cached list originates from this layer and that the counts match
  if( ( renderList.GetSourceLayer() == &layer )&&
      ( renderList.GetCachedItemCount() == renderableCount ) )
  {
    // check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong attachments
    // Attachments are not sorted, but render list is so at this stage renderers may be in different order
    // therefore we check a combined sum of all renderer addresses
    size_t checkSumNew = 0;
    size_t checkSumOld = 0;
    for( size_t index = 0; index < renderableCount; ++index )
    {
      const Render::Renderer& renderer = renderables[index].mRenderer->GetRenderer();
      checkSumNew += size_t( &renderer );
      checkSumOld += size_t( &renderList.GetRenderer( index ) );
    }
    if( checkSumNew == checkSumOld )
    {
      // tell list to reuse its existing items
      renderList.ReuseCachedItems();
      retValue = true;
    }
  }
  return retValue;
}

/**
 * Function which sorts render items by depth index then by instance
 * ptrs of shader/texture/geometry.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  if( lhs.renderItem->GetDepthIndex() == rhs.renderItem->GetDepthIndex() )
  {
    if( lhs.shader == rhs.shader )
    {
      if( lhs.textureResourceId == rhs.textureResourceId )
      {
        return lhs.geometry < rhs.geometry;
      }
      return lhs.textureResourceId < rhs.textureResourceId;
    }
    return lhs.shader < rhs.shader;
  }
  return lhs.renderItem->GetDepthIndex() < rhs.renderItem->GetDepthIndex();
}
/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/texture/geometry.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems3D( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  bool lhsIsOpaque = lhs.renderItem->IsOpaque();
  if( lhsIsOpaque ==  rhs.renderItem->IsOpaque())
  {
    if( lhsIsOpaque )
    {
      //If both RenderItems are opaque, sort using shader, then texture then geometry
      if( lhs.shader == rhs.shader )
      {
        if( lhs.textureResourceId == rhs.textureResourceId )
        {
          return lhs.geometry < rhs.geometry;
        }
        return lhs.textureResourceId < rhs.textureResourceId;
      }
      return lhs.shader < rhs.shader;
    }
    else
    {
      //If both RenderItems are transparent, sort using z,then shader, then texture, then geometry
      if( Equals(lhs.zValue, rhs.zValue) )
      {
        if( lhs.shader == rhs.shader )
        {
          if( lhs.textureResourceId == rhs.textureResourceId )
          {
            return lhs.geometry < rhs.geometry;
          }
          return lhs.textureResourceId < rhs.textureResourceId;
        }
        return lhs.shader < rhs.shader;
      }
      return lhs.zValue > rhs.zValue;
    }
  }
  else
  {
    return lhsIsOpaque;
  }
}

/**
 * Sort render items
 * @param bufferIndex The buffer to read from
 * @param renderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
inline void SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper )
{
  const size_t renderableCount = renderList.Count();
  // reserve space if needed
  const unsigned int oldcapacity = sortingHelper.size();
  if( oldcapacity < renderableCount )
  {
    sortingHelper.reserve( renderableCount );
    // add real objects (reserve does not construct objects)
    sortingHelper.insert( sortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RendererWithSortAttributes() );
  }
  else
  {
    // clear extra elements from helper, does not decrease capability
    sortingHelper.resize( renderableCount );
  }

  // calculate the sorting value, once per item by calling the layers sort function
  // Using an if and two for-loops rather than if inside for as its better for branch prediction
  if( layer.UsesDefaultSortFunction() )
  {
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.GetRenderer().SetSortAttributes( bufferIndex, sortingHelper[ index ] );

      // the default sorting function should get inlined here
      sortingHelper[ index ].zValue = Internal::Layer::ZValue( item.GetModelViewMatrix().GetTranslation3() ) - item.GetDepthIndex();

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }
  else
  {
    const Dali::Layer::SortFunctionType sortFunction = layer.GetSortFunction();
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.GetRenderer().SetSortAttributes( bufferIndex, sortingHelper[ index ] );
      sortingHelper[ index ].zValue = (*sortFunction)( item.GetModelViewMatrix().GetTranslation3() ) - item.GetDepthIndex();

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }

  if( layer.GetBehavior() ==  Dali::Layer::LAYER_3D)
  {
    // sort the renderers back to front, Z Axis point from near plane to far plane
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems3D );
  }
  else
  {
    // sort the renderers based on DepthIndex
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems );
  }

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, &sortingHelper[ index ].renderItem->GetRenderer() );
  }
}

/**
 * Add color renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param cameraAttachment to use the view frustum
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::CameraAttachment& cameraAttachment,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               bool tryReuseRenderList,
                               bool cull)
{
  RenderList& renderList = instruction.GetNextFreeRenderList( layer.colorRenderables.Size() );
  renderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  renderList.SetHasColorRenderItems( true );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, renderList, layer.colorRenderables ) )
    {
      return;
    }
  }

  AddRenderersToRenderList( updateBufferIndex, renderList, layer.colorRenderables, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
  SortRenderItems( updateBufferIndex, renderList, layer, sortingHelper );

  //Set render flags
  unsigned int flags = 0u;
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  // Special optimization. If this layer has got exactly one renderer
  // and this renderer is not writing to the depth buffer there is no point on enabling
  // depth buffering
  if ( ( renderList.Count() == 1 ) &&
       (( renderList.GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::OFF ) ||
        ( renderList.GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::AUTO && !renderList.GetItem(0).IsOpaque() )))
  {
    //Nothing to do here
  }
  else if( !layer.IsDepthTestDisabled())
  {
    flags |= RenderList::DEPTH_BUFFER_ENABLED;
    flags |= RenderList::DEPTH_CLEAR;
  }

  renderList.ClearFlags();
  renderList.SetFlags( flags );
}

/**
 * Add overlay renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddOverlayRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 bool stencilRenderablesExist,
                                 RenderInstruction& instruction,
                                 RendererSortingHelper& sortingHelper,
                                 bool tryReuseRenderList,
                                 bool cull )
{
  RenderList& overlayRenderList = instruction.GetNextFreeRenderList( layer.overlayRenderables.Size() );
  overlayRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  overlayRenderList.SetHasColorRenderItems( false );

  //Set render flags
  overlayRenderList.ClearFlags();
  if(stencilRenderablesExist)
  {
    overlayRenderList.SetFlags(RenderList::STENCIL_BUFFER_ENABLED);
  }

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, overlayRenderList, layer.overlayRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
  SortRenderItems( updateBufferIndex, overlayRenderList, layer, sortingHelper );
}

/**
 * Add stencil renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddStencilRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList,
                                 bool cull )
{
  RenderList& stencilRenderList = instruction.GetNextFreeRenderList( layer.stencilRenderables.Size() );
  stencilRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  stencilRenderList.SetHasColorRenderItems( false );

  //Set render flags
  stencilRenderList.ClearFlags();
  stencilRenderList.SetFlags(RenderList::STENCIL_CLEAR | RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, stencilRenderList, layer.stencilRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
}

void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               RenderInstructionContainer& instructions )
{
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::CameraAttachment& cameraAttachment = renderTask.GetCameraAttachment();

  const SortedLayersIter endIter = sortedLayers.end();
  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    const bool stencilRenderablesExist( !layer.stencilRenderables.Empty() );
    const bool colorRenderablesExist( !layer.colorRenderables.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers(renderTask.GetCamera()) );

    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( colorRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, instruction, tryReuseRenderList, cull );
    }

    if ( colorRenderablesExist )
    {
      AddColorRenderers( updateBufferIndex,
                         layer,
                         viewMatrix,
                         cameraAttachment,
                         stencilRenderablesExist,
                         instruction,
                         sortingHelper,
                         tryReuseRenderList,
                         cull );
    }

    if ( overlayRenderablesExist )
    {
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, stencilRenderablesExist,
                           instruction, sortingHelper, tryReuseRenderList, cull );
    }
  }

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
}

} // SceneGraph

} // Internal

} // Dali
