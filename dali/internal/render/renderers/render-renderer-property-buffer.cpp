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

#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderPropertyBuffer::RenderPropertyBuffer( const PropertyBufferDataProvider& propertyBufferDataProvider, bool isIndexBuffer )
: mDataProvider( propertyBufferDataProvider ),
  mGpuBuffer( NULL ),
  mIsIndexBuffer( isIndexBuffer )
{
}

RenderPropertyBuffer::~RenderPropertyBuffer()
{
}

void RenderPropertyBuffer::DoUpload( Context& context, BufferIndex bufferIndex )
{
  // Update the GpuBuffer and check for data changes
  bool needsUpload = UpdateGpuBuffer( context, bufferIndex ) || mDataProvider.HasDataChanged( bufferIndex );

  if( needsUpload )
  {
    std::size_t dataSize = mDataProvider.GetDataSize( bufferIndex );
    mGpuBuffer->UpdateDataBuffer( dataSize, &(mDataProvider.GetData( bufferIndex )[0]) );
    mGpuBuffer->SetStride( mDataProvider.GetElementSize( bufferIndex ) );
  }
}

void RenderPropertyBuffer::BindBuffer( Context& context, Program& progam )
{
  mGpuBuffer->Bind();
}

void RenderPropertyBuffer::EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{

  // Check if attribute locations are cached already
  if( mAttributesLocation.Size() == 0 )
  {
    UpdateAttributeLocations( context, bufferIndex, program );
  }

  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  DALI_ASSERT_DEBUG( attributeCount == mAttributesLocation.Size() && "Incorrect number of attributes!" );

  GLsizei elementSize = mDataProvider.GetElementSize( bufferIndex );

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLuint attributeLocation = mAttributesLocation[i];
    GLint attributeSize = mDataProvider.GetAttributeSize( bufferIndex, i );
    size_t attributeOffset = mDataProvider.GetAttributeOffset( bufferIndex, i );

    context.VertexAttribPointer( attributeLocation,
                                 attributeSize,
                                 GL_FLOAT, // TODO: MESH_REWORK
                                 GL_FALSE,  // Not normalized
                                 elementSize,
                                 (void*)attributeOffset );
  }
}

void RenderPropertyBuffer::DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLuint attributeLocation = mAttributesLocation[i];
    context.DisableVertexAttributeArray( attributeLocation );
  }
}

bool RenderPropertyBuffer::UpdateGpuBuffer( Context& context, BufferIndex bufferIndex )
{
  bool changed = false;

  // Check if we have the correct gpu-buffer
  unsigned int gpuBufferId = mDataProvider.GetGpuBufferId( bufferIndex );
  if ( ! mGpuBuffer )
  {
    DALI_ASSERT_DEBUG( false && "TODO: MESH_REWORK" );
//    mGpuBuffer /*= gpuBufferCache.GetGpuBuffer( gpuBufferId ) */;
    (void )gpuBufferId;

    if( mIsIndexBuffer )
    {
      mGpuBuffer = new GpuBuffer( context, GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
      unsigned int dataSize = mDataProvider.GetDataSize( bufferIndex );
      mGpuBuffer->UpdateDataBuffer( dataSize, mDataProvider.GetData( bufferIndex ) );
    }
    else
    {
      mGpuBuffer = new GpuBuffer( context, GpuBuffer::ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
      std::size_t dataSize = mDataProvider.GetDataSize( bufferIndex );
      mGpuBuffer->UpdateDataBuffer( dataSize, mDataProvider.GetData( bufferIndex ) );
      mGpuBuffer->SetStride( mDataProvider.GetElementSize( bufferIndex ) );
    }

    changed = true;
  }

  return changed;
}

void RenderPropertyBuffer::UpdateAttributeLocations( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  mAttributesLocation.Resize( attributeCount );

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    const std::string& attributeName = mDataProvider.GetAttributeName( bufferIndex, i );
    unsigned int index = program.RegisterCustomAttribute( attributeName );
    GLuint attributeLocation = program.GetCustomAttributeLocation( index );

    mAttributesLocation[i] = attributeLocation;
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
