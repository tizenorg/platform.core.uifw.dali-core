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
#include <dali/internal/update/manager/prepare-render-instructions.h>

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/layer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
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
//todor
#include <iostream>
#include <string>

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
static int gVisitedId = 1;//todortestonly

// Recursively dumps node tree.
void DumpNode( const Node& node, int indent = 0 )
{
  // Indent based on depth.
  for( int t = 0; t < indent; ++t )
  {
    std::cout << "  ";
  }
  //std::cout << "Parent:" << (int)node.GetParent() << std::endl;
  const NodeContainer& children = node.GetChildren();
  std::cout << "Node: childrenCount:" << children.Count() << std::endl;
  int i = 0;
  // Iterate through child nodes.
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    DumpNode( child, indent + 1 );
    i++;
  }
}

// Recursively dumps node tree used by RenderList.
// TODO: Traversing the whole list of RenderItems means the same node will be encountered more than once if it is parented / has children.
// We need a "visited" list to resolve this.
void DumpRenderList( RenderList& renderList, std::string name )
{
  int renderItemCount = renderList.Count();
  for( int r = 0; r < renderItemCount; ++r )
  {
    RenderItem& item = renderList.GetItem( r );
    const Node& node = item.GetNode();
    std::cout << "RenderItem: " << r << std::endl;
    DumpNode( node, 1 );
  }
}

// Recursively dumps node tree.
void PrepareClippingNodeInformation( const Node& node, int indent = 0 )
{
  // Indent based on depth.
  for( int t = 0; t < indent; ++t )
  {
    std::cout << "  ";
  }

  //std::cout << "Parent:" << (int)node.GetParent() << std::endl;
  const NodeContainer& children = node.GetChildren();
  std::cout << "Node: childrenCount:" << children.Count() << std::endl;
  unsigned int i = 0;
  // Iterate through child nodes.
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;

    //Dali::Internal::SceneGraph::Renderer r = node->GetRendererAt( 0 );
    //std::cout << "VIS: node name" << 1 << std::endl;

    PrepareClippingNodeInformation( child, indent + 1 );
    i++;
  }
}

// Recursively dumps node tree used by RenderList.
// TODO: Traversing the whole list of RenderItems means the same node will be encountered more than once if it is parented / has children.
// We need a "visited" list to resolve this.
void PrepareClippingInformation( RenderList& renderList, std::string name )
{
  int renderItemCount = renderList.Count();
  for( int r = 0; r < renderItemCount; ++r )
  {
    RenderItem& item = renderList.GetItem( r );
    const Node& node = item.GetNode();
    std::cout << "RenderItem: " << r << std::endl;
    PrepareClippingNodeInformation( node, 1 );
  }
}

std::string PrintBin( uint16_t value )
{
  std::string outString( "0000000000000000" );
  int pos = 16 - 1;

  //char *int2bin(int a, char *buffer, int buf_size) {
  //    buffer += (buf_size - 1);

  for( int i = 15; i >= 0; i-- )
  {
    outString[ pos-- ] = ( value & 1 ) + '0';
    value >>= 1;
  }

    //  return buffer;
  //}
  return outString;
}


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
#if 1
  std::cout << "todor: ++++++++++++++++++++ AddRendererToRenderList: RendererName:" << renderable.mRenderer->GetName() <<
      "  ClippingMode:" << (int)renderable.mRenderer->GetClippingMode() << " ++++START++++" << std::endl;
