#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESH_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESH_H__

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
#include <dali/internal/common/message.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>

/*
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>
*/

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * RenderMesh is the render side counterpart of SceneGraph::Mesh
 * It deals with uploading data to OpenGL and provides information to the renderers.
 */
class RenderMesh : public GlResourceOwner
{
public:
  /**
   * Create a new scene graph render mesh.
   * @return A newly allocated Mesh.
   */
  static RenderMesh* New(MeshData* meshData);

  /**
   * Virtual destructor
   */
  virtual ~RenderMesh();

  /**
   * Set the mesh data
   * @pre Should only be called in the update thread
   * @param[in] meshData The mesh data
   */
  void SetMeshData( MeshData* meshData );

  /**
   * Get the mesh data.
   * @return meshData.
   */
  const MeshData& GetMeshData() const;

  /**
   * The mesh data has been replaced. Allows the renderer to update
   * the vertex buffer objects the next time they are needed.
   * @param[in] meshData The mesh data ownership is passed in through a message, if threadBuffer is UPDATE it should be NULL
   */
  void MeshDataReplaced( MeshData* meshData );

  /**
   * Sends the vertex data to GL
   * @pre this function should only be called from the render thread
   * @param[in] context The GL context.
   */
  void UploadVertexData( Context& context );

  /**
   * Actually perform the vertex upload.
   * @param[in] context The GL context.
   */
  void DoUpload( Context& context );

  /**
   * Bind the vertex and index buffers.
   * @pre this function should only be called from the render thread
   * @param[in] context The GL context.
   */
  void BindBuffers( Context& context );

  /**
   * Get the number of face / line indices. (Note, this is not the number of faces)
   * @return the number of indices
   */
  size_t GetFaceIndexCount() const;

  /**
   * Check if there is geometry.
   * @return true if there is any geometry
   **/
  bool HasGeometry() const;

  /**
   * Method to set if the vertex buffer should be refreshed
   */
  void RefreshVertexBuffer();

private: // from GlResourceOwner

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

private:
  /**
   * Private constructor; see also Mesh::New()
   */
  RenderMesh(MeshData* meshData);

  // Undefined
  RenderMesh(const RenderMesh&);

  // Undefined
  RenderMesh& operator=(const RenderMesh& rhs);

private:
  OwnerPointer<MeshData>  mMeshData;            ///< Owner of the MeshData Object
  OwnerPointer<GpuBuffer> mVertexBuffer;        ///< Vertex buffer
  OwnerPointer<GpuBuffer> mIndicesBuffer;       ///< Index buffer
  size_t                  mNumberOfVertices;     ///< Number of Verices
  size_t                  mNumberOfFaces;       ///< Number of Faces
  bool                    mRefreshVertexBuffer; ///< True when GpuBuffers need updating
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESH_H__
