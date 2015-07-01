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
 */

#include <dali/internal/render/renderers/render-geometry.h>

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderGeometry::RenderGeometry( const GeometryDataProvider& geometryDataProvider )
: mGeometryDataProvider( geometryDataProvider ),
  mDataNeedsUploading( true ),
  mShaderChanged( true )
{
}

RenderGeometry::~RenderGeometry()
{
}

void RenderGeometry::GlContextCreated( Context& context )
{
  mDataNeedsUploading = true;
}

void RenderGeometry::GlContextDestroyed()
{
}

void RenderGeometry::UploadAndDraw(
  Context& context,
  Program& program,
  BufferIndex bufferIndex,
  Vector<GLint>& attributeLocation )
{
  UploadVertexData( context, bufferIndex );

  unsigned int base = 0;
  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->BindBuffer( context );
    base += mVertexBuffers[i]->EnableVertexAttributes( context, bufferIndex, attributeLocation, base );
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->BindBuffer( context );
  }

  Draw( context, bufferIndex );

  for( unsigned int i = 0; i < attributeLocation.Count(); ++i )
  {
    context.DisableVertexAttributeArray( attributeLocation[i] );
  }
}

void RenderGeometry::GeometryUpdated()
{
  mDataNeedsUploading = true;
}

void RenderGeometry::UploadVertexData( Context& context, BufferIndex bufferIndex )
{
  if( mDataNeedsUploading )
  {
    for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
    {
      mVertexBuffers[i]->Upload( context, bufferIndex );
    }
    if( mIndexBuffer )
    {
      mIndexBuffer->Upload( context, bufferIndex );
    }

    mDataNeedsUploading = false;
  }
}

//void RenderGeometry::EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
//{
//  OwnerContainer< RenderPropertyBuffer* >::Iterator it = mVertexBuffers.Begin();
//  OwnerContainer< RenderPropertyBuffer* >::ConstIterator end = mVertexBuffers.End();
//  for( ; it != end; ++it )
//  {
//    (*it)->EnableVertexAttributes( context, bufferIndex, program );
//  }
//}
//
//void RenderGeometry::DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
//{
//  OwnerContainer< RenderPropertyBuffer* >::Iterator it = mVertexBuffers.Begin();
//  OwnerContainer< RenderPropertyBuffer* >::ConstIterator end = mVertexBuffers.End();
//  for( ; it != end; ++it )
//  {
//    (*it)->DisableVertexAttributes( context, bufferIndex, program );
//  }
//}

void RenderGeometry::Draw( Context& context, BufferIndex bufferIndex )
{
  unsigned int numIndices(0u);
  if( mIndexBuffer )
  {
    const PropertyBufferDataProvider& indexBuffer = mIndexBuffer->GetDataProvider();
    numIndices = mIndexBuffer->GetDataProvider().GetDataSize(bufferIndex) / indexBuffer.GetElementSize(bufferIndex);
  }

  GeometryDataProvider::GeometryType type = mGeometryDataProvider.GetGeometryType( bufferIndex );
  switch(type)
  {
    case Dali::Geometry::TRIANGLES:
    {
      if( numIndices )
      {
        context.DrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
      }
      else
      {
        const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
        unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
        context.DrawArrays( GL_TRIANGLES, 0, numVertices );
      }
      break;
    }
    case Dali::Geometry::LINES:
    {
      if( numIndices )
      {
        context.DrawElements(GL_LINES, numIndices, GL_UNSIGNED_SHORT, 0);
      }
      else
      {
        const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
        unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
        context.DrawArrays( GL_LINES, 0, numVertices );
      }
      break;
    }
    case Dali::Geometry::POINTS:
    {
      const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
      unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
      std::cout<<"NUmVertices: " << numVertices << std::endl;
      context.DrawArrays(GL_POINTS, 0, numVertices );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( 0 && "Geometry type not supported (yet)" );
      break;
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
