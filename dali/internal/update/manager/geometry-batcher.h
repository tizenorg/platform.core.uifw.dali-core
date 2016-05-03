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
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <map>

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
class RenderItem;
class RenderGeometry;
class Node;
class Material;
class Geometry;
class RenderInstructionContainer;
class Shader;
class BatchInfo;



class GeometryBatcher
{
public:

  GeometryBatcher();
  ~GeometryBatcher();

  /**
   * @brief BuildBatches
   * @param items
   * @param count
   * @return total number of batches
   */
  size_t RebuildBatches( UpdateManager* updateManager, RenderItem **ppItems, size_t count,
                         const Matrix& viewMatrix,
                         unsigned int bufferIndex );

  // this shold run on the render thread in order to discard batches
  void DiscardBatches( UpdateManager* UpdateManager );

  // should be called by UpdateManager
  void Update(
      UpdateManager* updateManager,
      BufferIndex index,
      RenderInstructionContainer& instructions );

  // discards single batch
  void DiscardBatch( Node* node );

private:

  /**
   * @brief TransformVertices
   * Transforms vertices in the given array
   * @param data
   * @param componentSize
   * @param numVerts
   */
  void TransformVertices(
      void* dst,
      void* src,
      const Node* node,
      size_t componentSize,
      size_t numVerts,
      unsigned int bufferIndex );

private:

  struct Impl* mImpl;

};

} // SceneGraph

} // Internal

} // Dali


#endif