#endif
  bool inside( true );

  //todor

  const Matrix& worldMatrix = renderable.mNode->GetWorldMatrix( updateBufferIndex );
  if ( cull && renderable.mRenderer->GetMaterial().GetShader()->GeometryHintEnabled( Dali::ShaderEffect::HINT_DOESNT_MODIFY_GEOMETRY ) )
  {
    const Vector3& scale = renderable.mNode->GetWorldScale( updateBufferIndex );
    const Vector3& halfSize = renderable.mNode->GetSize( updateBufferIndex ) * scale * 0.5f;
    float radius( halfSize.Length() );

    if( radius > Math::MACHINE_EPSILON_1000 )
    {
      const Vector3& position = worldMatrix.GetTranslation3();
      const Quaternion& rotation = renderable.mNode->GetWorldOrientation( updateBufferIndex );
      bool axisAligned = rotation.IsIdentity();

      if( axisAligned )
      {
        inside = cameraAttachment.CheckAABBInFrustum( updateBufferIndex, position, halfSize );
      }
      else
      {
        inside = cameraAttachment.CheckSphereInFrustum( updateBufferIndex, position, halfSize.Length() );
      }
    }
    else
    {
      inside = false;
    }
  }

  if ( inside )
  {
    Renderer::Opacity opacity = renderable.mRenderer->GetOpacity( updateBufferIndex, *renderable.mNode );
    if( opacity != Renderer::TRANSPARENT )
    {
      // Get the next free RenderItem
      std::cout << "todor: RL: #1# co: " << renderList.Count() << std::endl;
      RenderItem& item = renderList.GetNextFreeItem();
      item.SetRenderer( &renderable.mRenderer->GetRenderer() );
      item.SetNode( renderable.mNode );
      item.SetIsOpaque( opacity == Renderer::OPAQUE );

      //todorscnow putback unsafe
      item.SetName( renderable.mRenderer->GetName() );//todor
      std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#if 0
      item.SetClippingMode( renderable.mRenderer->GetClippingMode() );//todor
      item.SetName( renderable.mRenderer->GetName() );//todor
      std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#endif
#if 1
      std::cout << "todor: RL: #2# co: " << renderList.Count() << std::endl;
      //item.SetClippingMode( Dali::Renderer::CLIPPING_DISABLED );//todor
      //item.SetName( "aaa" );//todor
      item.SetName2( "aaa" );//todor
      //std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#endif

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
      std::cout << "todor: RL: #3# co: " << renderList.Count() << std::endl;
    }
  }
  std::cout << "todor: AddRendererToRenderList:                                                   ++++DONE++++" << std::endl;
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
//todor
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
 * ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
//todor mod
bool CompareItems( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  // @todo MESH_REWORK Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs)

  //todor get sort modifier:
#if 0
  std::cout << "todor: ### Doing compare: depth:" << lhsNode.GetDepth() <<
      "  UCID:" << lhsNode.GetClippingId() <<
      "  CD:" << lhsNode.GetClippingDepth() <<
      "  CM:" << (int)lhsNode.GetClippingMode() << std::endl;
#endif

  //todor optimise:
  const Node& lhsNode = lhs.renderItem->GetNode();
  const Node& rhsNode = rhs.renderItem->GetNode();

#if 1
  //todor
  if( lhsNode.GetClippingId() == rhsNode.GetClippingId() )
  {
    bool lhsClip = lhsNode.GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    bool rhsClip = rhsNode.GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    if( lhsClip == rhsClip )
    {
#endif
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
#if 1
    }
    return lhsClip && !rhsClip;
  }
  return lhsNode.GetClippingId() < rhsNode.GetClippingId();
#endif
}

/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
//todor mod
bool CompareItems3D( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  bool lhsIsOpaque = lhs.renderItem->IsOpaque();
  if( lhsIsOpaque == rhs.renderItem->IsOpaque())
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
 * Sort render items
 * @param bufferIndex The buffer to read from
 * @param renderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
//todor
inline void SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper )
{
  std::cout << "todor: SortRenderItems" << std::endl;
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

  if( layer.GetBehavior() == Dali::Layer::LAYER_3D)
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
  std::cout << "todor: SortRenderItems: DONE" << std::endl;
}

