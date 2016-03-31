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
class SceneController;
class RenderInstructionContainer;

typedef Vector< RenderItem* > BatchRenerItemContainer;

/**
 * @brief The GeometryBatchInfo struct
 *
 *
 *
 *
 */
struct BatchInfo
{
  BatchInfo(
      const Node* node = NULL,
      const Material* material = NULL,
      RenderGeometry* geometry = NULL
      ) :
      mBatchParent( node )
    , mMaterial( material )
    , mGeometry( geometry )
    , mVertexBuffer( NULL )
    , mIndexBuffer( NULL )
    , mCRC( 0 )
    , mTouched( false)
  {}

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
    mVertexBuffer = rhs.mVertexBuffer;
    mIndexBuffer = rhs.mIndexBuffer;
    mCRC = rhs.mCRC;
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

  const Node*                   mBatchParent;
  const Material*               mMaterial;
  RenderGeometry*               mGeometry; // not synced yet
  Render::PropertyBuffer*       mVertexBuffer; // not synced yet
  Render::PropertyBuffer*       mIndexBuffer; // not synced yet
  uint32_t                      mCRC;
  bool                          mTouched;
};

struct Marker
{
  Marker( BatchInfo& i, size_t f, size_t t ) :
    info( i ), from( f ), to( t ) {}

  Marker( const Marker& marker ) :
    info( marker.info ),
    from( marker.from ),
    to( marker.to )
  {}

  Marker& operator=( const Marker& marker )
  {
    info = marker.info;
    from = marker.from;
    to = marker.to;
    return *this;
  }

  BatchInfo& info;
  size_t from;
  size_t to;
};


class GeometryBatcher
{
public:

  GeometryBatcher(){}

  void SetSceneController( SceneController& sceneController )
  {
    mSceneController = &sceneController;
  }

  /**
   * @brief BuildBatches
   * @param items
   * @param count
   * @return total number of batches
   */
  size_t RebuildBatches( RenderItem **ppItems, size_t count,
                         const Matrix& viewMatrix,
                         unsigned int bufferIndex );

  // this shold run on the render thread in order to discard batches
  void   DiscardBatches();

  // should be called by UpdateManager
  void   Update( BufferIndex index, RenderInstructionContainer& instructions );

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

  typedef std::map<uint32_t, BatchInfo*> BatchMap;
  BatchMap mBatchMap;

  SceneController*  mSceneController;
};

} // SceneGraph

} // Internal

} // Dali


#endif
