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
#include <dali/devel-api/common/owner-container.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace Render
{
class PropertyBuffer;
}

namespace SceneGraph
{

/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class RenderGeometry
{
public:

  typedef SceneGraph::Geometry::GeometryType GeometryType;
  /**
   * Constructor. Creates a render geometry object with no GPU buffers.
   * @param[in] center The center of the geometry
   * @param[in] geometryType The geometry type
   * @param[in] requiresDepthTest True if geometry requires depth testing, false otherwise
   */
  RenderGeometry( GeometryType geometryType, bool requiresDepthTest );
  /**
   * Destructor
   */
  ~RenderGeometry();

  /**
   * Adds a property buffer to the geometry
   * @param[in] dataProvider The PropertyBuffer data provider
   * @param[in] isIndexBuffer True if the property buffer is intended to be used as an index buffer
   */
  void AddPropertyBuffer( Render::PropertyBuffer* propertyBuffer, bool isIndexBuffer );

  /**
   * Removes a PropertyBuffer from the geometry
   * @param[in] propertyBuffer The property buffer to be removed
   */
  void RemovePropertyBuffer(  const Render::PropertyBuffer* propertyBuffer );

  /**
   * Gets the attribute locations on the shader for the attributes defined in the geometry RenderBuffers
   * @param[out] attributeLocation The vector where the attributes locations will be stored
   * @param[in] program The program
   * @param[in] bufferIndex The current buffer index
   */
  void GetAttributeLocationFromProgram( Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex ) const;

  /**
   * Called from RenderManager to notify the geometry that current rendering pass has finished.
   */
  void OnRenderFinished();

  /**
   * Chack if the attributes for the geometry have changed
   * @return True if vertex buffers have been added or removed since last frame, false otherwise
   */
  bool AttributesChanged() const
  {
    return mAttributesChanged;
  }

  /**
   * Sets the geometry type
   * @param[in] type The new geometry type
   */
  void SetGeometryType( GeometryType type )
  {
    mGeometryType = type;
  }

  /**
   * Sets if the geometry requires depth testing
   * @param[in] requiresDepthTest True if depth testing is required, false otherwise
   */
  void SetRequiresDepthTest( bool requiresDepthTest )
  {
    mRequiresDepthTest = requiresDepthTest;
  }

  /**
   * Check if geometry requires depth testing
   * @return True if depth testing is required, false otherwise
   */
  bool RequiresDepthTest() const
  {
    return mRequiresDepthTest;
  }

  /**
   * Upload the geometry if it has changed, set up the attributes and perform
   * the Draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] attributeLocation The location for the attributes in the shader
   */
  void UploadAndDraw(Context& context,
                     BufferIndex bufferIndex,
                     Vector<GLint>& attributeLocation );

private:

  // PropertyBuffers
  Render::PropertyBuffer* mIndexBuffer;
  Vector<Render::PropertyBuffer*> mVertexBuffers;

  GeometryType  mGeometryType;

  // Booleans
  bool mRequiresDepthTest : 1;
  bool mHasBeenUpdated : 1;
  bool mAttributesChanged : 1;

};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SAMPLER_DATA_PROVIDER_H
