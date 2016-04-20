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
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderGeometry::RenderGeometry( GeometryType type, bool requiresDepthTest )
: mIndices(NULL),
  mIndexBuffer(NULL),
  mIndicesChanged(false),
  mGeometryType( type ),
  mRequiresDepthTest(requiresDepthTest ),
  mHasBeenUpdated(false),
  mAttributesChanged(true)
{
}

RenderGeometry::~RenderGeometry()
{
}

void RenderGeometry::GlContextCreated( Context& context )
{
}

void RenderGeometry::GlContextDestroyed()
{
}

void RenderGeometry::AddPropertyBuffer( Render::PropertyBuffer* propertyBuffer )
{
  mVertexBuffers.PushBack( propertyBuffer );
  mAttributesChanged = true;
}

void RenderGeometry::SetIndexBuffer( const Dali::Vector<unsigned short>* indices )
{
  mIndices = indices;
  mIndicesChanged = true;
}

void RenderGeometry::RemovePropertyBuffer( const Render::PropertyBuffer* propertyBuffer )
{
  size_t bufferCount = mVertexBuffers.Size();
  for( size_t i(0); i<bufferCount; ++i )
  {
    if( propertyBuffer == mVertexBuffers[i] )
    {
      //This will delete the gpu buffer associated to the RenderPropertyBuffer if there is one
      mVertexBuffers.Remove( mVertexBuffers.Begin()+i);
      mAttributesChanged = true;
      break;
    }
  }
}

void RenderGeometry::GetAttributeLocationFromProgram( Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex ) const
{
  attributeLocation.Clear();

  for( size_t i(0); i< mVertexBuffers.Size(); ++i )
  {
    unsigned int attributeCount = mVertexBuffers[i]->GetAttributeCount();
    for( unsigned int j = 0; j < attributeCount; ++j )
    {
      const std::string& attributeName = mVertexBuffers[i]->GetAttributeName( j );
      unsigned int index = program.RegisterCustomAttribute( attributeName );
      GLint location = program.GetCustomAttributeLocation( index );

      if( -1 == location )
      {
        DALI_LOG_WARNING( "Attribute not found in the shader: %s\n", attributeName.c_str() );
      }

      attributeLocation.PushBack( location );
    }
  }
}

void RenderGeometry::OnRenderFinished()
{
  mHasBeenUpdated = false;
  mAttributesChanged = false;
}

void RenderGeometry::UploadAndDraw(
    Context& context,
    BufferIndex bufferIndex,
    Vector<GLint>& attributeLocation )
{
  if( !mHasBeenUpdated )
  {
    // Update buffers
    if( mIndicesChanged )
    {
      if( mIndices == NULL || mIndices->Empty() )
      {
        mIndexBuffer = NULL;
      }
      else
      {
        if ( mIndexBuffer == NULL )
        {
          mIndexBuffer = new GpuBuffer( context );
        }

        std::size_t buuferSize =  sizeof( unsigned short ) * mIndices->Size();
        mIndexBuffer->UpdateDataBuffer( buuferSize, &((*mIndices)[0]), GpuBuffer::STATIC_DRAW, GpuBuffer::ELEMENT_ARRAY_BUFFER );
      }

      mIndicesChanged = false;
    }

    for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
    {
      if( !mVertexBuffers[i]->Update( context ) )
      {
        //Vertex buffer is not ready ( Size, data or format has not been specified yet )
        return;
      }
    }

    mHasBeenUpdated = true;
  }

  //Bind buffers to attribute locations
  unsigned int base = 0;
  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->BindBuffer( GpuBuffer::ARRAY_BUFFER );
    base += mVertexBuffers[i]->EnableVertexAttributes( context, attributeLocation, base );
  }

  GLenum geometryGLType(0);
  switch(mGeometryType)
  {
    case Dali::Geometry::TRIANGLES:
    {
      geometryGLType = GL_TRIANGLES;
      break;
    }
    case Dali::Geometry::LINES:
    {
      geometryGLType = GL_LINES;
      break;
    }
    case Dali::Geometry::POINTS:
    {
      geometryGLType = GL_POINTS;
      break;
    }
    case Dali::Geometry::TRIANGLE_STRIP:
    {
      geometryGLType = GL_TRIANGLE_STRIP;
      break;
    }
    case Dali::Geometry::TRIANGLE_FAN:
    {
      geometryGLType = GL_TRIANGLE_FAN;
      break;
    }
    case Dali::Geometry::LINE_LOOP:
    {
      geometryGLType = GL_LINE_LOOP;
      break;
    }
    case Dali::Geometry::LINE_STRIP:
    {
      geometryGLType = GL_LINE_STRIP;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( 0 && "Geometry type not supported (yet)" );
      break;
    }
  }

  //Draw call
  if( mIndexBuffer && geometryGLType != GL_POINTS )
  {
    //Indexed draw call
    mIndexBuffer->Bind( GpuBuffer::ELEMENT_ARRAY_BUFFER );
    context.DrawElements(geometryGLType, mIndices->Size(), GL_UNSIGNED_SHORT, 0);
  }
  else
  {
    //Unindex draw call
    unsigned int numVertices = mVertexBuffers[0]->GetElementCount();
    context.DrawArrays( geometryGLType, 0, numVertices );
  }

  //Disable atrributes
  for( unsigned int i = 0; i < attributeLocation.Count(); ++i )
  {
    if( attributeLocation[i] != -1 )
    {
      context.DisableVertexAttributeArray( attributeLocation[i] );
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
