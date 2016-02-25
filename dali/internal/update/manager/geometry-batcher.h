#pragma once
#ifndef __DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_BATCHER_H__

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
 *
 */

#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/render/renderers/render-property-buffer.h>

#include <map>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class RenderItem;
class RenderGeometry;
class Node;

typedef Vector< RenderItem* > BatchRenerItemContainer;

/**
 * @brief The GeometryBatchInfo struct
 * Encapsulates batch parent, all render items included into the batch and
 * dynamic elements buffer to be used when rendering the item
 *
 */
struct GeometryBatchInfo
{
  uint32_t                mBatchUUID; // for now just computed with simple CRC
  Node*                   mBatchParent;
  BatchRenerItemContainer mBatchedItems; // batched items
  Render::PropertyBuffer* mElementsBuffer;
};

typedef Vector< GeometryBatchInfo* > BatchInfoContainer;
typedef std::map<Render::PropertyBuffer*, Render::PropertyBuffer*> BufferMapping;

class GeometryBatcher
{
public:

  /**
   * @brief BuildBatches
   * @param items
   * @param count
   * @return total number of batches
   */
  size_t RebuildBatches( RenderItem **ppItems, size_t count );
private:

  /**
   * @brief TransformVertices
   * Transforms vertices in the given array
   * @param data
   * @param componentSize
   * @param numVerts
   */
  void TransformVertices( void* dst, void* src, Node* node, size_t componentSize, size_t numVerts );

private:

  BatchInfoContainer mBatches;

  // need to map tranformed vertices onto source vertex buffer
  BufferMapping mVertexBufferMapping;


};

} // SceneGraph

} // Internal

} // Dali


#endif
