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

// CLASS HEADER
#include <dali/internal/update/manager/prepare-render-instructions.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/node-attachments/scene-graph-renderer-attachment.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Set flags for opaque renderlist
 * @param renderList to set the flags to
 * @param transparentRenderersExist is true if there is transparent renderers in this layer
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param depthTestDisabled whether depth test is disabled.
 */
inline void SetOpaqueRenderFlags( RenderList& renderList, bool transparentRenderersExist, bool stencilRenderablesExist, bool depthTestDisabled )
{
  //@todo MESH_REWORK Move RequiresDepthTest from render thread object to update thread object

  // Special optimization if depth test is disabled or if only one opaque rendered in the layer (for example background image)
  // and this renderer does not need depth test against itself (e.g. mesh)
  // and if this layer has got exactly one opaque renderer
  // and this renderer is not interested in depth testing
  // (i.e. is an image and not a mesh)
  if ( ( (  renderList.Count() == 1 ) &&
         (  !transparentRenderersExist ) &&
         (  !renderList.GetRenderer( 0 )->RequiresDepthTest() ) ) ||
       depthTestDisabled )
  {
    // no need to enable depth test or clear depth buffer
    // if there's something transparent already rendered by previous layers,
    // this opaque renderer will correctly draw on top of them since no depth test
    renderList.ClearFlags();
  }
  else
  {
    // Prepare for rendering multiple opaque objects
    unsigned int flags = RenderList::DEPTH_BUFFER_ENABLED | RenderList::DEPTH_WRITE | RenderList::DEPTH_CLEAR; // clear depth buffer, draw over the previously rendered layers;

    renderList.ClearFlags();
    renderList.SetFlags(flags);
  }

  if( stencilRenderablesExist )
  {
    renderList.SetFlags( RenderList::STENCIL_BUFFER_ENABLED );
  }
}

/**
 * Set the transparent flags on the renderlist
 * @param renderList to set the flags on
 * @param opaqueRenderersExist is true if there are opaque renderers on this layer
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param depthTestDisabled whether depth test is disabled.
 */
inline void SetTransparentRenderFlags( RenderList& renderList, bool opaqueRenderersExist, bool stencilRenderablesExist, bool depthTestDisabled )
{
  renderList.ClearFlags();
  // We don't need to write to the depth buffer, as transparent objects
  // don't obscure each other.

  if ( opaqueRenderersExist && !depthTestDisabled )
  {
    // If there are a mix of opaque and transparent objects, the transparent
    // objects should be rendered with depth test on to avoid background objects
    // appearing in front of opaque foreground objects.

    renderList.SetFlags( RenderList::DEPTH_BUFFER_ENABLED );
  }

  if( stencilRenderablesExist )
  {
    renderList.SetFlags( RenderList::STENCIL_BUFFER_ENABLED );
  }
}


/**
 * Set flags for overlay renderlist
 * @param renderList to set the flags for
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 */
inline void SetOverlayRenderFlags( RenderList& renderList, bool stencilRenderablesExist )
{
  if(stencilRenderablesExist)
  {
    renderList.SetFlags(RenderList::STENCIL_BUFFER_ENABLED);
  }
}

/**
 * Set flags for stencil renderlist
 * @param renderList to set the flags for
 */
