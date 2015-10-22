#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>

namespace Dali
{

namespace Internal
{
class MessageBase;

namespace Render
{
class Renderer;
}
namespace SceneGraph
{

class RenderManager;
class RenderQueue;
class RenderTracker;
class RenderGeometry;
class PropertyBufferDataProvider;
/**
 * A utility class for sending messages to the render-thread.
 */
class RenderMessageDispatcher
{
public:

  /**
   * Constructor
   */
  RenderMessageDispatcher( RenderManager& renderManager, RenderQueue& renderQueue, const SceneGraphBuffers& buffers );

  /**
   * Destructor
   */
  virtual ~RenderMessageDispatcher();

  /**
   * Add a Renderer.
   * @param[in] renderer The renderer to add.
   * @post renderer ownership is transferred.
   */
  void AddRenderer( Render::Renderer& renderer );

  /**
   * Remove a Renderer.
   * @param[in] renderer The renderer to remove.
   * @post renderer will be destroyed in the next Render.
   */
  void RemoveRenderer( Render::Renderer& renderer );

  /**
   * Add a Geometry
   * @param[in] renderGeometry The geometry to add.
   * @post RenderGeometry ownership is transferred.
   */
  void AddGeometry( RenderGeometry& renderGeometry );

  /**
   * Remove a Geometry.
   * @param[in] renderGeometry The geometry to remove.
   * @post RenderGeometry will be destroyed in the next Render.
   */
  void RemoveGeometry( RenderGeometry& renderGeometry );

  /**
   * Add a PropertyBuffer.
   * @param[in] renderGeometry The geometry
   * @param[in] propertyBuffer The PropertyBuffer
   * @param[in] isIndexBuffer True if the buffer is intended to be used as an index buffer
   */
  void AddPropertyBuffer( RenderGeometry& renderGeometry, Render::PropertyBuffer* propertyBuffer, bool isIndexBuffer );

  /**
   * Remove a PropertyBuffer.
   * @param[in] renderGeometry The geometry
   * @param[in] propertyBuffer The PropertyBuffer
   * @post PropertyBuffer will be destroyed in the next Render.
   */
  void RemovePropertyBuffer( RenderGeometry& renderGeometry, Render::PropertyBuffer* propertyBuffer );

  /**
   * Set the geometry type of an existing render geometry
   * @param[in] geometry The render geometry
   * @param[in] geometryType The new geometry type
   */
  void SetGeometryCenter( RenderGeometry& geometry, const Vector3& center );

  /**
   * Set the geometry type of an existing render geometry
   * @param[in] geometry The render geometry
   * @param[in] geometryType The new geometry type
   */
  void SetGeometryType( RenderGeometry& geometry, int geometryType );

  /**
   * Set if an existing geometry requires depth testing
   * @param[in] geometry The render geometry
   * @param[in] requiresDepthTest True if depth testing is required, false otherwise
   */
  void SetGeometryRequiresDepthTest( RenderGeometry& geometry, bool requiresDepthTest );

  /**
   * Add a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post ownership is transferred
   */
  void AddRenderTracker( RenderTracker& renderTracker );

  /**
   * Remove a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post render tracker will be destroyed in the next Render pass.
   */
  void RemoveRenderTracker( RenderTracker& renderTracker );

private:

  RenderManager& mRenderManager;
  RenderQueue& mRenderQueue;

  const SceneGraphBuffers& mBuffers;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDERER_DISPATCHER_H__
