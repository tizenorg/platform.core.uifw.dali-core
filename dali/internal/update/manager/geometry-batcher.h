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

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/common/render-list.h>

// PUBLIC INCLUDES
#include <dali/public-api/common/dali-vector.h>

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

public:

  GeometryBatcher();
  ~GeometryBatcher();

  /**
   * Assigns the update manager object
   * @param[in] updateManager Pointer to instance of update manager
   */
  void SetUpdateManager( UpdateManager* updateManager );

  /**
   * Updates all batches
   * @param[in] index Update buffer index
   * @param[in] instructions Container of render instructions
   */
  void Update( BufferIndex index, RenderInstructionContainer& instructions );

  /**
   * Marks all batches for deletion for which the node is a batch parent
   * @param[in] node batch parent node
   */
  void DiscardBatch( Node* node, int updateBufferIndex );

  /**
   * Deletes all resources used by the dead batches ( buffers, helper objects )
   */
  void DiscardDeadBatches( int updateBufferIndex );

private:

  /**
   * Rebuild batches for a set of render items
   * @param[in,out] container Container with render items, items will be modified during batching
   * @param[in] bufferIndex current index of update buffer
   */
  void RebuildBatches( RenderItemContainer& container, unsigned int bufferIndex );

private:
  /**
   * @brief Creates a set of batch stream markers per frame.
   * Batch stream marker describes begining and end of render items which
   * can create a single batch and associates that with BatchInfo structure.
   * @param[in] container Container of render items
   */
  void GenerateBatchMarkers( RenderItemContainer& container );

  /**
   * @brief Creates new geometry for batch by analyzing memory requirments and allocating
   * sufficient space for vertex and index buffers.
   * @param[in] marker BatchStreamMarker associated with batch being processed
   * @param[in] itemsPtr Pointer to the array of render items
   * @param[in,out] batchInfo Batch info object
   */
  void CreateGeometry( BatchStreamMarker& marker, RenderItem **itemsPtr, BatchInfo& info );

  /**
   * @brief Clones vertex format of source geometry and stores generated data in the batchInfo object
   * @param[in] sourceGeometry Geometry of the very first batched item
   * @param[out] batchInfo Batch info object used to store cloned vertex format
   * @return true if format could be cloned, otherwise batch will be skipped
   */
  bool CloneVertexFormat( const Render::Geometry* sourceGeometry, BatchInfo& batchInfo );

  /**
   * @brief Transforms single item vertices inside the batch vertex buffer
   * @param[in] item Render item being processed
   * @param[in] info Batch descriptor
   * @param[in] itemIndexBuffer Index buffer of processed item
   * @param[in,out] indexIter iterator within source index buffer
   * @param[in,out] vertexIter iterator within source vertex buffer
   * @param[out] outVertexPtr result storage
   * @param[in] bufferIndex current update buffer index
   */
  void TransformItemVertices( RenderItem* item,
                              BatchInfo& info,
                              const Vector< unsigned short >* itemIndexBuffer,
                              unsigned int& indexIter,
                              unsigned int& vertexIter,
                              char* outVertexPtr,
                              unsigned int bufferIndex );

  /**
   * Updates internam BatchList based on current batch stream marker
   * @param currentBatchInfo BatchInfo object containing bach description
   * @param crc Checksum value
   * @param startBatchIndex Index of first render item in the batch
   * @param endBatchIndex Index of last render item in the batch
   */
  void UpdateBatchList( BatchInfo& currentBatchInfo, uint32_t crc, uint32_t startBatchIndex, uint32_t endBatchIndex );

private:

  struct Impl* mImpl;

};

} // SceneGraph

} // Internal

} // Dali


#endif
