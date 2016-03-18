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
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
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

//todor
//static bool CLIP_DEBUG = false;

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

namespace
{
//todor helper
// Recursively dumps node tree.
void DumpNode( const Node* node, int indent = 0 )
{
  // Indent based on depth.
  for( int t = 0; t < indent; ++t )
  {
    std::cout << "  ";
  }
  //std::cout << "Parent:" << (int)node.GetParent() << std::endl;
  const NodeContainer& children = node->GetChildren();
  std::cout << "Node: childrenCount:" << children.Count() << std::endl;
  int i = 0;
  // Iterate through child nodes.
  const NodeIter endIter = children.End();
  for ( NodeIter iter = children.Begin(); iter != endIter; ++iter )
  {
    Node* child = *iter; //todor remove child tmpvar
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
    const Node* node = item.mNode;
    std::cout << "RenderItem: " << r << std::endl;
    DumpNode( node, 1 );
  }
}

//todor helper
std::string PrintBin( uint16_t value )
{
  std::string outString( "0000000000000000" );
  int pos = 16 - 1;

  for( int i = 15; i >= 0; i-- )
  {
    outString[ pos-- ] = ( value & 1 ) + '0';
    value >>= 1;
  }

  return outString;
}

  //todor
  typedef bool ( *ComparitorPointer )( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs );

  static std::vector< ComparitorPointer > sComparitors;
} // Unnamed namespace


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
  if( lhs.renderItem->mDepthIndex == rhs.renderItem->mDepthIndex )
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
  return lhs.renderItem->mDepthIndex < rhs.renderItem->mDepthIndex;
}