inline void SetStencilRenderFlags( RenderList& renderList )
{
  renderList.ClearFlags();
  renderList.SetFlags(RenderList::STENCIL_CLEAR | RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED );
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable attachment
 * @param viewMatrix used to calculate modelview matrix for the item
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     RenderableAttachment& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment )
{
  // Check for clip against view frustum
  Matrix mvm;
  bool inside = true;

  const Node& parentNode = renderable.GetParent();
  const Matrix& worldMatrix = parentNode.GetWorldMatrix( updateBufferIndex );
  Matrix::Multiply( mvm, worldMatrix, viewMatrix );

  if ( RendererAttachment* rendererAttachment = dynamic_cast< RendererAttachment* >( &renderable ) )
  {
    const Geometry& geometry = rendererAttachment->GetGeometry();
    if ( geometry.IsFrustumCulled( updateBufferIndex) )
    {

      // Get the geometry extents for frustum checking
      const Vector3& position = worldMatrix.GetTranslation3();
      const Vector3& localCenter = geometry.mCenter[ updateBufferIndex ];
      const Vector3& size = parentNode.GetSize( updateBufferIndex );
      Vector3 center = position + ( localCenter * size );

      // If the size components are all the same, then we can do a quick bounding sphere check
      if ( fabsf( size.x - size.y ) <= Math::MACHINE_EPSILON_1 * size.x )
      {
        inside = cameraAttachment.CheckSphereInFrustum( updateBufferIndex, center, geometry.mRadius[ updateBufferIndex ] * size.x );
      }

      if ( inside )
      {
        // Check against AABB of model
        inside = cameraAttachment.CheckAABBInFrustum( updateBufferIndex, center, geometry.mHalfExtents[ updateBufferIndex ] * size );
      }
    }
  }

  if ( inside )
  {
    // Get the next free RenderItem
    RenderItem& item = renderList.GetNextFreeItem();
    const Renderer& renderer = renderable.GetRenderer();
    item.SetRenderer( const_cast< Renderer* >( &renderer ) );

    // save MV matrix onto the item
    Matrix& modelViewMatrix = item.GetModelViewMatrix();
    modelViewMatrix = mvm;
  }
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the items to
 * @param renderable attachments
 * @param viewMatrix used to calculate modelview matrix for the items
 */
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableAttachmentContainer& attachments,
                                      const Matrix& viewMatrix,
                                      SceneGraph::CameraAttachment& cameraAttachment )
{
  // Add renderer for each attachment
  const RenderableAttachmentIter endIter = attachments.end();
  for ( RenderableAttachmentIter iter = attachments.begin(); iter != endIter; ++iter )
  {
    RenderableAttachment& attachment = **iter;
    AddRendererToRenderList( updateBufferIndex, renderList, attachment, viewMatrix, cameraAttachment );
  }
}

/**
 * Try to reuse cached renderitems from the renderlist
 * This avoids recalculating the model view matrices in case this part of the scene was static
 * An example case is a toolbar layer that rarely changes or a popup on top of the rest of the stage
 * @param layer that is being processed
 * @param renderList that is cached from frame N-1
 * @param attachmentList that is being used
 */
inline bool TryReuseCachedRenderers( Layer& layer,
                                     RenderList& renderList,
                                     RenderableAttachmentContainer& attachmentList )
{
  bool retValue = false;
  size_t renderableCount = attachmentList.size();
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
      RenderableAttachment* attachment = attachmentList[ index ];
      const Renderer& renderer = attachment->GetRenderer();
      checkSumNew += size_t( &renderer );
      checkSumOld += size_t( renderList.GetRenderer( index ) );
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
 * Add opaque renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param cameraAttachment to use the view frustum
 * @param transparentRenderersExist is true if there is transparent renderers in this layer
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddOpaqueRenderers( BufferIndex updateBufferIndex,
                                Layer& layer,
                                const Matrix& viewMatrix,
                                SceneGraph::CameraAttachment& cameraAttachment,
                                bool transparentRenderablesExist,
                                bool stencilRenderablesExist,
                                RenderInstruction& instruction,
                                bool tryReuseRenderList )
{
  RenderList& opaqueRenderList = instruction.GetNextFreeRenderList( layer.opaqueRenderables.size() );
  opaqueRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, opaqueRenderList, layer.opaqueRenderables ) )
    {
      // reset the flags as other layers might have changed
      // opaque flags can only be set after renderers are added
      SetOpaqueRenderFlags(opaqueRenderList, transparentRenderablesExist, stencilRenderablesExist, layer.IsDepthTestDisabled() );
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, opaqueRenderList, layer.opaqueRenderables, viewMatrix, cameraAttachment );

  // opaque flags can only be set after renderers are added
  SetOpaqueRenderFlags(opaqueRenderList, transparentRenderablesExist, stencilRenderablesExist, layer.IsDepthTestDisabled() );
}

