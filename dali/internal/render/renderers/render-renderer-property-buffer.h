#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_RENDERER_PROPERTY_BUFFER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_RENDERER_PROPERTY_BUFFER_H

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

#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/data-providers/property-buffer-data-provider.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace SceneGraph
{
class NewRenderer;
/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderPropertyBuffer
{
public:
  typedef OwnerContainer< GpuBuffer* > GpuBuffers;

  /**
   * Constructor. Creates a render geometry object with no GPU buffers.
   * @param[in] propertyBufferDataProvider The property-buffer  data provider (to fetch geometry from)
   * @param[in] isIndexBuffer Whether or not this property buffer is an index-buffer
   */
  RenderPropertyBuffer( const PropertyBufferDataProvider& propertyBufferDataProvider, bool isIndexBuffer );

  /**
   * Destructor
   */
  ~RenderPropertyBuffer();

  /**
   * Perform the upload of the buffer
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   */
  void Upload( Context& context, BufferIndex bufferIndex );

  /**
   * Bind the geometry buffers
   * @param[in] context The GL context
   * @param[in] program The shader program to query for attribute locations
   */
  void BindBuffer( Context& context, Program& progam );

  /**
   * Enable the vertex attributes for each vertex buffer from the corresponding
   * shader program.
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] program The shader program to query for attribute locations
   */
  void EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program );

  /**
   * Disable the vertex attributes for each vertex buffer from the corresponding
   * shader program.
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] program The shader program to query for attribute locations
   */
  void DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program );

private: // implementation

  /**
   * Update attribute locations
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] program The shader program to query for attribute locations
   */
  void UpdateAttributeLocations( Context& context, BufferIndex bufferIndex, Program& program );

private:
  const PropertyBufferDataProvider& mDataProvider;  ///< Data provider used by this property buffer
  Vector<GLint> mAttributesLocation;                ///< Location of the attributes for the property buffer in this renderer.
  GpuBuffer* mGpuBuffer;                            ///< Pointer to the GpuBuffer associated with this RenderPropertyBuffer

  bool mIsIndexBuffer;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_RENDERER_PROPERTY_BUFFER_H