/**
 * Function which sorts render items by depth index then by instance
 * ptrs of shader/geometry/material.
 * Takes into account clipping hierarchy.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
//todor mod
bool CompareItemsWithClipping( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  // @todo MESH_REWORK Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs)

  //todor get sort modifier:
#if 0
  std::cout << "todor: ### Doing compare: depth:" << lhsNode->GetDepth() <<
      "  UCID:" << lhsNode->GetClippingId() <<
      "  CD:" << lhsNode->GetClippingDepth() <<
      "  CM:" << (int)lhsNode->GetClippingMode() << std::endl;
#endif

  //todor optimise:
  const Node* lhsNode = lhs.renderItem->mNode;
  const Node* rhsNode = rhs.renderItem->mNode;

#if 1
  //todor
  if( lhsNode->GetClippingId() == rhsNode->GetClippingId() )
  {
    bool lhsClip = lhsNode->GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    bool rhsClip = rhsNode->GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    if( lhsClip == rhsClip )
    {
#endif
      if( lhs.renderItem->mDepthIndex == rhs.renderItem->mDepthIndex )
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
      return lhs.renderItem->mDepthIndex < rhs.renderItem->mDepthIndex;
#if 1
    }
    return lhsClip && !rhsClip;
  }
  return lhsNode->GetClippingId() < rhsNode->GetClippingId();
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
  bool lhsIsOpaque = lhs.renderItem->mIsOpaque;
  if( lhsIsOpaque == rhs.renderItem->mIsOpaque )
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
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/geometry/material.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
//todor mod
bool CompareItems3DWithClipping( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  //todor get sort modifier:
#if 0
  std::cout << "todor: ### Doing compare: depth:" << lhsNode->GetDepth() <<
      "  UCID:" << lhsNode->GetClippingId() <<
      "  CD:" << lhsNode->GetClippingDepth() <<
      "  CM:" << (int)lhsNode->GetClippingMode() << std::endl;
#endif

  //todor optimise:
  const Node* lhsNode = lhs.renderItem->mNode;
  const Node* rhsNode = rhs.renderItem->mNode;

#if 1
  //todor
  if( lhsNode->GetClippingId() == rhsNode->GetClippingId() )
  {
    bool lhsClip = lhsNode->GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    bool rhsClip = rhsNode->GetClippingMode() != Dali::ClippingMode::CLIPPING_DISABLED;
    if( lhsClip == rhsClip )
    {
#endif


      bool lhsIsOpaque = lhs.renderItem->mIsOpaque;
      if( lhsIsOpaque == rhs.renderItem->mIsOpaque )
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


#if 1
    }
    return lhsClip && !rhsClip;
  }
  return lhsNode->GetClippingId() < rhsNode->GetClippingId();
#endif
}

void InitializePrepareRenderInstructions()
{
  //todor
#if 0
  // Set up a container of comparators for fast run-time selection.
  sComparitors.push_back( CompareItems );
  sComparitors.push_back( CompareItemsWithClipping );
  sComparitors.push_back( CompareItems3D );
  sComparitors.push_back( CompareItems3DWithClipping );
#else
  // todorscnow
  // todordel: force old sort comparitors.
  sComparitors.push_back( CompareItems );
  sComparitors.push_back( CompareItems );
  sComparitors.push_back( CompareItems3D );
  sComparitors.push_back( CompareItems3D );
#endif
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     Renderable& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::Camera& camera,
                                     bool isLayer3d,
                                     bool cull )
{
#if 1
  if( CLIP_DEBUG ) std::cout << "todor: ++++++++++++++++++++ AddRendererToRenderList: RendererName:" << renderable.mRenderer->GetName() << "  ClippingMode:" << (int)renderable.mRenderer->GetClippingMode() << " ++++START++++" << std::endl;
#endif

  bool inside( true );
  if ( cull && !renderable.mRenderer->GetShader().HintEnabled( Dali::Shader::HINT_MODIFIES_GEOMETRY ) )
  {
    const Vector4& boundingSphere = renderable.mNode->GetBoundingSphere();
    inside = (boundingSphere.w > Math::MACHINE_EPSILON_1000) &&
             (camera.CheckSphereInFrustum( updateBufferIndex, Vector3(boundingSphere), boundingSphere.w ) );
  }

  if ( inside )
  {
    Renderer::Opacity opacity = renderable.mRenderer->GetOpacity( updateBufferIndex, *renderable.mNode );
    if( opacity != Renderer::TRANSPARENT )
    {
      // Get the next free RenderItem
      RenderItem& item = renderList.GetNextFreeItem();
      item.mRenderer = &renderable.mRenderer->GetRenderer();
      item.mNode = renderable.mNode;
      item.mIsOpaque = (opacity == Renderer::OPAQUE);

      //todorscnow putback unsafe
      item.SetName( renderable.mRenderer->GetName() );//todor
      if( CLIP_DEBUG ) std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#if 0
      item.SetClippingMode( renderable.mRenderer->GetClippingMode() );//todor
      item.SetName( renderable.mRenderer->GetName() );//todor
      std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#endif
#if 1
      //std::cout << "todor: RL: #2# co: " << renderList.Count() << std::endl;
      //item.SetClippingMode( Dali::Renderer::CLIPPING_DISABLED );//todor
      //item.SetName( "aaa" );//todor
      //item.SetName2( "aaa" );//todor
      //std::cout << "todor: Adding renderer to list: " << item.GetName() << std::endl;
#endif

      if( isLayer3d )
      {
        item.mDepthIndex = renderable.mRenderer->GetDepthIndex();
      }
      else
      {
        item.mDepthIndex = renderable.mRenderer->GetDepthIndex() + static_cast<int>( renderable.mNode->GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER;
      }

      // Save ModelView matrix onto the item
      renderable.mNode->GetWorldMatrixAndSize( item.mModelMatrix, item.mSize );
      Matrix::Multiply( item.mModelViewMatrix, item.mModelMatrix, viewMatrix );
      //std::cout << "todor: RL: #3# co: " << renderList.Count() << std::endl;
    }
  }
  if( CLIP_DEBUG ) std::cout << "todor: AddRendererToRenderList:                                                   ++++DONE++++" << std::endl;
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the items to
 * @param renderers to render
 * NodeRendererContainer Node-Renderer pairs
 * @param viewMatrix used to calculate modelview matrix for the items
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
//todor
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableContainer& renderers,
                                      const Matrix& viewMatrix,
                                      SceneGraph::Camera& camera,
                                      bool isLayer3d,
                                      bool cull)
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  //std::cout << "todor: AddRenderersToRenderList: gh1" << std::endl;
  unsigned int rendererCount( renderers.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    //std::cout << "todor: AddRenderersToRenderList: gh2" << std::endl;
    AddRendererToRenderList( updateBufferIndex, renderList, renderers[i], viewMatrix, camera, isLayer3d, cull );
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
    // check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong things
    // Render list is sorted so at this stage renderers may be in different order
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
 * Sort render items
 * @param bufferIndex The buffer to read from
 * @param renderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param respectClippingOrder Sort with the correct clipping hierarchy.
 */
