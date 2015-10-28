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
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
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
 * @param renderable attachment
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     RenderableAttachment& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool isLayer3d )
{
  const Node& parentNode = renderable.GetParent();
  const Matrix& worldMatrix = parentNode.GetWorldMatrix( updateBufferIndex );

  // Get the next free RenderItem and initialization
  RenderItem& item = renderList.GetNextFreeItem();
  const Render::Renderer& renderer = renderable.GetRenderer();
  item.SetRenderer( const_cast< Render::Renderer* >( &renderer ) );
  item.SetNode( const_cast< Node* >( &parentNode ) );

  item.SetIsOpaque( renderable.IsFullyOpaque(updateBufferIndex) );
  if( isLayer3d )
  {
    item.SetDepthIndex( renderable.GetDepthIndex() );
  }
  else
  {
    item.SetDepthIndex( renderable.GetDepthIndex() + static_cast<int>( parentNode.GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER );
  }

  // save MV matrix onto the item
  Matrix::Multiply( item.GetModelViewMatrix(), worldMatrix, viewMatrix );
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param NodeRenderer NodeRenderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param cameraAttachment The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     NodeRenderer& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::CameraAttachment& cameraAttachment,
                                     bool isLayer3d )
{
  // Check for cull against view frustum
  const Matrix& worldMatrix = renderable.mNode->GetWorldMatrix( updateBufferIndex );
  bool inside = true;

  if ( renderable.mRenderer->GetMaterial().GetShader()->GeometryHintEnabled( Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY ) )
  {
    const Vector3& position = worldMatrix.GetTranslation3();
    const Vector3& scale = renderable.mNode->GetScale( updateBufferIndex );
    const Vector3& halfSize = renderable.mNode->GetSize( updateBufferIndex ) * scale * 0.5f;

    // Do a fast sphere check
    if ( cameraAttachment.CheckSphereInFrustum( updateBufferIndex, position, halfSize.Length() ) )
    {
      // Check geometry AABB
      //TODO: Take into account orientation
      if ( !cameraAttachment.CheckAABBInFrustum( updateBufferIndex, position, halfSize ) )
      {
        inside = false;
      }
    }
    else
    {
      inside = false;
    }
  }

  if ( inside )
  {
    // Get the next free RenderItem
    RenderItem& item = renderList.GetNextFreeItem();
    item.SetRenderer( &renderable.mRenderer->GetRenderer() );
    item.SetNode( renderable.mNode );
    item.SetIsOpaque( renderable.mRenderer->IsFullyOpaque(updateBufferIndex, *renderable.mNode ) );

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
 */
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableAttachmentContainer& attachments,
                                      NodeRendererContainer& renderers,
                                      const Matrix& viewMatrix,
                                      SceneGraph::CameraAttachment& cameraAttachment,
                                      bool isLayer3d )
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  // Add renderer for each attachment
  unsigned int index(0);
  const RenderableAttachmentIter endIter = attachments.end();
  for ( RenderableAttachmentIter iter = attachments.begin(); iter != endIter; ++iter )
  {
    RenderableAttachment& attachment = **iter;
    AddRendererToRenderList( updateBufferIndex, renderList, attachment, viewMatrix, cameraAttachment, isLayer3d );

    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  List[%d].renderer = %p\n", index, &(attachment.GetRenderer()));
    ++index;
  }

  unsigned int rendererCount( renderers.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    AddRendererToRenderList( updateBufferIndex, renderList, renderers[i], viewMatrix, cameraAttachment, isLayer3d );
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
      const Render::Renderer& renderer = attachment->GetRenderer();
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
 * ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  // @todo MESH_REWORK Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs)
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
    return lhs.shader < rhs.shader;;
  }
  return lhs.renderItem->GetDepthIndex() < rhs.renderItem->GetDepthIndex();
}
/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/geometry/material.
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
      //If both RenderItems are opaque, sort using shader, then material then geometry
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
      //If both RenderItems are transparent, sort using z,then shader, then material, then geometry
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
 * Sort color render items
 * @param colorRenderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
inline void SortColorRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper )
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

  // sort the renderers back to front, Z Axis point from near plane to far plane
  std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems3D );

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, &sortingHelper[ index ].renderItem->GetRenderer() );
  }
}

