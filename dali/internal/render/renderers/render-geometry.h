#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_GEOMETRY_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_GEOMETRY_H

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

#include <dali/public-api/common/dali-vector.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-renderer-property-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace SceneGraph
{
class RenderDataProvider;
class GeometryDataProvider;

/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderGeometry
{
public:

  /**
   * Constructor. Creates a render geometry object with no GPU buffers.
   */
  RenderGeometry( const GeometryDataProvider& geometryDataProvider );

  void AddVertexBuffer( const PropertyBufferDataProvider& dataProvider, GpuBuffer::Target gpuBufferTarget, GpuBuffer::Usage gpuBufferUsage )
  {
    RenderPropertyBuffer* renderPropertyBuffer = new RenderPropertyBuffer( dataProvider, gpuBufferTarget, gpuBufferUsage );
    mVertexBuffers.PushBack( renderPropertyBuffer );
  }

  void AddIndexBuffer( const PropertyBufferDataProvider& dataProvider )
  {
    RenderPropertyBuffer* renderPropertyBuffer = new RenderPropertyBuffer( dataProvider,GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
    mIndexBuffer = renderPropertyBuffer;
  }

  /**
   * Destructor
   */
  ~RenderGeometry();

  /**
   * Called on Gl Context created
   */
  void GlContextCreated( Context& context );

  /**
   * Called on Gl Context destroyed.
   */
  void GlContextDestroyed();

  /**
   * Upload the geometry if it has changed, set up the attributes and perform
   * the Draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] program The shader program to query for attribute locations
   * @param[in] bufferIndex The current buffer index
   * @param[in] attributeLocation The location for the attributes in the shader
   */
  void UploadAndDraw(Context& context,
                     Program& program,
                     BufferIndex bufferIndex,
                     Vector<GLint>& attributeLocation );

  /**
   * Tell the object that the geometry has been updated.
   * It will be uploaded on the next UploadAndDraw call.
   */
  void GeometryUpdated();

  /**
   * Upload the vertex data if it needs uploading.
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] geometryDataProvider The geometry data provider (to fetch geometry from)
   */
  void UploadVertexData( Context& context,
                         BufferIndex bufferIndex
                       );

  void QueryAttributeLocation( Vector<GLint>& attributeLocation, Program program, BufferIndex bufferIndex )
  {
    attributeLocation.Clear();

    for( size_t i(0); i< mVertexBuffers.Size(); ++i )
    {
      unsigned int attributeCount = mVertexBuffers[i]->GetDataProvider().GetAttributeCount( bufferIndex );
      for( unsigned int j = 0; j < attributeCount; ++j )
      {
        const std::string& attributeName = mVertexBuffers[i]->GetDataProvider().GetAttributeName( bufferIndex, j );
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

private:

  /**
   * Bind the geometry buffers
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] program The shader program to query for attribute locations
   */
  void BindBuffers( Context& context,
                    BufferIndex bufferIndex,
                    Program& program );

//  /**
//   * Enable the vertex attributes for each vertex buffer from the corresponding
//   * shader program.
//   * @param[in] context The GL context
//   * @param[in] bufferIndex The current buffer index
//   * @param[in] program The shader program to query for attribute locations
//   */
//  void EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& progam );
//
//  /**
//   * Disable the vertex attributes for each vertex buffer from the corresponding
//   * shader program.
//   * @param[in] context The GL context
//   * @param[in] bufferIndex The current buffer index
//   * @param[in] program The shader program to query for attribute locations
//   */
//  void DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program );

  /**
   * Perform the correct draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] dataProviders The data providers (to fetch geometry from)
   */
  void Draw( Context& context,
             BufferIndex bufferIndex
           );

private:
  // PropertyBuffers
  OwnerPointer< RenderPropertyBuffer > mIndexBuffer;
  OwnerContainer< RenderPropertyBuffer* > mVertexBuffers;

  const GeometryDataProvider& mGeometryDataProvider;

  // Booleans
  bool mDataNeedsUploading;
  bool mShaderChanged;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H