//todor
inline void SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper, bool respectClippingOrder )
{
  if( CLIP_DEBUG ) std::cout << "todor: SortRenderItems" << std::endl;
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

      item.mRenderer->SetSortAttributes( bufferIndex, sortingHelper[ index ] );

      // the default sorting function should get inlined here
      sortingHelper[ index ].zValue = Internal::Layer::ZValue( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

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

      item.mRenderer->SetSortAttributes( bufferIndex, sortingHelper[ index ] );
      sortingHelper[ index ].zValue = (*sortFunction)( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }

  //todor comment
  // Here we detemine which comparitor (of the 4) to use.
  // The 3D comparitors start at index 2 (hence adding 2).
  // The Clipping comparitors are offset from the 3D comparitors by 1. So 1 is added if clipping.
  int comparitorIndex = ( respectClippingOrder ? 1 : 0 ) + ( layer.GetBehavior() == Dali::Layer::LAYER_3D ? 2 : 0 );
  std::stable_sort( sortingHelper.begin(), sortingHelper.end(), sComparitors[ comparitorIndex ] );

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, sortingHelper[ index ].renderItem->mRenderer);
  }
  if( CLIP_DEBUG ) std::cout << "todor: SortRenderItems: DONE" << std::endl;
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
 * @param updateBufferIndex       to use
 * @param layer                   to get the renderers from
 * @param viewmatrix              for the camera from rendertask
 * @param camera                  to use the view frustum
 * @param stencilRenderablesExist is true if there are stencil (clipping) renderers on this layer, to optimize sorting if not
 * @param instruction             to fill in
 * @param sortingHelper           to use for sorting the renderitems (to avoid reallocating)
 * @param tryReuseRenderList      whether to try to reuse the cached items from the instruction
 * @param cull                    Whether frustum culling is enabled or not
 */
//todor
#if 0
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::Camera& camera,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               bool tryReuseRenderList,
                               bool cull )
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

  AddRenderersToRenderList( updateBufferIndex, renderList, layer.colorRenderables, viewMatrix, camera, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

  //DumpRenderList( renderList, "Color" );

  SortRenderItems( updateBufferIndex, renderList, layer, sortingHelper, stencilRenderablesExist );

  //todordel
  //Set render flags
  unsigned int flags = 0u;

  //todor DO WE NEED THIS? check
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  // Special optimization. If this layer has got exactly one renderer
  // and this renderer is not writing to the depth buffer there is no point on enabling
  // depth buffering
  if ( ( renderList.Count() == 1 ) &&
       (( renderList.GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::OFF ) ||
        ( renderList.GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::AUTO && !renderList.GetItem(0).mIsOpaque )))
  {
    //Nothing to do here
  }
  else if( !layer.IsDepthTestDisabled())
  {
    flags |= RenderList::DEPTH_BUFFER_ENABLED | RenderList::DEPTH_CLEAR;
  }

  renderList.ClearFlags();
  renderList.SetFlags( flags ); //todor can we combine clear+set ?
  std::cout << "todor: AddColorRenderers: DONE" << std::endl;
}
#endif

