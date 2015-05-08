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
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderQueue;
class PostProcessResourceDispatcher;
class Mesh;

/**
 * Mesh resources consist of vertices, face indices and normals; they are shared (weakly referenced) by Nodes.
 */
class Mesh : public GlResourceOwner
{
public:

  enum ThreadBuffer
  {
    UPDATE_THREAD,
    RENDER_THREAD
  };

  /**
   * Create a new scene graph mesh.
   * @param[in] id The resource ID of the mesh
   * @param[in] postProcessResourceDispatcher The dispatcher
   * @param[in] meshData The mesh data
   * @return A smart-pointer to the newly allocated Mesh.
   */
  static Mesh* New( ResourceId id,
                    PostProcessResourceDispatcher& postProcessResourceDispatcher,
                    RenderQueue& renderQueue,
                    MeshData* meshData )
  {
    return new Mesh( id, postProcessResourceDispatcher, renderQueue, meshData );
  }

  /**
   * Virtual destructor
   */
  virtual ~Mesh();

  /**
   * Set the mesh data
   * @pre Should only be called in the update thread
   * @param[in] meshData The mesh data
   */
  void SetMeshData( MeshData* meshData );

  /**
   * Get the mesh data.
   * The caller can modify this data. If they do, they should call
   * MeshDataUpdated() when finished. This should only be done in the update thread.
   * @param[in] threadBuffer indicates what buffer should be returned
   * @return meshData.
   */
  MeshData& GetMeshData( ThreadBuffer threadBuffer );

  /**
   * Get the mesh data.
   * @param[in] threadBuffer indicates what buffer should be returned
   * @return meshData.
   */
  const MeshData& GetMeshData( ThreadBuffer threadBuffer ) const;

  /**
   * The mesh data has been updated. Allows the renderer to update
   * the vertex buffer objects the next time they are needed.
   * @param[in] bufferIndex to be used in double buffered values.
   * @param[in] threadBuffer indicates what buffer should be used
   * @param[in] meshData The mesh data ownership is passed in through a message, if threadBuffer is UPDATE it should be NULL
   */
  void MeshDataUpdated( BufferIndex bufferIndex, ThreadBuffer threadBuffer, MeshData* meshData );

  /**
   * Sends the vertex data to GL if it has been refreshed. Notify resource manager when it has finished.
   * @pre this function should only be called from the render thread
   * @param[in] context The GL context.
   * @param[in] renderBufferIndex The index that should be accessed in double buffered values.
   */
  void UploadVertexData( Context& context, BufferIndex renderBufferIndex );

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
   * @param[in] threadBuffer indicates what buffer should be used
   * @return the number of indices
   */
  size_t GetFaceIndexCount( ThreadBuffer threadBuffer ) const;

  /**
   * Returns a new
   * @param[in] threadBuffer indicates what buffer should be used
   * @return true if there is any geometry
   **/
  bool HasGeometry( ThreadBuffer threadBuffer ) const;

public: // from GlResourceOwner

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextCreated() {};

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
   * Method to set if the vertex buffer should be refreshed in the render thread
   */
  void RefreshVertexBuffer();

  /**
   * Private constructor; see also Mesh::New()
   */
  Mesh( ResourceId id,
        PostProcessResourceDispatcher& postProcessResourceDispatcher,
        RenderQueue& renderQueue,
        MeshData* meshData );

  // Undefined
  Mesh(const Mesh&);

  // Undefined
  Mesh& operator=(const Mesh& rhs);

protected:
  PostProcessResourceDispatcher& mPostProcessResourceDispatcher;
  SceneGraph::RenderQueue& mRenderQueue;
  /**
   * The mUpdateMeshData will point to a mesh data that was just received
   * or to the MeshData pointed by mRenderMeshData if it's more that one frame old
   */
  MeshData* mUpdateMeshData;              ///< Pointer to MeshData object
  OwnerPointer<MeshData> mRenderMeshData; ///< Owner of the MeshData Object
  OwnerPointer<GpuBuffer> mVertexBuffer;  ///< Vertex buffer
  OwnerPointer<GpuBuffer> mIndicesBuffer; ///< Index buffer
  size_t     mNumberOfVertices;    ///< Number of vertices
  size_t     mNumberOfFaces;       ///< Number of faces
  ResourceId mResourceId;
  bool mRefreshVertexBuffer;              ///< True when GpuBuffers need updating
};

} // namespace SceneGraph

// value types used by messages
template <> struct ParameterType< SceneGraph::Mesh::ThreadBuffer >
: public BasicType< SceneGraph::Mesh::ThreadBuffer > {};
class Context;

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_MESH_H__
