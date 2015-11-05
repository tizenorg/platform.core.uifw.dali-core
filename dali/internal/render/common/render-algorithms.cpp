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
#include <dali/public-api/math/matrix.h>
#include <dali/internal/render/common/batch-info.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/shaders/program.h>

using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;
using Dali::Internal::SceneGraph::RenderInstruction;

namespace
{

Dali::Matrix gViewProjectionMatrix( false ); ///< a shared matrix to calculate the MVP matrix, dont want to store it in object to reduce storage overhead

}


namespace Dali
{

namespace Internal
{

namespace Render
{

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

  bool enableDepthBuffer = ( ( renderFlags & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );

  GLbitfield clearMask   = ( renderFlags & RenderList::DEPTH_CLEAR ) ? GL_DEPTH_BUFFER_BIT : 0u;

  context.EnableDepthBuffer( enableDepthBuffer );

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
 * @param[in] cullMode True if the renderers should be subjected to clipspace culling
 */
inline void ProcessRenderList(
  const RenderList& renderList,
  Context& context,
  SceneGraph::TextureCache& textureCache,
  SceneGraph::Shader& defaultShader,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix,
  bool cullMode )
{
  DALI_PRINT_RENDER_LIST( renderList );

  SetScissorTest( renderList, context );
  SetRenderFlags( renderList, context );

  if( renderList.HasColorRenderItems() )
  {
    bool depthBufferEnabled = ( ( renderList.GetFlags() & RenderList::DEPTH_BUFFER_ENABLED ) != 0u );
    size_t count = renderList.Count();
    for ( size_t index = 0; index < count; ++index )
    {
      const RenderItem& item = renderList.GetItem( index );
      DALI_PRINT_RENDER_ITEM( item );

      if( !item.IsBatch() )
      {
        // Enable depth writes if depth buffer is enabled and item is opaque
        context.DepthMask( depthBufferEnabled && ( item.IsOpaque() || item.GetRenderer().RequiresDepthTest() ) );

        item.GetRenderer().Render( context, textureCache, bufferIndex, item.GetNode(), defaultShader, item.GetModelViewMatrix(), viewMatrix, projectionMatrix, cullMode, !item.IsOpaque() );
      }
      else // Render batched geometry
      {
        // Only batching for text in LAYER_2D, no depth-test required
        context.DepthMask( false );
        context.SetBlend( true );

        BatchInfo* batchInfo = item.GetBatchInfo();

        if( batchInfo->program )
        {
          batchInfo->program->Use();

          // Set some standard uniforms
          GLint loc = batchInfo->program->GetUniformLocation( Program::UNIFORM_MVP_MATRIX );
          if( Program::UNIFORM_UNKNOWN != loc )
          {
            Matrix::Multiply( gViewProjectionMatrix, viewMatrix, projectionMatrix );
            batchInfo->program->SetUniformMatrix4fv( loc, 1, gViewProjectionMatrix.AsFloat() );
          }

          loc = batchInfo->program->GetUniformLocation( Program::UNIFORM_COLOR );
          if( Program::UNIFORM_UNKNOWN != loc )
          {
            batchInfo->program->SetUniform4f( loc, batchInfo->color.r, batchInfo->color.g, batchInfo->color.b, batchInfo->color.a );
          }

          // Bind the texture for this batch
          Internal::Texture* texture = textureCache.GetTexture( batchInfo->textureId );
          if( texture )
          {
            textureCache.BindTexture( texture, batchInfo->textureId, GL_TEXTURE_2D, static_cast<TextureUnit>(0) );

            loc = batchInfo->program->GetUniformLocation( Program::UNIFORM_SAMPLER );
            if( Program::UNIFORM_UNKNOWN != loc )
            {
              batchInfo->program->SetUniform1i( loc, 0 );
            }
          }

          // Upload the batched geometry
          GpuBuffer* vertexBuffer = context.GetNextVertexBuffer();
          vertexBuffer->UpdateDataBuffer( batchInfo->vertices.Count(), &batchInfo->vertices[0], GpuBuffer::DYNAMIC_DRAW );
          vertexBuffer->Bind( GpuBuffer::ARRAY_BUFFER );
          context.VertexAttribPointer( 0, 3/*Vector3*/, GL_FLOAT, GL_FALSE/*Not normalized*/, 20/*stride for 5 floats*/, (void*)0 );

          GpuBuffer* indexBuffer = context.GetNextIndexBuffer();
          indexBuffer->UpdateDataBuffer( batchInfo->indices.Count(), &batchInfo->vertices[0], GpuBuffer::DYNAMIC_DRAW );
          indexBuffer->Bind( GpuBuffer::ELEMENT_ARRAY_BUFFER );
          context.VertexAttribPointer( 1, 2/*Vector3*/, GL_FLOAT, GL_FALSE/*Not normalized*/, 20/*stride*/, (void*)12/*offset for 3 floats*/ );

          // Draw the mofo
          unsigned int numIndices = indexBuffer->GetBufferSize();
          context.DrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
        }
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

      item.GetRenderer().Render( context, textureCache, bufferIndex, item.GetNode(), defaultShader, item.GetModelViewMatrix(), viewMatrix, projectionMatrix, cullMode, !item.IsOpaque() );
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
        ProcessRenderList( *renderList, context, textureCache, defaultShader, bufferIndex, *viewMatrix, *projectionMatrix, instruction.mCullMode );
      }
    }
  }
}

} // namespace Render

} // namespace Internal

} // namespace Dali
