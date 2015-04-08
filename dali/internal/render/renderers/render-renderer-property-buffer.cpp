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

#include <dali/internal/render/renderers/render-renderer-property-buffer.h>

#include <dali/internal/render/data-providers/property-buffer-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

NewRenderer::PropertyBuffer::PropertyBuffer( const PropertyBufferDataProvider& propertyBufferDataProvider )
: mDataProvider( propertyBufferDataProvider ),
  mGpuBuffer( NULL ),
  mGpuBufferId( 0 )
{
}

NewRenderer::PropertyBuffer::~PropertyBuffer()
{
}

void NewRenderer::PropertyBuffer::DoUpload( Context& context, BufferIndex bufferIndex )
{
  // Update the GpuBuffer and check for data changes
  bool needsUpload = UpdateGpuBuffer( bufferIndex ) || mDataProvider.HasDataChanged( bufferIndex );

  if( needsUpload )
  {
    std::size_t dataSize = mDataProvider.GetDataSize( bufferIndex );
    mGpuBuffer->UpdateDataBuffer( dataSize, mDataProvider.GetData( bufferIndex ) );
    mGpuBuffer->SetStride( mDataProvider.GetElementSize( bufferIndex ) );
  }
}

void NewRenderer::PropertyBuffer::BindBuffer( Context& context, Program& progam )
{

}

void NewRenderer::PropertyBuffer::EnableVertexAttributes( Context& context, Program& progam )
{

}

void NewRenderer::PropertyBuffer::DisableVertexAttributes( Context& context, Program& program )
{

}

bool NewRenderer::PropertyBuffer::UpdateGpuBuffer( BufferIndex bufferIndex  )
{
  bool changed = false;

  // Check if we have the correct gpu-buffer
  unsigned int gpuBufferId = mDataProvider.GetGpuBufferId( bufferIndex );
  if ( mGpuBufferId != gpuBufferId || !mGpuBuffer )
  {
    mGpuBufferId = gpuBufferId;
    DALI_ASSERT_DEBUG( false && "TODO: MESH_REWORK" );
//    mGpuBuffer /*= gpuBufferCache.GetGpuBuffer( mGpuBufferId ) */;

    changed = true;
  }

  return changed;
}

void RenderGeometry::BindBuffers()
{
  for( GpuBuffers::Iterator iter=mVertexBuffers.Begin(); iter != mVertexBuffers.End(); ++iter )
  {
    (*iter)->Bind();
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->Bind();
  }
}

void RenderGeometry::EnableVertexAttributes( Context* context, Program& program )
{
  // @todo Loop thru the array of vertex buffers
  // @todo Use AttributeDataProvider to get the attrs and enable them
  // Need mapping from gpu buffers index to a particular attributes
  Vector4 *vertex=0;

  unsigned int gpuBufferIndex = 0;

  RegisterAndCacheAttributes();

  GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  context->VertexAttribPointer( positionLoc,
                                2,         // 2D position
                                GL_FLOAT,
                                GL_FALSE,  // Not normalized
                                mVertexBuffers[gpuBufferIndex]->GetStride(),
                                &vertex->x );

  context->EnableVertexAttributeArray( positionLoc );

  GLint textureCoordsLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );
  context->VertexAttribPointer( textureCoordsLoc,
                                2,         // Texture Coords = U, V
                                GL_FLOAT,
                                GL_FALSE,
                                mVertexBuffers[gpuBufferIndex]->GetStride(),
                                &vertex->z );
  context->EnableVertexAttributeArray( textureCoordsLoc );
}

void RenderGeometry::DisableVertexAttributes( Context* context, Program& program )
{
  // @todo Loop thru the array of vertex buffers
  // @todo Use AttributeDataProvider to get the attrs and disable them
  GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  GLint textureCoordsLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );
  context->DisableVertexAttributeArray( positionLoc );
  context->DisableVertexAttributeArray( textureCoordsLoc );
}

void RenderGeometry::Draw( Context* context, BufferIndex bufferIndex, const GeometryDataProvider& geometry )
{
  GeometryDataProvider::GeometryType type = geometry.GetGeometryType( bufferIndex );

  unsigned int numIndices = 0;
  const PropertyBuffer* indexBuffer = geometry.GetIndexBuffer();

  if( indexBuffer )
  {
    numIndices = /* TODO: MESH_REWORK remove this 2, should implement unsigned short properties  */ 2 * indexBuffer->GetDataSize(bufferIndex) / indexBuffer->GetElementSize(bufferIndex);
  }

  switch(type)
  {
    case Dali::Geometry::TRIANGLES:
    {
      context->DrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
      break;
    }
    case Dali::Geometry::LINES:
    {
      context->DrawElements(GL_LINES, numIndices, GL_UNSIGNED_SHORT, 0);
      break;
    }
    case Dali::Geometry::POINTS:
    {
      GpuBuffer* firstVertexBuffer = mVertexBuffers[0];

      unsigned int numVertices = 0;
      GLuint stride = firstVertexBuffer->GetStride();
      if( stride != 0 )
      {
        numVertices = firstVertexBuffer->GetBufferSize() / stride;
      }

      context->DrawArrays(GL_POINTS, 0, numVertices );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( 0 && "Geometry type not supported (yet)" );
      break;
    }
  }
}

void RenderGeometry::RegisterAndCacheAttributes()
{

}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
