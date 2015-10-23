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

// FIXME
#include <dali/public-api/math/matrix.h>
#include <dali/internal/render/renderers/render-new-renderer.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/shaders/program.h>

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

static Matrix gModelViewProjectionMatrix( false ); ///< a shared matrix to calculate the MVP matrix, dont want to store it in object to reduce storage overhead
static Matrix3 gNormalMatrix; ///< a shared matrix to calculate normal matrix, dont want to store it in object to reduce storage overhead

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

inline void SetMatrices( Program& program,
                         const Matrix& modelMatrix,
                         const Matrix& viewMatrix,
                         const Matrix& projectionMatrix,
                         const Matrix& modelViewMatrix,
                         const Matrix& modelViewProjectionMatrix )
{
  GLint loc = program.GetUniformLocation(Program::UNIFORM_MODEL_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelMatrix.AsFloat() );
  }
  loc = program.GetUniformLocation( Program::UNIFORM_VIEW_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetViewMatrix() != &viewMatrix )
    {
      program.SetViewMatrix( &viewMatrix );
      program.SetUniformMatrix4fv( loc, 1, viewMatrix.AsFloat() );
    }
  }
  // set projection matrix if program has not yet received it this frame or if it is dirty
  loc = program.GetUniformLocation( Program::UNIFORM_PROJECTION_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetProjectionMatrix() != &projectionMatrix )
    {
      program.SetProjectionMatrix( &projectionMatrix );
      program.SetUniformMatrix4fv( loc, 1, projectionMatrix.AsFloat() );
    }
  }
  loc = program.GetUniformLocation(Program::UNIFORM_MODELVIEW_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelViewMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_MVP_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelViewProjectionMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_NORMAL_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    gNormalMatrix = modelViewMatrix;
    gNormalMatrix.Invert();
    gNormalMatrix.Transpose();
    program.SetUniformMatrix3fv( loc, 1, gNormalMatrix.AsFloat() );
  }
}


void RenderWithBatching( const RenderList& renderList,
                         size_t batchBegin,
                         size_t batchEnd,
                         Context& context,
                         SceneGraph::TextureCache& textureCache,
                         SceneGraph::Shader& defaultShader,
                         BufferIndex bufferIndex,
                         const Matrix& viewMatrix,
                         const Matrix& projectionMatrix )
{
  const RenderItem& item = renderList.GetItem( batchBegin );

  Render::NewRenderer& firstRenderer = static_cast< Render::NewRenderer& >( item.GetRenderer() );

  Program* program = firstRenderer.mShader->GetProgram();
  if( !program )
  {
    return;
  }
  program->Use();

  const SceneGraph::MaterialDataProvider& material = firstRenderer.mRenderDataProvider->GetMaterial();

  context.SetCustomBlendColor( material.GetBlendColor( bufferIndex ) );

  // Set blend source & destination factors
  context.BlendFuncSeparate( material.GetBlendSrcFactorRgb( bufferIndex ),
                             material.GetBlendDestFactorRgb( bufferIndex ),
                             material.GetBlendSrcFactorAlpha( bufferIndex ),
                             material.GetBlendDestFactorAlpha( bufferIndex ) );

  // Set blend equations
  context.BlendEquationSeparate( material.GetBlendEquationRgb( bufferIndex ),
                                 material.GetBlendEquationAlpha( bufferIndex ) );

  const SceneGraph::NodeDataProvider& node = item.GetNode();
  const Matrix& modelMatrix = node.GetModelMatrix( bufferIndex );
  const Matrix& modelViewMatrix = item.GetModelViewMatrix();
  Matrix::Multiply( gModelViewProjectionMatrix, modelViewMatrix, projectionMatrix );

  SetMatrices( *program, modelMatrix, viewMatrix, projectionMatrix, modelViewMatrix, gModelViewProjectionMatrix );

  // set color uniform
  GLint loc = program->GetUniformLocation( Program::UNIFORM_COLOR );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    const Vector4& color = node.GetRenderColor( bufferIndex );
    program->SetUniform4f( loc, color.r, color.g, color.b, color.a );
  }

  size_t count = renderList.Count();
  for ( size_t index = 0; index < count; ++index )
  {
    //const RenderItem& item = renderList.GetItem( index );

    //Render::NewRenderer& renderer = static_cast< Render::NewRenderer >( item.GetRenderer() );

    // TODO
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

      //Enable depth writes if depth buffer is enabled and item is opaque
      context.DepthMask( depthBufferEnabled && item.IsOpaque() );

      //Check whether batching is enabled for a group of renderers
      Render::Renderer& renderer = item.GetRenderer();
      if( renderer.GetBatchingEnabled() )
      {
        size_t batchBegin = index;
        for( ; index < count; ++index )
        {
          // TODO - Check that renderers share same shader etc.
          if( ! renderer.GetBatchingEnabled() )
          {
            break;
          }
        }

        RenderWithBatching( renderList, batchBegin, index, context, textureCache, defaultShader, bufferIndex, viewMatrix, projectionMatrix  );
      }
      else
      {
        renderer.Render( context, textureCache, bufferIndex, item.GetNode(), defaultShader, item.GetModelViewMatrix(), viewMatrix, projectionMatrix, cullMode, !item.IsOpaque() );
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
