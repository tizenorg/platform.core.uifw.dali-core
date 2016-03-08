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
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
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
class Material;
class Geometry;

typedef Vector< RenderItem* > BatchRenerItemContainer;

/**
 * @brief The GeometryBatchInfo struct
 * Encapsulates batch parent, all render items included into the batch and
 * dynamic elements buffer to be used when rendering the item
 *
 * This class is responsible for:
 * - generating batches
 * - caching them
 * - generating renderer geometry instances based on incoming buffers
 *
 * Synchronization:
 * - Batches are being created on the Event thread. It means that we may need double buffer
 *   the BatchInfo cache. Do we need that?
 *
 * Snippet:
 *
 * Application side:
 * -------------------------------------
 * TableView tv = TableView::New();
 *
 * // create new batcher instance bound to the TableView, 'tv' becomes batch parent
 * GeometryBatcher batcher = GeometryBatcher::New( tv );
 *
 * -------------------------------------
 * Toolkit -> BatchImageRenderer
 *
 * ImageView imageView = ImageView::New(); // any image view, batchable
 *
 * // Find batcher, may be nullptr if no batch parent set
 * GeometryBatcher batcher = GeometryBatcher::FindBatcher( imageView, true );
 *
 * if (!batcher)
 * {
 * // use regular ImageRenderer
 * }
 * else
 * {
 *  // create geometry as normal
 *  Geometry geom = Geometry::New();
    geom.AddVertexBuffer();
    geom.SetIndexBuffer();
    geom.SetGeometryType();
    // add geometryto batcher
    batcher.UpdateGeometry( imageView, geom ); // it allows to modify existing geometry too marking actor as modified
 * }
 *
 *
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
typedef std::map<Render::PropertyBuffer*, Vector<char>* > BufferMapping;

struct BatchInfo
{
  BatchInfo(
      const Node* node = NULL,
      const Material* material = NULL,
      RenderGeometry* geometry = NULL
      ) :
      mBatchParent( node )
    , mMaterial( material )
    , mGeometry( geometry ) {}


  bool operator==( const BatchInfo& rhs ) const
  {
    return ( rhs.mBatchParent == mBatchParent )
        && (rhs.mMaterial == mMaterial )
        //&& (rhs.mGeometry == mGeometry )
        ;
  }

  bool operator!=( const BatchInfo& rhs ) const
  {
    return !(( rhs.mBatchParent == mBatchParent )
        && (rhs.mMaterial == mMaterial )
        //&& (rhs.mGeometry == mGeometry )
           );;
  }

  BatchInfo& operator=( const BatchInfo& rhs )
  {
    mBatchParent = rhs.mBatchParent;
    mMaterial = rhs.mMaterial;
    mGeometry = rhs.mGeometry;
    return *this;
  }

  operator bool() const
  {
    return ( mBatchParent && mMaterial/* && mGeometry */);
  }

  bool operator!() const
  {
    return !mBatchParent || !mMaterial/* || !mGeometry*/;
  }

  void Reset()
  {
    mBatchParent = NULL;
    mMaterial = NULL;
    mGeometry = NULL;
  }

  const Node*       mBatchParent;
  const Material*   mMaterial;
  RenderGeometry*   mGeometry; // not synced yet
  Render::PropertyBuffer*   mVertexBuffer; // not synced yet
  Render::PropertyBuffer*   mIndexBuffer; // not synced yet
  uint32_t          mCRC;
};

struct Marker
{
  Marker( BatchInfo i, size_t f, size_t t ) :
    info( i ), from( f ), to( t ) {}

  Marker( const Marker& marker )
  {
    info = marker.info;
    from = marker.from;
    to = marker.to;
  }

  BatchInfo info;
  size_t from;
  size_t to;
};

class GeometryBatcher
{
public:

  /**
   * @brief BuildBatches
   * @param items
   * @param count
   * @return total number of batches
   */
  size_t RebuildBatches( RenderItem **ppItems, size_t count,
                         const Matrix& viewMatrix,
                         unsigned int bufferIndex );
private:

  /**
   * @brief TransformVertices
   * Transforms vertices in the given array
   * @param data
   * @param componentSize
   * @param numVerts
   */
  void TransformVertices( void* dst, void* src, const Node* node, size_t componentSize, size_t numVerts, unsigned int bufferIndex );


  /**
   * @brief GetBatchedGeometry
   * @return
   */
  RenderGeometry* GetBatchedGeometry( Render::PropertyBuffer* vertexBuffer, Render::PropertyBuffer* indexBuffer );

private:

  /**
   * @brief UpdateGeometry
   * Updates batch geometry based on changes
   * @param batchInfo
   * @return
   */
  bool UpdateGeometry( BatchInfo* batchInfo );

  BatchInfoContainer mBatches;

  // need to map tranformed vertices onto source vertex buffer
  BufferMapping   mVertexBufferMapping;
  BufferMapping   mIndexBufferMapping;


  BatchInfo*      mBatchInfoCache;

};

} // SceneGraph

} // Internal

} // Dali


#endif