bool CompareItems2d( const NodeRenderer& lhs, const NodeRenderer& rhs )
{
  if( lhs.mRenderer->GetDepthIndex() == rhs.mRenderer->GetDepthIndex() )
  {
    return lhs.mRenderer->GetMaterial().GetShader() < rhs.mRenderer->GetMaterial().GetShader();
  }

  return lhs.mRenderer->GetDepthIndex() < rhs.mRenderer->GetDepthIndex();
}

void DepthIndexSort( Dali::Vector< NodeRenderer >& container )
{
  std::sort( container.Begin(), container.End(), CompareItems2d );
}

void AddBatchedRenderers( BufferIndex updateBufferIndex,
                          RenderList& renderList,
                          const Matrix& viewMatrix,
                          unsigned int batchBegin,
                          unsigned int batchEnd,
                          Dali::Vector< NodeRenderer >& container,
                          RenderMessageDispatcher& messageDispatcher,
                          unsigned int& currentBatchId )
{
  RenderItem& item = renderList.GetNextFreeItem();

  if( 1u == (batchEnd - batchBegin) )
  {
    NodeRenderer& renderer = container[ batchBegin ];

    // Add lone renderer
    item.SetIsBatch( false );
    item.SetRenderer( &renderer.mRenderer->GetRenderer() );
    item.SetNode( renderer.mNode );
    item.SetIsOpaque( renderer.mRenderer->IsFullyOpaque(updateBufferIndex, *renderer.mNode ) );

    // save MV matrix onto the item
    const Matrix& worldMatrix = renderer.mNode->GetWorldMatrix( updateBufferIndex );
    Matrix::Multiply( item.GetModelViewMatrix(), worldMatrix, viewMatrix );
  }
  else
  {
    Renderer* firstRenderer = container[ batchBegin ].mRenderer;
    Material& material = firstRenderer->GetMaterial();

    // Batch renderers together
    item.SetIsBatch( true );
    item.SetBatchId( currentBatchId );

    // Get the program to use for this batch
    Program* program = material.GetShader()->GetProgram();

    // Get the texture to use for this batch
    ResourceId textureId = material.GetTextureId( 0 );

    // Get the color to use for this batch
    const Vector4& batchColor = container[ batchBegin ].mNode->GetWorldColor( updateBufferIndex );

    // Batch the verties
    Dali::Vector<char>* batchVertices = new Dali::Vector<char>();
    Dali::Vector<char>* batchIndices  = new Dali::Vector<char>();

    // Reserve enough space for all the inserts FIXME
    //batchVertices->Reserve( (batchEnd - batchBegin) * 16 * sizeof(Vertex2D) );
    //batchIndices->Reserve( (batchEnd - batchBegin) * 24 * sizeof(?) );

    for( unsigned int i=batchBegin; i<batchEnd; ++i )
    {
      Geometry& geometry = container[ i ].mRenderer->GetGeometry();

      const Render::PropertyBuffer* vertexBuffer = geometry.GetVertexBuffers()[0];
      const Dali::Vector<char>* vertices = vertexBuffer->GetData();
      if( vertices )
      {
        batchVertices->Insert( batchVertices->End(), vertices->Begin(), vertices->End() );
      }

      const Render::PropertyBuffer* indexBuffer = geometry.GetIndexBuffer();
      if( indexBuffer )
      {
        const Dali::Vector<char>* indices = indexBuffer->GetData();
        if( indices )
        {
          batchIndices->Insert( batchIndices->End(), indices->Begin(), indices->End() );
        }
      }
    }

    // Transfer ownership to RenderManager
    messageDispatcher.SetBatchInfo( currentBatchId, program, textureId, batchColor, batchVertices, batchIndices );

    // Move to next batch ID
    ++currentBatchId;
  }
}

/**
 * @brief Create a render-list for a LAYER_2D
 *
 * @param[in] updateBufferIndex to use
 * @param[in] layer to get the renderers from
 * @param[in] viewmatrix for the camera from rendertask
 * @param[in] cameraAttachment to use the view frustum
 * @param[in] stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param[in,out] instruction The render-list will be added here
 * @param[in] messageDispatcher Used to send messages to render-thread
 * @param[in,out] currentBatchId This is incremented for every group of renderers which are batched together
 */
