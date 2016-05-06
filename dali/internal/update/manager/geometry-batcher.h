#ifndef DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H
#define DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/common/render-list.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Geometry;
}

namespace SceneGraph
{
struct BatchInfo;
struct BatchStreamMarker;

class Node;
class RenderInstructionContainer;

class GeometryBatcher
{
  typedef Vector< unsigned short > IndexBuffer;

public:

  GeometryBatcher();
  ~GeometryBatcher();

  /**
   * @brief Updates all batches
   * @param updateManager Pointer to the update manager
   * @param index Update buffer index
   * @param instructions Container of render instructions
   */
  void Update( UpdateManager* updateManager, BufferIndex index, RenderInstructionContainer& instructions );

  /**
   * Marks all batches for deletion for which the node is a batch parent
   * @param[in] node batch parent node
   */
  void DiscardBatch( Node* node );

private:

  /**
   * Rebuild batches for set of render items
   * @param[in] updateManager Pointer to the update manager
   * @param[in] container Container with render items
   * @param[in] bufferIndex current index of update buffer
   */
  void RebuildBatches( UpdateManager* updateManager, RenderItemContainer& container, unsigned int bufferIndex );

  /**
   * @brief Deletes all resources used by the dead batches ( buffers, helper objects )
   * @param[in] updateManager pointer to the update manager
   */
  void DiscardDeadBatches( UpdateManager* updateManager );

  /**
   * @brief Creates set of batch stream markers per frame
   * @param[in] container Container of render items
   */
  void GenerateBatchMarkers( RenderItemContainer& container );

  /**
   * @brief Creates new geometry for batch by analyzing memory requirments and allocating
   * sufficient space for vertex and index buffers.
   * @param[in] updateManager Pointer to the update manager
   * @param[in] marker BatchStreamMarker associated with batch being processed
   * @param[in] itemsPtr Pointer to the array of render items
   * @param[in,out] batchInfo Batch info object
   */
  void CreateGeometry( UpdateManager* updateManager, BatchStreamMarker& marker, RenderItem **itemsPtr, BatchInfo& info );

  /**
   * @brief Clones vertex format of source geometry and stores generated data in the batchInfo object
   * @param[in] sourceGeometry Geometry of the very first batched item
   * @param[out] batchInfo Batch info object used to store cloned vertex format
   * @return true if format could be clone, otherwise batch will be skipped
   */
  bool CloneVertexFormat( const Render::Geometry* sourceGeometry, BatchInfo& batchInfo );

  /**
   * @brief Transforms single item vertices inside the batch vertex buffer
   * @param[in] item Render item being processed
   * @param[in] info batch descriptor
   * @param[in] itemIndexBuffer Index buffer of processed item
   * @param[in,out] indexIter iterator within source index buffer
   * @param[in,out] vertexIter iterator within source vertex buffer
   * @param[out] outVertexPtr result storage
   * @param[in] bufferIndex current update buffer index
   */
  void TransformItemVertices( RenderItem* item,
                              BatchInfo &info,
                              const Vector< unsigned short >* itemIndexBuffer,
                              unsigned int& indexIter,
                              unsigned int& vertexIter,
                              char* outVertexPtr,
                              unsigned int bufferIndex );

private:

  struct Impl* mImpl;

};

} // SceneGraph

} // Internal

} // Dali


#endif
