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
  void UpdateNew( BufferIndex index, RenderInstructionContainer& instructions );
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

  /**
   * Marks all batches as not rendered yet. First renderitem refering to the batch
   * will mark it as rendered.
   */
  void PrepareToRender();

private:

  /**
   * @brief Clones vertex format of source geometry and stores generated data in the batchInfo object
   * @param[in] sourceGeometry Geometry of the very first batched item
   * @param[out] batchInfo Batch info object used to store cloned vertex format
   */
  void CloneVertexFormat( const Render::Geometry* sourceGeometry, struct VertexDescriptor& batchInfo );

public:
  static GeometryBatcher* Get();

  void AddBatchParent( Node* node );
  void ResetBatchParents();
  void AddNode( Node* node );
  void RemoveNode( Node* node );

  Render::Geometry* CreateBatchRenderGeometry( Render::PropertyBuffer::Format* format, Dali::Vector<char>* sourceData );

  // gets batch geometry for particular node ( and its renderer[0] )
  Render::Geometry* GetGeometry( int batchIndex );

  int GetBatchIndexFromNode( Node* node );
  bool HasRendered( int batchIndex );
  void SetRendered( int batchIndex );

private:

  struct Impl* mImpl;

};

} // SceneGraph

} // Internal

} // Dali


#endif