void PrepareRenderList2d( BufferIndex updateBufferIndex,
                          Layer& layer,
                          const Matrix& viewMatrix,
                          SceneGraph::CameraAttachment& cameraAttachment,
                          bool stencilRenderablesExist,
                          RenderInstruction& instruction,
                          RenderMessageDispatcher& messageDispatcher,
                          unsigned int& currentBatchId )
{
  Dali::Vector< NodeRenderer >& container = layer.colorRenderers;
  const unsigned int count = container.Count();

  if( 0 != count )
  {
    RenderList& renderList = instruction.GetNextFreeRenderList( count );

    renderList.SetHasColorRenderItems( true );

    // Enable scisscor test if necessary
    renderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );

    // Sort into 2D depth-index order
    DepthIndexSort( container );

    // Find renderables using the same material & color
    const Material* batchMaterial = &(container[0].mRenderer->GetMaterial());
    const Vector4* batchColor = &(container[0].mNode->GetWorldColor( updateBufferIndex ));

    unsigned int batchBegin = 0;
    unsigned int batchEnd = 1;
    for ( ; batchEnd < count; ++batchEnd )
    {
      Renderer* renderer = container[ batchEnd ].mRenderer;
      const Material* material = &renderer->GetMaterial();
      const Vector4*  color    = &(container[ batchEnd ].mNode->GetWorldColor( updateBufferIndex ));

      // Continue batching if material and color are the same
      if( batchMaterial != material ||
          1 != material->GetTextureCount() ||
          *batchColor != *color )
      {
        AddBatchedRenderers( updateBufferIndex,
                             renderList,
                             viewMatrix,
                             batchBegin,
                             batchEnd,
                             container,
                             messageDispatcher,
                             currentBatchId );

        batchBegin    = batchEnd;
        batchMaterial = material;
        batchColor    = color;
      }
    }

    AddBatchedRenderers( updateBufferIndex,
                         renderList,
                         viewMatrix,
                         batchBegin,
                         batchEnd,
                         container,
                         messageDispatcher,
                         currentBatchId );
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
 */
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::CameraAttachment& cameraAttachment,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               bool tryReuseRenderList )
{
  RenderList& renderList = instruction.GetNextFreeRenderList( layer.colorRenderables.size() );
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

  AddRenderersToRenderList( updateBufferIndex, renderList, layer.colorRenderables, layer.colorRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
  SortColorRenderItems( updateBufferIndex, renderList, layer, sortingHelper );

  //Set render flags
  unsigned int flags = 0u;
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  // Special optimization if depth test is disabled or if only one opaque rendered in the layer (for example background image)
  // and this renderer does not need depth test against itself (e.g. mesh)
  // and if this layer has got exactly one opaque renderer
  // and this renderer is not interested in depth testing
  // (i.e. is an image and not a mesh)

  if ( ( renderList.Count() == 1 ) &&
       ( !renderList.GetRenderer( 0 ).RequiresDepthTest() ) &&
       ( !renderList.GetItem(0).IsOpaque() ) )
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
  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, layer.overlayRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
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
  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, layer.stencilRenderers, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D );
}

/**
 * Prepare a single render instruction
 * @param updateBufferIndex to use
 * @param sortedLayers to prepare the instruction from
 * @param renderTask to get the view matrix
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param renderTracker An optional render tracker object
 * @param instructions container
 * @param[in] messageDispatcher Used to send messages to render-thread
 * @param[in,out] currentBatchId This is incremented for every group of renderers which are batched together
 */
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               RenderTracker* renderTracker,
                               RenderInstructionContainer& instructions,
                               RenderMessageDispatcher& messageDispatcher,
                               unsigned int& currentBatchId )
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

    const bool stencilRenderablesExist( !layer.stencilRenderables.empty() || !layer.stencilRenderers.Empty() );
    const bool colorRenderablesExist( !layer.colorRenderables.empty() || !layer.colorRenderers.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.empty() || !layer.overlayRenderers.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers(renderTask.GetCamera()) );

    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( colorRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, instruction, tryReuseRenderList );
    }

    if ( colorRenderablesExist )
    {
      if( layer.GetBehavior() ==  Dali::Layer::LAYER_2D )
      {
        PrepareRenderList2d( updateBufferIndex,
                             layer,
                             viewMatrix,
                             cameraAttachment,
                             stencilRenderablesExist,
                             instruction,
                             messageDispatcher,
                             currentBatchId );
      }
      else
      {
        AddColorRenderers( updateBufferIndex,
                           layer,
                           viewMatrix,
                           cameraAttachment,
                           stencilRenderablesExist,
                           instruction,
                           sortingHelper,
                           tryReuseRenderList );
      }
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