#if 0
// Recursively dumps node tree.
void ProcessRenderItem( RenderItem& item, int indent = 0 )
{
  //std::cout << "Parent:" << (int)node.GetParent() << std::endl;

  item.Get
  const NodeContainer& children = node.GetChildren();
  std::cout << "Node: childrenCount:" << children.Count() << std::endl;
  int i = 0;
  // Iterate through child nodes.
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node& child = **iter;
    DumpNode( child, indent + 1 );
    i++;
  }

  if( item.GetClippingMode() != Dali::Renderer::CLIPPING_DISABLED )
  {
    ++clippingId;
  }

  item.SetClippingId( clippingId );
}
#endif

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
  std::cout << "todor: AddColorRenderers" << std::endl;
  RenderList& renderList = instruction.GetNextFreeRenderList( layer.colorRenderables.Size() );
  renderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );//todordel
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

  //PrepareClippingInformation( renderList, "Color" );
  //DumpRenderList( renderList, "Color" );

  SortRenderItems( updateBufferIndex, renderList, layer, sortingHelper );

  //todordel
  //Set render flags
  unsigned int flags = 0u;
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  //todor
  //#####################################################################################
  //#####################################################################################
  //#####################################################################################
  //#####################################################################################

  if( gVisitedId == 0 )
  {
  }
#if 0
  // If any stencil renderables exist
  const size_t renderableCount = renderList.Count();

  int clippingId = 0;

  // Temp way to generate clipping IDs.
  // TODO: This should be done BEFORE sorting by recursion.
  for( size_t index = 0; index < renderableCount; ++index )
  {
    RenderItem& item = renderList.GetItem( index );

    //ProcessRenderItem( RenderItem& item );
    //std::cout << "todor: testb: " << item.IsClipping() << std::endl;

    std::cout << "todor: ITER RENDERLIST: index:" << index << "/" << renderableCount << " item:" << item.GetName() << "  clippingMode:" << item.GetClippingMode() << std::endl;

    if( item.GetClippingMode() != Dali::Renderer::CLIPPING_DISABLED )
    {
      ++clippingId;
    }

    item.SetClippingId( clippingId );
  }
#endif
  //#####################################################################################
  //#####################################################################################
  //#####################################################################################
  //#####################################################################################

#if 0
  //TODO
  int renderItemCount = renderList.Count();
  for( int r = 0; r < renderItemCount ;++r )
  {

    RenderItem& item = renderList.GetItem( r );
    //item.GetNode().GetChildren()...

    // DFS build clipping order
    const Node& node = item.GetNode();
    const NodeContainer& children = node.GetChildren();
    //int childrenCount = children.Count();

    std::cout << "renderItem:" << r << " childrenCount:" << children.Count() << std::endl;

    // Recurse children
    int i = 0;
    const NodeIter endIter = children.End();
    for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
    {
      Node& child = **iter;

      std::cout << "child: i:" << i << "  rendererCount:" << child.GetRendererCount() << std::endl;
      //child.GetRendererAt();

      i++;
    }
  }
#endif

  //#####################################################################################
  //#####################################################################################
  //#####################################################################################
  //#####################################################################################

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
  std::cout << "todor: AddColorRenderers: DONE" << std::endl;
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

  DumpRenderList( overlayRenderList, "Overlay" );
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
  DumpRenderList( stencilRenderList, "Stencil" );
}

void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               RenderInstructionContainer& instructions )
{
  std::cout << "todor: PrepareRenderInstruction START" << std::endl;
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

    std::cout << "todor: PrepareRenderInstruction: Layer:# Stencils:" << stencilRenderablesExist <<
        " Colors:" << colorRenderablesExist <<
        " Overlays:" << overlayRenderablesExist <<
        " TryReuse:" << tryReuseRenderList <<
        std::endl;

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

    std::cout << "todor: AddOverlayRenderers START" << std::endl;
    if ( overlayRenderablesExist )
    {
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, cameraAttachment, stencilRenderablesExist,
                           instruction, sortingHelper, tryReuseRenderList, cull );
    }
    std::cout << "todor: AddOverlayRenderers END" << std::endl;
  }

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
  std::cout << "todor: PrepareRenderInstruction END" << std::endl;
}

} // SceneGraph

} // Internal

} // Dali
