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

// CLASS HEADERS
#include <dali/internal/render/renderers/render-renderer-property-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace
{

using namespace Dali;
using Dali::Property;
using Dali::Internal::PropertyImplementationType;

Dali::GLenum GetPropertyImplementationGlType( Property::Type& propertyType )
{
  Dali::GLenum type = GL_BYTE;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::TYPE_COUNT:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::RECTANGLE:
    case Property::ROTATION:
    {
      // types not supported by gl
      break;
    }
    case Property::BOOLEAN:
    {
      type = GL_BYTE;
      break;
    }
    case Property::INTEGER:
    case Property::UNSIGNED_INTEGER:
    {
      type = GL_SHORT;
      break;
    }
    case Property::FLOAT:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    {
      type = GL_FLOAT;
      break;
    }
  }

  return type;
}

size_t GetPropertyImplementationGlSize( Property::Type& propertyType )
{
  Dali::GLenum type = 1u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::TYPE_COUNT:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::RECTANGLE:
    case Property::ROTATION:
    {
      // types not supported by gl
      break;
    }
    case Property::BOOLEAN:
    {
      type = 1u;
      break;
    }
    case Property::INTEGER:
    case Property::UNSIGNED_INTEGER:
    {
      type = 2u;
      break;
    }
    case Property::FLOAT:
    case Property::VECTOR2:
    case Property::VECTOR3:
    case Property::VECTOR4:
    case Property::MATRIX3:
    case Property::MATRIX:
    {
      type = 4u;
      break;
    }
  }

  return type;
}

void UploadAttribute( Dali::Internal::Context& context,
                      Dali::GLint attributeLocation,
                      Dali::GLint attributeSize,
                      size_t attributeOffset,
                      Dali::Property::Type attributeType,
                      Dali::GLsizei elementSize )
{
  // TODO: MESH_REWORK  Matrices need multiple calls to this function
  context.VertexAttribPointer( attributeLocation,
                               attributeSize  / GetPropertyImplementationGlSize(attributeType),
                               GetPropertyImplementationGlType(attributeType),
                               GL_FALSE,  // Not normalized
                               elementSize,
                               (void*)attributeOffset );
}

}  // unnamed namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderPropertyBuffer::RenderPropertyBuffer( const PropertyBufferDataProvider& propertyBufferDataProvider,
                                            GpuBuffer::Target gpuBufferTarget,
                                            GpuBuffer::Usage gpuBufferUsage )
: mDataProvider( propertyBufferDataProvider ),
  mGpuBuffer( NULL ),
  mGpuBufferTarget( gpuBufferTarget ),
  mGpuBufferUsage( gpuBufferUsage )
{
}

RenderPropertyBuffer::~RenderPropertyBuffer()
{
}

void RenderPropertyBuffer::Upload( Context& context, BufferIndex bufferIndex )
{
  // Check if we have a gpu-buffer
  unsigned int gpuBufferId = 0; // TODO: MESH_REWORK FIX THIS  mDataProvider.GetGpuBufferId( bufferIndex );
  if ( ! mGpuBuffer )
  {
    // TODO: MESH_REWORK
//    mGpuBuffer /*= gpuBufferCache.GetGpuBuffer( gpuBufferId ) */;
    (void )gpuBufferId;

    mGpuBuffer = new GpuBuffer( context, mGpuBufferTarget, mGpuBufferUsage );
  }

  // Update the GpuBuffer
  if ( mGpuBuffer && mDataProvider.HasDataChanged( bufferIndex ) )
  {
    std::size_t dataSize = mDataProvider.GetDataSize( bufferIndex );
    DALI_ASSERT_DEBUG( dataSize && "No data in the property buffer!" );

    const void *data = &(mDataProvider.GetData( bufferIndex )[0]);
    Vector<unsigned short> ushortData;

    // Index buffer needs to be unsigned short which is not supported by the property system
    if( mGpuBufferTarget == GpuBuffer::ELEMENT_ARRAY_BUFFER )
    {
      ushortData.Resize( dataSize );
      const unsigned int* unsignedData = static_cast<const unsigned int*>(data);
      unsigned int numberOfElements = dataSize / sizeof(unsigned int);
      for( unsigned int i = 0; i < numberOfElements; ++i )
      {
        ushortData[i] = unsignedData[i];
      }
      data = &(ushortData[0]);
    }

    mGpuBuffer->UpdateDataBuffer( dataSize, data );
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
    GLint attributeLocation = mAttributesLocation[i];
    if( attributeLocation != -1 )
    {
      context.EnableVertexAttributeArray( attributeLocation );

      GLint attributeSize = mDataProvider.GetAttributeSize( bufferIndex, i );
      size_t attributeOffset = mDataProvider.GetAttributeOffset( bufferIndex, i );
      Property::Type attributeType = mDataProvider.GetAttributeType( bufferIndex, i );

      UploadAttribute( context,
                       attributeLocation,
                       attributeSize,
                       attributeOffset,
                       attributeType,
                       elementSize );
    }
  }
}

void RenderPropertyBuffer::DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    GLint attributeLocation = mAttributesLocation[i];
    if( attributeLocation != -1 )
    {
      context.DisableVertexAttributeArray( attributeLocation );
    }
  }
}

void RenderPropertyBuffer::UpdateAttributeLocations( Context& context, BufferIndex bufferIndex, Program& program )
{
  unsigned int attributeCount = mDataProvider.GetAttributeCount( bufferIndex );
  mAttributesLocation.Resize( attributeCount );

  for( unsigned int i = 0; i < attributeCount; ++i )
  {
    const std::string& attributeName = mDataProvider.GetAttributeName( bufferIndex, i );
    unsigned int index = program.RegisterCustomAttribute( attributeName );
    GLint attributeLocation = program.GetCustomAttributeLocation( index );

    if( -1 == attributeLocation )
    {
      DALI_LOG_WARNING( "Attribute not found in the shader: %s\n", attributeName.c_str() );
    }
    mAttributesLocation[i] = attributeLocation;
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