/**
 * Add overlay renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param camera to use
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
//todor
#if 0
inline void AddOverlayRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::Camera& camera,
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

  //todor DO WE NEED THIS? check
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

  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, viewMatrix, camera, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

  DumpRenderList( overlayRenderList, "Overlay" );

  // Clipping hierarchy is irrelevant when sorting overlay items.
  SortRenderItems( updateBufferIndex, overlayRenderList, layer, sortingHelper, false );
}
#endif
//todor ###########################################################################
inline bool AddRenderers( RenderableContainer& renderables,
                          Layer& layer,
                          RenderInstruction& instruction,
                          bool tryReuseRenderList,
                          RenderList** renderList )
{
  *renderList = &( instruction.GetNextFreeRenderList( renderables.Size() ) );
  ( *renderList )->SetClipping( layer.IsClipping(), layer.GetClippingBox() );

  // Set render flags
  ( *renderList )->ClearFlags();

  // Try to reuse cached RenderItems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, **renderList, renderables ) )
    {
      return true;
    }
  }

  return false;
}

#if 0
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::CameraAttachment& cameraAttachment,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               bool tryReuseRenderList,
                               bool cull )
{
  RenderableContainer& renderables = layer.colorRenderables;
  RenderList* renderList = NULL;

  AddRenderers( renderables, layer, stencilRenderablesExist, instruction, tryReuseRenderList, &renderList );
  renderList->SetHasColorRenderItems( true );
  AddRenderersToRenderList( updateBufferIndex, *renderList, renderables, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

  //DumpRenderList( renderList, "Color" );
  SortRenderItems( updateBufferIndex, *renderList, layer, sortingHelper, stencilRenderablesExist );

  //todordel
  //Set render flags
  unsigned int flags = 0u;

  //todor DO WE NEED THIS? check
  if( stencilRenderablesExist )
  {
    flags = RenderList::STENCIL_BUFFER_ENABLED;
  }

  // Special optimization if depth test is disabled or if only one opaque rendered in the layer (for example background image)
  // and this renderer does not need depth test against itself (e.g. mesh)
  // and if this layer has got exactly one opaque renderer
  // and this renderer is not interested in depth testing
  // (i.e. is an image and not a mesh)

  if ( ( renderList->Count() == 1 ) &&
       ( !renderList->GetRenderer( 0 ).RequiresDepthTest() ) &&
       ( !renderList->GetItem( 0 ).IsOpaque() ) )
  {
    //Nothing to do here
  }
  else if( !layer.IsDepthTestDisabled() )
  {
    flags |= RenderList::DEPTH_BUFFER_ENABLED | RenderList::DEPTH_CLEAR;
  }

  //renderList.ClearFlags(); //done by AR
  renderList->SetFlags( flags ); //todor can we combine clear+set ?
  std::cout << "todor: AddColorRenderers: DONE" << std::endl;
}
#endif
#if 0
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
  RenderableContainer& renderables = layer.overlayRenderables;
  RenderList* renderList = NULL;

  AddRenderers( renderables, layer, stencilRenderablesExist, instruction, tryReuseRenderList, &renderList );
  renderList->SetHasColorRenderItems( false );
  AddRenderersToRenderList( updateBufferIndex, *renderList, layer.overlayRenderables, viewMatrix, cameraAttachment, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

  // Clipping hierarchy is irrelevant when sorting overlay items.
  //DumpRenderList( overlayRenderList, "Overlay" );
  SortRenderItems( updateBufferIndex, *renderList, layer, sortingHelper, false );
}
#endif
//todor del
#if 0
/**
 * Add stencil renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param camera to use
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddStencilRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::Camera& camera,
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

  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, viewMatrix, camera, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
  DumpRenderList( stencilRenderList, "Stencil" );
}
#endif
//todor
#if 0
void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               bool stencilRenderablesExist,
                               RenderInstructionContainer& instructions )
{
  std::cout << "todor: PrepareRenderInstruction START" << std::endl;
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::Camera& camera = renderTask.GetCamera();

  const SortedLayersIter endIter = sortedLayers.end();
  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    //todordel const bool stencilRenderablesExist( !layer.stencilRenderables.Empty() );
    const bool colorRenderablesExist( !layer.colorRenderables.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers( &renderTask.GetCamera() ) );

    std::cout << "todor: PrepareRenderInstruction: Layer:# " << //Stencils:" << stencilRenderablesExist <<
        " Colors:" << colorRenderablesExist <<
        " Overlays:" << overlayRenderablesExist <<
        " TryReuse:" << tryReuseRenderList <<
        std::endl;

#if 0
    //todor del
    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( colorRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, camera, instruction, tryReuseRenderList, cull );
    }
#endif

    if ( colorRenderablesExist )
    {
      AddColorRenderers( updateBufferIndex,
                         layer,
                         viewMatrix,
                         camera,
                         stencilRenderablesExist,
                         instruction,
                         sortingHelper,
                         tryReuseRenderList,
                         cull );
    }

    std::cout << "todor: AddOverlayRenderers START" << std::endl;
    if ( overlayRenderablesExist )
    {
      // We deliberately ignore stencils for overlay renderers, this speeds up the sorting.
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, camera, false,
                           instruction, sortingHelper, tryReuseRenderList, cull );
    }
    std::cout << "todor: AddOverlayRenderers END" << std::endl;
  }

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
  std::cout << "todor: PrepareRenderInstruction END" << std::endl;
}
#endif

void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               bool stencilRenderablesExist,
                               RenderInstructionContainer& instructions )
{
  if( CLIP_DEBUG ) std::cout << "todor: PrepareRenderInstruction START" << std::endl;
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::Camera& camera = renderTask.GetCamera();

  const SortedLayersIter endIter = sortedLayers.end();
  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    const bool colorRenderablesExist( !layer.colorRenderables.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers( &renderTask.GetCamera() ) );

    if( CLIP_DEBUG ) std::cout << "todor: PrepareRenderInstruction: Layer:#  Colors:" << colorRenderablesExist << " Overlays:" << overlayRenderablesExist << " TryReuse:" << tryReuseRenderList << std::endl;

    RenderList* renderList = NULL;

    if ( colorRenderablesExist )
    {
      if( CLIP_DEBUG ) std::cout << "todor: AddColorRenderers: START" << std::endl;
      RenderableContainer& renderables = layer.colorRenderables;
      if( !AddRenderers( renderables, layer, instruction, tryReuseRenderList, &renderList ) )
      {
        renderList->SetHasColorRenderItems( true );
        AddRenderersToRenderList( updateBufferIndex, *renderList, renderables, viewMatrix, camera, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

        //DumpRenderList( renderList, "Color" );
        SortRenderItems( updateBufferIndex, *renderList, layer, sortingHelper, stencilRenderablesExist );

        // Set render flags
        // Special optimization. If this layer has got exactly one renderer
        // and this renderer is not writing to the depth buffer there is no point
        // in enabling depth buffering.
        if ( ( renderList->Count() == 1 ) &&
             ( ( renderList->GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::OFF ) ||
               ( renderList->GetRenderer( 0 ).GetDepthWriteMode() == DepthWriteMode::AUTO && !renderList->GetItem( 0 ).mIsOpaque ) ) )
        {
          // Nothing to do here (flags have already been cleared).
        }
        else if( !layer.IsDepthTestDisabled() )
        {
          // Note, the flags have already been cleared.
          renderList->SetFlags( RenderList::DEPTH_BUFFER_ENABLED | RenderList::DEPTH_CLEAR );
        }
      }
      if( CLIP_DEBUG ) std::cout << "todor: AddColorRenderers: DONE" << std::endl;
    }

    if ( overlayRenderablesExist )
    {
      if( CLIP_DEBUG ) std::cout << "todor: AddOverlayRenderers START" << std::endl;
      RenderableContainer& renderables = layer.overlayRenderables;

      if( !AddRenderers( renderables, layer, instruction, tryReuseRenderList, &renderList ) )
      {
        renderList->SetHasColorRenderItems( false );
        AddRenderersToRenderList( updateBufferIndex, *renderList, renderables, viewMatrix, camera, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );

        // Clipping hierarchy is irrelevant when sorting overlay items.
        //DumpRenderList( overlayRenderList, "Overlay" );
        SortRenderItems( updateBufferIndex, *renderList, layer, sortingHelper, false );
      }
      if( CLIP_DEBUG ) std::cout << "todor: AddOverlayRenderers END" << std::endl;
    }
  }

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
  if( CLIP_DEBUG ) std::cout << "todor: PrepareRenderInstruction END" << std::endl;
}


} // SceneGraph

} // Internal

} // Dali
