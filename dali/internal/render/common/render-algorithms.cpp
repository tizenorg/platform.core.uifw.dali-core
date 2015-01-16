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

#include <dali/internal/render/renderers/scene-graph-image-renderer.h> // @todo temp mem alloc check
#include <dali/internal/render/renderers/scene-graph-mesh-renderer.h>  // @todo temp mem alloc check
#include <dali/internal/render/renderers/scene-graph-text-renderer.h>  // @todo temp mem alloc check

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
inline void ProcessRenderList( const RenderList& renderList,
                               Context& context,
                               SceneGraph::Shader& defaultShader,
                               BufferIndex bufferIndex,
                               float frameTime,
                               const Matrix& viewMatrix,
                               const Matrix& projectionMatrix,
                               bool cullMode )
{
  DALI_PRINT_RENDER_LIST( renderList );

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

  const unsigned int renderFlags = renderList.GetFlags();

  bool setDepthTest     = ( ( renderFlags & RenderList::DEPTH_TEST ) != 0u );
  bool depthMask        = ( ( renderFlags & RenderList::DEPTH_WRITE ) != 0u );

  GLbitfield clearMask  = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.SetDepthTest( setDepthTest );
  context.DepthMask( depthMask );

  // Stencil testing, writing, and clearing...
  const bool enableStencilTest(  renderFlags & RenderList::STENCIL_TEST );
  const bool enableStencilWrite( renderFlags & RenderList::STENCIL_WRITE );

  context.SetStencilTest( enableStencilTest );

  if( enableStencilTest )
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
    context.Clear( clearMask );
  }

  size_t count = renderList.Count();
  for ( size_t index = 0; index < count; ++index )
  {
    const RenderItem& item = renderList.GetItem( index );

    DALI_PRINT_RENDER_ITEM( item );

    SceneGraph::Renderer* renderer = const_cast< SceneGraph::Renderer* >( item.GetRenderer() );

    // @todo temp mem alloc
    bool ok = true;
    if( 0x0ACEBABE != (renderer->checkSum1 & 0x0FFFFFFF) )
    {
      DALI_LOG_ERROR("Memory corruption? Renderer with wrong start magic number %x\n", renderer->checkSum1);
      ok = false;
    }

    if( 0x0ACEBABE != (renderer->checkSum2 & 0x0FFFFFFF) )
    {
      DALI_LOG_ERROR("Memory corruption? Renderer with wrong end magic number %x\n", renderer->checkSum2);

      ok = false;
    }

    if( 0x1ACEBABE == renderer->checkSum1 &&
        0x1ACEBABE != ((SceneGraph::ImageRenderer*)renderer)->imageCheckSum2)
    {
      DALI_LOG_ERROR("Memory corruption? Image Renderer with wrong end magic number %x\n",((SceneGraph::ImageRenderer*)renderer)->imageCheckSum2);
      ok = false;
    }

    if( 0x2ACEBABE == renderer->checkSum1 &&
        0x2ACEBABE != ((SceneGraph::MeshRenderer*)renderer)->meshCheckSum2)
    {
      DALI_LOG_ERROR("Memory corruption? Mesh Renderer with wrong end magic number %x\n", ((SceneGraph::MeshRenderer*)renderer)->meshCheckSum2);
      ok = false;
    }

    if( 0x3ACEBABE == renderer->checkSum1 &&
        0x3ACEBABE != ((SceneGraph::TextRenderer*)renderer)->textCheckSum2)
    {
      DALI_LOG_ERROR("Memory corruption? Text Renderer with wrong end magic number %x (%d,%d,%d,%d)\n", ((SceneGraph::TextRenderer*)renderer)->textCheckSum2);
      ok = false;
    }

    if(ok)
    {
      const Matrix& modelViewMatrix = item.GetModelViewMatrix();

      renderer->Render( bufferIndex, defaultShader, modelViewMatrix, viewMatrix, projectionMatrix, frameTime, cullMode );
    }
  }
}

void ProcessRenderInstruction( const RenderInstruction& instruction,
                               Context& context,
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
    for( RenderListContainer::SizeType index = 0; index < count; ++index )
    {
      const RenderList* renderList = instruction.GetRenderList( index );

      if(  renderList &&
          !renderList->IsEmpty() )
      {
        ProcessRenderList( *renderList, context, defaultShader, bufferIndex, frameTime, *viewMatrix, *projectionMatrix, instruction.mCullMode );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
