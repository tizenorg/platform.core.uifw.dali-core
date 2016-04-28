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
class SceneController;
class RenderInstructionContainer;
class Shader;

typedef Vector< RenderItem* >       BatchRenerItemContainer;
typedef Vector< unsigned short >    IndexBuffer;

/**
 * @brief The GeometryBatchInfo struct
 *
 */
struct BatchInfo
{
  BatchInfo(
      const Node* node = NULL,
      const TextureSet* textureSet = NULL,
      Dali::Internal::SceneGraph::Shader* shader = NULL,
      Render::Geometry* geometry = NULL
      ) :
      mBatchParent( node )
    , mTextureSet( textureSet )
    , mShader( shader )
    , mGeometry( geometry )
    , mVertexBuffer( NULL )
    , mCRC( 0 )
    , mTouched( false)
  {}

  bool operator==( const BatchInfo& rhs ) const
  {
    return ( rhs.mBatchParent == mBatchParent )
        && ( rhs.mTextureSet == mTextureSet )
        && ( rhs.mShader == mShader )
        ;
  }

  bool operator!=( const BatchInfo& rhs ) const
  {
    return !(( rhs.mBatchParent == mBatchParent )
        && ( rhs.mTextureSet == mTextureSet )
        && ( rhs.mShader == mShader )
        );;
  }

  BatchInfo& operator=( const BatchInfo& rhs )
  {
    mBatchParent = rhs.mBatchParent;
    mTextureSet = rhs.mTextureSet;
    mGeometry = rhs.mGeometry;
    mShader = rhs.mShader;
    mVertexBuffer = rhs.mVertexBuffer;
    mIndexBuffer = rhs.mIndexBuffer;
    mCRC = rhs.mCRC;
    return *this;
  }

  operator bool() const
  {
    return ( mBatchParent && mTextureSet && mShader );
  }

  bool operator!() const
  {
    return !mBatchParent || !mTextureSet || !mShader;
  }

  void Reset()
  {
    mBatchParent = NULL;
    mTextureSet = NULL;
    mShader = NULL;
    mGeometry = NULL;
  }

  IndexBuffer                   mIndexBuffer;

  const Node*                   mBatchParent;
  const TextureSet*             mTextureSet;
  const Dali::Internal::SceneGraph::Shader*                 mShader;
  Render::Geometry*             mGeometry; // not synced yet
  Render::PropertyBuffer*       mVertexBuffer; // not synced yet

  size_t                        mVertexComponentSize;
  Property::Type                mVertexPositionType;

  uint32_t                      mCRC;
  bool                          mTouched;
};

struct BatchStreamMarker
{
  BatchStreamMarker( BatchInfo& i, size_t f, size_t t ) :
    info( i ), from( f ), to( t ) {}

  BatchStreamMarker( const BatchStreamMarker& marker ) :
    info( marker.info ),
    from( marker.from ),
    to( marker.to )
  {}

  BatchStreamMarker& operator=( const BatchStreamMarker& marker )
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
  size_t RebuildBatches( UpdateManager* updateManager, RenderItem **ppItems, size_t count,
                         const Matrix& viewMatrix,
                         unsigned int bufferIndex );

  // this shold run on the render thread in order to discard batches
  void   DiscardBatches( UpdateManager* UpdateManager );

  // should be called by UpdateManager
  void   Update( UpdateManager* updateManager, BufferIndex index, RenderInstructionContainer& instructions );

  void DiscardBatch( Node* node );
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




  /**
   * @brief CreateBuffer
   * creates new or reuses property buffer
   * @param size
   * @return
   */
  Render::PropertyBuffer*   CreateBuffer( size_t size );

  /**
   * @brief ReleaseBuffer
   * releases property buffer
   * @param buffer
   */
  void                      ReleaseBuffer( Render::PropertyBuffer* buffer );

  void                      GenerateMarkers( RenderItem** ppItems, size_t count, BufferIndex bufferIndex );


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
  std::vector<uint32_t> mDiscardList;

  SceneController*  mSceneController;
  UpdateManager*    mUpdateManager;


  typedef std::vector< BatchStreamMarker > MarkerArray;
  MarkerArray mMarkers;


};

} // SceneGraph

} // Internal

} // Dali


#endif
