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
class Node;
class RenderInstructionContainer;

class GeometryBatcher
{
public:

  GeometryBatcher();
  ~GeometryBatcher();


  // should be called by UpdateManager
  void Update(
      UpdateManager* updateManager,
      BufferIndex index,
      RenderInstructionContainer& instructions );

  /**
   * Marks all batches for deletion for which the node is a batch parent
   * @param[in] node batch parent node
   */
  void DiscardBatch( Node* node );

private:

  /**
   * Rebuild batches for set of render items
   * @param[in] updateManager pointer to the update manager
   * @param[in] ppItems pointer to the render item list
   * @param count
   * @return total number of batches
   */
  void RebuildBatches(  UpdateManager* updateManager,
                        RenderItemContainer& container,
                        unsigned int bufferIndex );

  /**
   * @brief Deletes all resources used by the dead batches ( buffers, helper objects )
   * @param[in] updateManager pointer to the update manager
   */
  void DiscardDeadBatches( UpdateManager* updateManager );

private:

  struct Impl* mImpl;

};

} // SceneGraph

} // Internal

} // Dali


#endif