/**
 * Function which sorts based on the calculated depth values ordering them back to front
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool SortByDepthSortValue( const RendererWithSortValue& lhs, const RendererWithSortValue& rhs )
{
  return lhs.first > rhs.first;
}

/**
 * Sort transparent render items
 * @param transparentRenderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
inline void SortTransparentRenderItems( RenderList& transparentRenderList, Layer& layer, RendererSortingHelper& sortingHelper )
{
  const size_t renderableCount = transparentRenderList.Count();
  // reserve space if needed
  const unsigned int oldcapacity = sortingHelper.size();
  if( oldcapacity < renderableCount )
  {
    sortingHelper.reserve( renderableCount );
    // add real objects (reserve does not construct objects)
    sortingHelper.insert( sortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RendererWithSortValue( 0.0f, NULL ) );
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
      RenderItem& item = transparentRenderList.GetItem( index );
      // the default sorting function should get inlined here
      sortingHelper[ index ].first = Internal::Layer::ZValue(
          item.GetModelViewMatrix().GetTranslation3(),
          layer.transparentRenderables[ index ]->GetSortModifier() );
      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].second = &item;
    }
  }
  else
  {
    const Dali::Layer::SortFunctionType sortFunction = layer.GetSortFunction();
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = transparentRenderList.GetItem( index );
      sortingHelper[ index ].first = (*sortFunction)(
          item.GetModelViewMatrix().GetTranslation3(),
          layer.transparentRenderables[ index ]->GetSortModifier() );
      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].second = &item;
    }
  }

  // sort the renderers back to front, Z Axis point from near plane to far plane
  std::sort( sortingHelper.begin(), sortingHelper.end(), SortByDepthSortValue );

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  RenderItemContainer::Iterator renderListIter = transparentRenderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].second;
  }
}

/**
 * Add transparent renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param opaqueRenderablesExist is true if there are opaque renderers on this layer
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddTransparentRenderers( BufferIndex updateBufferIndex,
                                     Layer& layer,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool opaqueRenderablesExist,
                                     bool stencilRenderablesExist,
                                     RenderInstruction& instruction,
                                     RendererSortingHelper& sortingHelper,
                                     bool tryReuseRenderList )
{
  const size_t renderableCount = layer.transparentRenderables.size();
  RenderList& transparentRenderList = instruction.GetNextFreeRenderList( renderableCount );
  transparentRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  // transparent flags are independent of the amount of transparent renderers
  SetTransparentRenderFlags( transparentRenderList, opaqueRenderablesExist, stencilRenderablesExist, layer.IsDepthTestDisabled() );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, transparentRenderList, layer.transparentRenderables ) )
    {
      return;
    }
  }
  transparentRenderList.SetSourceLayer( &layer );

  AddRenderersToRenderList( updateBufferIndex, transparentRenderList, layer.transparentRenderables, viewMatrix, cameraAttachment );

  // sorting is only needed if more than 1 item
  if( renderableCount > 1 )
  {
    SortTransparentRenderItems( transparentRenderList, layer, sortingHelper );
  }
}

/**
 * Add overlay renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddOverlayRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 bool stencilRenderablesExist,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList )
{
  RenderList& overlayRenderList = instruction.GetNextFreeRenderList( layer.overlayRenderables.size() );
  overlayRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  SetOverlayRenderFlags( overlayRenderList, stencilRenderablesExist );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, overlayRenderList, layer.overlayRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, viewMatrix, cameraAttachment );
}

/**
 * Add stencil renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 */
inline void AddStencilRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::CameraAttachment& cameraAttachment,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList )
{
  RenderList& stencilRenderList = instruction.GetNextFreeRenderList( layer.stencilRenderables.size() );
  stencilRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  SetStencilRenderFlags( stencilRenderList );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, stencilRenderList, layer.stencilRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, viewMatrix, cameraAttachment );
}

/**
 * Prepare a single render instruction
 * @param updateBufferIndex to use
 * @param sortedLayers to prepare the instruction from
 * @param renderTask to get the view matrix
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param renderTracker An optional render tracker object
 * @param instructions container
 */
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               RenderTracker* renderTracker,
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

    const bool stencilRenderablesExist( !layer.stencilRenderables.empty() );
    const bool opaqueRenderablesExist( !layer.opaqueRenderables.empty() );
    const bool transparentRenderablesExist( !layer.transparentRenderables.empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers(renderTask.GetCamera()) );

    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( opaqueRenderablesExist || transparentRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, instruction, tryReuseRenderList );
    }

    if ( opaqueRenderablesExist )
    {
      AddOpaqueRenderers( updateBufferIndex,
                          layer,
                          viewMatrix,
                          cameraAttachment,
                          transparentRenderablesExist,
                          stencilRenderablesExist,
                          instruction,
                          tryReuseRenderList );
    }

    if ( transparentRenderablesExist )
    {
      AddTransparentRenderers( updateBufferIndex,
                               layer,
                               viewMatrix,
                               cameraAttachment,
                               opaqueRenderablesExist,
                               stencilRenderablesExist,
                               instruction,
                               sortingHelper,
                               tryReuseRenderList );
    }

    if ( overlayRenderablesExist )
    {
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, stencilRenderablesExist,
                           instruction, tryReuseRenderList );
    }
  }

  instruction.mRenderTracker = renderTracker;
  instruction.mCullMode = renderTask.GetCullMode();

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
}

} // SceneGraph

} // Internal

} // Dali
