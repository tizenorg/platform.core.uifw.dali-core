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
#include <dali/internal/render/renderers/render-renderer.h>

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

/**
 * Helper to set the depth function
 * @param[in] context The GL context
 * @param[in] depthFunction The depth function
 */
inline void SetDepthFunction( Context& context, DepthFunction::Type depthFunction )
{
  switch( depthFunction )
  {
    case DepthFunction::OFF:
    {
      context.EnableDepthBuffer( false );
      break;
    }
    case DepthFunction::NEVER:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_NEVER );
      break;
    }
    case DepthFunction::ALWAYS:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_ALWAYS );
      break;
    }
    case DepthFunction::LESS:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_LESS );
      break;
    }
    case DepthFunction::GREATER:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_GREATER );
      break;
    }
    case DepthFunction::EQUAL:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_EQUAL );
      break;
    }
    case DepthFunction::NOT_EQUAL:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_NOTEQUAL );
      break;
    }
    case DepthFunction::LESS_EQUAL:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_LEQUAL );
      break;
    }
    case DepthFunction::GREATER_EQUAL:
    {
      context.EnableDepthBuffer( true );
      context.DepthFunc( GL_GEQUAL );
      break;
    }
  }
}

/**
 * Sets up the scissor test if required.
 * @param[in] renderList The render list from which to get the clipping flag
 * @param[in] context The context
 */
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

/**
 * Sets the render flags for depth testing and stencil buffer
 *
 * @param[in] renderList The render list from which to get the render flags
 * @param[in] context The context
 */
inline void SetRenderFlags( const RenderList& renderList, Context& context )
{
  const unsigned int renderFlags = renderList.GetFlags();

  if( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u )
  {
    //Enable depth testing
    context.EnableDepthBuffer( true );
  }
  else
  {
    //Disable depth test and depth write
    context.EnableDepthBuffer( false );
    context.DepthMask( false );
  }

  GLbitfield clearMask   = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

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
 * @param[in] viewMatrix The view matrix from the appropriate camera.
 * @param[in] projectionMatrix The projection matrix from the appropriate camera.
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix )
{
  DALI_PRINT_RENDER_LIST( renderList );

  SetScissorTest( renderList, context );
  SetRenderFlags( renderList, context );

  if( renderList.HasColorRenderItems() )
  {
    bool depthBufferEnabled = ( ( renderList.GetFlags() & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
    size_t count = renderList.Count();

    if( depthBufferEnabled )
    {
      for ( size_t index = 0; index < count; ++index )
      {
        const RenderItem& item = renderList.GetItem( index );
        DALI_PRINT_RENDER_ITEM( item );

        DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();
        context.DepthMask( ( depthWriteMode == DepthWriteMode::AUTO && item.mIsOpaque ) ||
                           ( depthWriteMode == DepthWriteMode::ON ) );

        SetDepthFunction( context, item.mRenderer->GetDepthFunction() );
        if( !item.mSkipIfBatched )
        {
           item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                           item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, item.mBatchRenderGeometry, !item.mIsOpaque );
        }
        item.mSkipIfBatched = false;
        item.mBatchRenderGeometry = NULL;
      }
    }
    else
    {
      for ( size_t index = 0; index < count; ++index )
      {
        const RenderItem& item = renderList.GetItem( index );
        DALI_PRINT_RENDER_ITEM( item );
        if( !item.mSkipIfBatched )
        {
          item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                                  item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, item.mBatchRenderGeometry, !item.mIsOpaque );
        }
        item.mSkipIfBatched = false;
        item.mBatchRenderGeometry = NULL;
      }
    }
  }
  else
  {
    size_t count = renderList.Count();
    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

      item.mRenderer->Render( context, textureCache, bufferIndex, *item.mNode, defaultShader,
                              item.mModelMatrix, item.mModelViewMatrix, viewMatrix, projectionMatrix, item.mSize, item.mBatchRenderGeometry, !item.mIsOpaque );
    }
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
                               SceneGraph::TextureCache& textureCache,
                               SceneGraph::Shader& defaultShader,
                               BufferIndex bufferIndex )
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
        ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, *viewMatrix, *projectionMatrix );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
