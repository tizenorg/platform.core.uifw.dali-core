#ifndef __DALI_INTERNAL_SCENE_GRAPH_MESH_H__
#define __DALI_INTERNAL_SCENE_GRAPH_MESH_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>
#include <dali/internal/render/renderers/render-mesh.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderQueue;
class Mesh;
class RenderMesh;

/**
 * Mesh resources consist of vertices, face indices and normals; they are shared (weakly referenced) by Nodes.
 */
class Mesh
{
public:

  /**
   * Create a new scene graph mesh.
   * @param[in] id The resource ID of the mesh
   * @param[in] postProcessResourceDispatcher The dispatcher
   * @param[in] meshData The mesh data
   * @param[in] renderMesh Pointer to the render side object
   * @return A smart-pointer to the newly allocated Mesh.
   */
  static Mesh* New( ResourceId id,
                    RenderQueue& renderQueue,
                    MeshData* meshData,
                    RenderMesh *renderMesh );

  /**
   * Virtual destructor
   */
  virtual ~Mesh();

  /**
   * Set the mesh data
   * @param[in] bufferIndex to be used in double buffered values.
   * @param[in] meshData The mesh data
   */
  void SetMeshData( BufferIndex bufferIndex, MeshData* meshData );

  /**
   * Get a the mesh data const reference.
   * The caller can modify this data. If they do, they should call
   * MeshDataUpdated() when finished.
   * @param[in] bufferIndex to be used in double buffered values.
   * @return meshData.
   */
  const MeshData& GetMeshData( BufferIndex bufferIndex ) const;

  /**
   * Get the mesh data.
   * The caller can modify this data. If they do, they should call
   * MeshDataUpdated() when finished.
   * @param[in] bufferIndex to be used in double buffered values.
   * @return meshData.
   */
  MeshData& GetMeshData( BufferIndex bufferIndex );

  /**
   * @brief The mesh data has been changed.
   * @param[in] bufferIndex to be used in double buffered values.
   */
  void MeshDataUpdated( BufferIndex bufferIndex );

  /**
   * Check if there is any geometry in this mesh
   * @return true if there is any geometry
   **/
  bool HasGeometry( BufferIndex bufferIndex ) const;

  /**
   * Get the Render side object.
   * @return a pointer to the render side object
   */
  RenderMesh* GetRenderMesh();

private:
  /**
   * Private constructor; see also Mesh::New()
   */
  Mesh( ResourceId id,
        RenderQueue& renderQueue,
        MeshData* meshData,
        RenderMesh* renderMesh );

  // Undefined
  Mesh(const Mesh&);

  // Undefined
  Mesh& operator=(const Mesh& rhs);

protected:
  ResourceId    mResourceId;        ///< Resource ID of the mesh
  RenderQueue&  mRenderQueue;       ///< RenderQueue to send messages to the RenderMesh
  RenderMesh*   mRenderMesh;        ///< Weak pointer to RenderMesh object
  MeshData*     mMeshData;          ///< Weak pointer to MeshData object
  size_t        mNumberOfVertices;  ///< Number of vertices
  size_t        mNumberOfFaces;     ///< Number of faces
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_MESH_H__
