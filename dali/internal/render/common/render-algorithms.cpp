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
#include <dali/internal/render/common/render-algorithms.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>

using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;
using Dali::Internal::SceneGraph::RenderInstruction;

namespace Dali
{

namespace Internal
{

namespace Render
{

inline void SetScissorTest( const RenderList& renderList, Context& context )
{
  // Scissor testing
  if( renderList.IsClipping() )
  {
    context.SetScissorTest( true );

    const Dali::ClippingBox& clip = renderList.GetClippingBox();
    context.Scissor(clip.x, clip.y, clip.width, clip.height);
  }
  else
  {
    context.SetScissorTest( false );
  }
}

inline void SetRenderFlags( const RenderList& renderList, Context& context )
{
  const unsigned int renderFlags = renderList.GetFlags();

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
  bool depthMask         = ( ( renderFlags & RenderList::DEPTH_WRITE ) != 0u );

  GLbitfield clearMask   = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( enableDepthBuffer );
  context.DepthMask( depthMask );

  // Stencil enabled, writing, and clearing...
  const bool enableStencilBuffer( renderFlags & RenderList::STENCIL_BUFFER_ENABLED );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  context.EnableStencilBuffer( enableStencilBuffer );

  if( enableStencilBuffer )
  {
    context.StencilFunc( (enableStencilWrite ? GL_ALWAYS : GL_EQUAL), 1, 0xFF );
    context.StencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    clearMask |= (renderFlags & RenderList::STENCIL_CLEAR) ? GL_STENCIL_BUFFER_BIT : 0u;
  }

  // Write to stencil buffer or color buffer, but not both
  context.StencilMask( enableStencilWrite ? 0xFF : 0x00 );
  context.ColorMask( !enableStencilWrite );

  // Clear depth and/or stencil buffer.
  if( clearMask )
  {
    // only clear if the depth and/or stencil buffer have been written to after a previous clear
    context.Clear( clearMask, Context::CHECK_CACHED_VALUES );
  }
}


/**
 * Process a render-list.
 * @param[in] renderList The render-list to process.
 * @param[in] context The GL context.
 * @param[in] defaultShader The default shader to use.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] frameTime The elapsed time between the last two updates.
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 * @param[in] cullMode True if the renderers should be subjected to clipspace culling
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  float frameTime,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix,
  bool cullMode )
{
  DALI_PRINT_RENDER_LIST( renderList );

  SetScissorTest( renderList, context );
  SetRenderFlags( renderList, context );

  size_t count = renderList.Count();
  for ( size_t index = 0; index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );

    DALI_PRINT_RENDER_ITEM( item );

    SceneGraph::Renderer* renderer = const_cast< SceneGraph::Renderer* >( item.GetRenderer() );
    const Matrix& modelViewMatrix = item.GetModelViewMatrix();

    renderer->Render( context, textureCache, bufferIndex, defaultShader, modelViewMatrix, viewMatrix, projectionMatrix, frameTime, cullMode );
  }
}

/**
 * Render items from the currentIndex until the depth index changes.
 * Leaves currentIndex pointing at the
 *
 * @param[in] renderList The render-list to process.
 * @param[in] context The GL context.
 * @param[in] defaultShader The default shader to use.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] frameTime The elapsed time between the last two updates.
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 * @param[in] cullMode True if the renderers should be subjected to clipspace culling
 */
inline void RenderItemsAtDepthIndex(
  const RenderList&         renderList,
  Context&                  context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader&       defaultShader,
  BufferIndex               bufferIndex,
  float                     frameTime,
  const Matrix&             viewMatrix,
  const Matrix&             projectionMatrix,
  bool                      cullMode,
  int                       depthIndex,
  size_t&                   currentIndex ) // Out parameter
{
  size_t count = renderList.Count();

  for( ; currentIndex < count ; currentIndex++ )
  {
    const RenderItem& renderItem = renderList.GetItem( currentIndex );
    DALI_PRINT_RENDER_ITEM( renderItem );

    if( renderItem.GetDepthIndex() == depthIndex )
    {
      SceneGraph::Renderer* renderer = const_cast< SceneGraph::Renderer* >( renderItem.GetRenderer() );
      const Matrix& modelViewMatrix = renderItem.GetModelViewMatrix();
      renderer->Render( context, textureCache, bufferIndex, defaultShader, modelViewMatrix, viewMatrix, projectionMatrix, frameTime, cullMode );

    }
    else
    {
      break; // Stop iterating when we reach a new depth index
    }
  }
}


/**
 * Process two interleaved render-lists.
 *
 * For each depth index, it will set the flags for the first list,
 * render items in the first list, set flags for the second list and
 * render items from the second list.
 *
 * @param[in] renderList The render-list to process.
 * @param[in] context The GL context.
 * @param[in] defaultShader The default shader to use.
 * @param[in] buffer The current render buffer index (previous update buffer)
 * @param[in] frameTime The elapsed time between the last two updates.
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 * @param[in] cullMode True if the renderers should be subjected to clipspace culling
 */

inline void ProcessInterleavedRenderLists(
  const RenderList&         renderList1,
  const RenderList&         renderList2,
  Context&                  context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader&       defaultShader,
  BufferIndex               bufferIndex,
  float                     frameTime,
  const Matrix&             viewMatrix,
  const Matrix&             projectionMatrix,
  bool                      cullMode )
{

  SetScissorTest( renderList1, context ); // Scissor settings are identical for both lists
  size_t count1 = renderList1.Count();
  size_t count2 = renderList2.Count();
  size_t index1 = 0;
  size_t index2 = 0;

  int depthIndex=renderList1.GetItem( 0 ).GetDepthIndex();

  while( index1 < count1 || index2 < count2 )
  {
    if( index1 < count1 && index2 < count2 )
    {
      // Take the lowest depth index in both lists
      depthIndex = std::min( renderList1.GetItem( index1 ).GetDepthIndex(),
                             renderList2.GetItem( index2 ).GetDepthIndex() );
    }
    else if( index1 < count1 )
    {
      // Items left only in list 1
      depthIndex = renderList1.GetItem( index1 ).GetDepthIndex();
      // @todo MESH_REWORK Clear back to opaque flags and render the remaining
      // items, then quit
    }
    else // index2 < count2
    {
      // Items left only in list 2
      depthIndex = renderList2.GetItem( index2 ).GetDepthIndex();

      // @todo MESH_REWORK Clear back to opaque flags, then transparent flags
      // and render the remaining items, then quit
    }

    // Between each successive depth index, reset the flags to the Opaque flags
    // to clear the depth buffer if required.
    SetRenderFlags( renderList1, context );

    // Find and render items in the first list that correspond to the current depth index
    if( index1 < count1 )
    {
      RenderItemsAtDepthIndex( renderList1, context, textureCache, defaultShader,
                               bufferIndex, frameTime, viewMatrix, projectionMatrix,
                               cullMode, depthIndex,
                               index1 ); // Out parameter
    }

    SetRenderFlags( renderList2, context );
    if( index2 < count2 )
    {
      RenderItemsAtDepthIndex( renderList2, context, textureCache, defaultShader,
                               bufferIndex, frameTime, viewMatrix, projectionMatrix,
                               cullMode, depthIndex,
                               index2 ); // Out parameter
    }
  }
}


void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
                               BufferIndex bufferIndex,
                               float frameTime )
{
  DALI_PRINT_RENDER_INSTRUCTION( instruction, bufferIndex );

  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  DALI_ASSERT_DEBUG( NULL != viewMatrix );
  DALI_ASSERT_DEBUG( NULL != projectionMatrix );

  if( NULL != viewMatrix &&
      NULL != projectionMatrix )
  {
    const RenderListContainer::SizeType count = instruction.RenderListCount();

    // Iterate through each render list in order. If a pair of render lists
    // are marked as interleaved, then process them together.
    for( RenderListContainer::SizeType index = 0; index < count; ++index )
    {
      const RenderList* renderList = instruction.GetRenderList( index );

      if(  renderList &&
          !renderList->IsEmpty() )
      {
        if( renderList->GetInterleave() &&
            index < count-1 &&
            instruction.GetRenderList(index+1)->GetInterleave() )
        {
          ProcessInterleavedRenderLists( *renderList,
                                         *(instruction.GetRenderList(index+1)),
                                         context,
                                         textureCache,
                                         defaultShader,
                                         bufferIndex,
                                         frameTime,
                                         *viewMatrix,
                                         *projectionMatrix,
                                         instruction.mCullMode );
          index++; // Skip over next render list
        }
        else
        {
          ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, frameTime, *viewMatrix, *projectionMatrix, instruction.mCullMode );
        }
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
