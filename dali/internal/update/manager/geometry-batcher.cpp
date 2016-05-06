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

// CLASS HEADER
#include <dali/internal/update/manager/geometry-batcher.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/common/math.h>
#include <dali/devel-api/common/crc32.h>

namespace
{

/**
 * @brief DEFAULT_BATCH_MARKERS_POOL_CAPACITY
 * Collection of markers is recreated every frame from scratch. To avoid
 * frequent heap allocation, all markers are taken from the pool and put back
 * when batching ends.
 *
 * Default capacity of the pool is based on the benchmark although if batching
 * goes beyound that, the pool will resize.
 */
const int DEFAULT_BATCH_MARKERS_POOL_CAPACITY( 128 );

/**
 * The TransformVertexBufferInfo struct
 * Must be filled before transforming vertices
 */
struct TransformVertexBufferData
{
  void*                         mDestinationPtr;       ///< pointer to the destination vertex buffer
  const void*                   mSourcePtr;            ///< pointer to the source vertex buffer
  const float*                  mWorldMatrix;          ///< model/world matrix of node being batched
  const float*                  mParentInvWorldMatrix; ///< inv world matrix of batch parent
  unsigned                      mComponentSize;        ///< size of component
  unsigned                      mVertexCount;          ///< number of vertices to process
  const float*                  mSize;                 ///< size of render item
};

/**
 * @brief function transforms vertices from 'source' and writes into 'destination'
 * @param[in,out] data Filled TransformVertexBufferInfo arguments structure
 */
template <typename PositionType >
void TransformVertexBuffer( TransformVertexBufferData& data )
{
  const PositionType* source       = reinterpret_cast<const PositionType*>( data.mSourcePtr );
  PositionType* destination  = reinterpret_cast<PositionType*>( data.mDestinationPtr );

  Dali::Internal::Mat4 transform;
  Dali::Internal::MultiplyMatrices( transform, data.mWorldMatrix, data.mParentInvWorldMatrix );
  bool interleaved( true );
  if( !data.mComponentSize || data.mComponentSize == sizeof( PositionType ) )
  {
    interleaved = false;
  }

  if( interleaved )
  {
    for( unsigned i = 0; i < data.mVertexCount; ++i )
    {
      Dali::Internal::MultiplyVectorBySize( *destination, *source, data.mSize );
      Dali::Internal::MultiplyVectorByMatrix4( *destination, transform, *destination );
      *(reinterpret_cast<const char**>(&source)) += data.mComponentSize;
      *(reinterpret_cast<char**>(&destination)) += data.mComponentSize;
    }
  }
  else
  {
    for( unsigned i = 0; i < data.mVertexCount; ++i )
    {
      Dali::Internal::MultiplyVectorBySize( *destination, *source, data.mSize );
      Dali::Internal::MultiplyVectorByMatrix4( *destination, transform, *destination );
      ++source;
      ++destination;
    }
  }
}

} //Unnamed namespace

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * @brief The BatchInfo struct
 * Helper structure that describes single batch.
 */
struct BatchInfo
{
  BatchInfo( const Node* node = NULL,
             const TextureSet* textureSet = NULL,
             const Dali::Internal::SceneGraph::Shader* shader = NULL,
             Render::Geometry* geometry = NULL )
  : mIndexBuffer( NULL ),
    mBatchParent( node ),
    mTextureSet( textureSet ),
    mShader( shader ),
    mGeometry( geometry ),
    mVertexBuffer( NULL ),
    mVertexComponentSize( 0 ),
    mVertexPositionType( Property::NONE ),
    mVertexFormat( NULL ),
    mCRC( 0 ),
    mDirtyFlag( false)
  {}

  bool operator==( const BatchInfo& rhs ) const
  {
    return ( rhs.mBatchParent == mBatchParent )
        && ( rhs.mTextureSet == mTextureSet )
        && ( rhs.mShader == mShader );
  }

  bool operator!=( const BatchInfo& rhs ) const
  {
    return ( rhs.mBatchParent != mBatchParent )
        ||  ( rhs.mTextureSet != mTextureSet )
        ||  ( rhs.mShader != mShader );
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

  Vector< unsigned short >*       mIndexBuffer;             ///< Content of the index buffer

  const Node*                     mBatchParent;             ///< Batch parent node
  const TextureSet*               mTextureSet;              ///< Texture set associated with the batch
  const Shader*                   mShader;                  ///< Shader used by batch
  Render::Geometry*               mGeometry;                ///< Geometry used by batch
  Render::PropertyBuffer*         mVertexBuffer;            ///< Pointer to the vertex buffer

  unsigned int                    mVertexComponentSize;     ///< Vertex component size in bytes
  Property::Type                  mVertexPositionType;      ///< Vertex position type ( may be Vector2, Vector3, Vector4 )
  Render::PropertyBuffer::Format* mVertexFormat;            ///< Vertex format cloned from the very first batched item

  uint32_t                        mCRC;                     ///< Calculated checksum value identifying the batch
  bool                            mDirtyFlag;               ///< True if any item of the batch is modified
};

// ------------------------------------------------------------------------
// BatchStreamMarker
// Helper structure that binds part of stream of items with BatchInfo structure
// ------------------------------------------------------------------------
struct BatchStreamMarker
{
  uint32_t batchInfoIndex;
  uint16_t from;
  uint16_t to;
};

/**
 * @brief The Impl struct
 * Impl structure of the geometry batcher
 */
struct Impl
{
  typedef Vector< BatchInfo >                BatchList;
  typedef Vector< BatchStreamMarker >        MarkerArray;

  // fields
  BatchList             mBatchList;       ///< List of all the batches
  Vector<uint32_t>      mDiscardList;     ///< List of CRCs to be discarded
  MarkerArray           mMarkers;         ///< Array of markers used within a frame

  GeometryBatcher*      mGeometryBatcher; ///< Associated GeometryBatcher object

  UpdateManager*        mUpdateManager;

  // ------------------------------------------------------------------------
  // Helper API, lookup and erase functions
  BatchInfo* FindBatchInfo( uint32_t crc )
  {
    BatchList::Iterator iter = mBatchList.Begin();
    BatchList::Iterator end = mBatchList.End();
    for( ; iter != end; ++iter )
    {
      if( (*iter).mCRC == crc )
      {
        return &(*iter);
      }
    }
    return NULL;
  }

  int PushBatchInfo( const BatchInfo& batchInfo )
  {
    int retval = mBatchList.Size();
    mBatchList.PushBack( batchInfo );
    return retval;
  }

  int FindBatchInfoIndex( uint32_t crc )
  {
    BatchList::Iterator iter = mBatchList.Begin();
    BatchList::Iterator end = mBatchList.End();
    for( ; iter != end; ++iter )
    {
      if( (*iter).mCRC == crc )
      {
        return iter - mBatchList.Begin();
      }
    }
    return -1;
  }

  void EraseBatchInfo( uint32_t crc )
  {
    BatchList::Iterator iter = mBatchList.Begin();
    BatchList::Iterator end = mBatchList.End();
    for( ; iter != end; ++iter )
    {
      if( (*iter).mCRC == crc )
      {
        mBatchList.Erase( iter );
        return;
      }
    }
  }

};

GeometryBatcher::GeometryBatcher()
{
  mImpl = new Impl();
  mImpl->mMarkers.Reserve( DEFAULT_BATCH_MARKERS_POOL_CAPACITY );
}

void GeometryBatcher::SetUpdateManager( UpdateManager* updateManager )
{
  mImpl->mUpdateManager = updateManager;
}

GeometryBatcher::~GeometryBatcher()
{
  delete mImpl;
}

void GeometryBatcher::GenerateBatchMarkers( RenderItemContainer& container )
{
  BatchInfo currentBatchInfo;
  unsigned startBatchIndex = 0;

  RenderItem **itemsPtr = static_cast<RenderItem**>( container.Begin() );
  unsigned count = container.Size();

  intptr_t crc = 0;

  // Reset pool of markers
  mImpl->mMarkers.Clear();
  unsigned i = 0;
  for( ; i < count; ++i )
  {
    RenderItem* item = itemsPtr[ i ];

    item->mBatched = false;
    item->mBatchRenderGeometry = NULL;

    const Node* node = item->mNode;
    const Node* batchParentNode = node->GetBatchParent();
    SceneGraph::Renderer* nodeRenderer = node->GetRendererAt( 0 );

    bool batchable = nodeRenderer->IsBatchingEnabled();

    if( batchParentNode && batchable )
    {
      item->mBatched = true;
    }

    BatchInfo info( batchParentNode, nodeRenderer->GetTextures(), &nodeRenderer->GetShader(), NULL );

    // Test if the current item shouldn't be batchable. If not, then
    // whatever has been collected until now is saved as new BatchInfo object.
    // If object should be batchable but it doesn't match current batching criteria,
    // it starts a new batch.
    if( !batchable || !currentBatchInfo || currentBatchInfo != info ) // no batch, or batch swapped
    {
      // push batch marker
      UpdateBatchList( currentBatchInfo, crc, startBatchIndex, i );

      crc = 0;
      currentBatchInfo.Reset();
      if( batchParentNode && batchable ) // new batch
      {
        startBatchIndex = i;
        currentBatchInfo = info;
        crc = CalculateCRC32( reinterpret_cast<const uint8_t**>( &node ), sizeof(void*), 0 );
      }
    }
    else
    {
      crc = CalculateCRC32( reinterpret_cast<const uint8_t**>( &node ), sizeof(void*), crc );
    }
  }

  // last batch remained
  UpdateBatchList( currentBatchInfo, crc, startBatchIndex, i );
}

bool GeometryBatcher::CloneVertexFormat( const Render::Geometry* sourceGeometry, BatchInfo& batchInfo )
{
  const Render::Geometry* geometry = sourceGeometry;
  const Render::PropertyBuffer::Format* format = NULL;
  if ( geometry->GetPropertyBuffer( 0 ) )
  {
    format = geometry->GetPropertyBuffer( 0 )->GetFormat();
  }

  if( !format )
  {
    return false;
  }

  Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
  Render::PropertyBuffer::Component& firstComponent = clonedVertexFormat->components[0];

  batchInfo.mVertexPositionType = firstComponent.type;
  batchInfo.mVertexComponentSize = clonedVertexFormat->size;
  batchInfo.mVertexFormat = clonedVertexFormat;

  return true;
}

void GeometryBatcher::CreateGeometry( BatchStreamMarker& marker, RenderItem **itemsPtr, BatchInfo& info )
{
  // -----------------------------------------------------------------------------------------
  // calculate size of buffers
  unsigned vertexBufferSize = 0;
  unsigned indexBufferSize = 0;
  for( unsigned k = marker.from; k < marker.to; ++k )
  {
    RenderItem* item = itemsPtr[ k ];
    const Render::PropertyBuffer* vertexBuffer = item->mRenderer->GetGeometry()->GetPropertyBuffer(0);
    const Vector< unsigned short >* indexBuffer = item->mRenderer->GetGeometry()->GetIndexBuffer();

    vertexBufferSize += vertexBuffer->GetSize();
    indexBufferSize += indexBuffer->Size();
  }

  // -----------------------------------------------------------------------------------------
  // initialise batch geometry for each new batch ( what happens if render item goes? should remove own batch too )
  info.mGeometry = new Render::Geometry();


  // create buffers
  Render::PropertyBuffer* vertexBuffer = new Render::PropertyBuffer();

  // assign formats using cloned values
  vertexBuffer->SetFormat( info.mVertexFormat );

  // set vertex data buffer
  Vector<char>* vertexData = new Vector<char>();
  vertexData->Resize( vertexBufferSize*info.mVertexComponentSize );
  vertexBuffer->SetData( vertexData, vertexBufferSize );

  // resize index buffer
  if( !info.mIndexBuffer )
  {
    info.mIndexBuffer = new Vector< unsigned short >();
  }
  info.mIndexBuffer->Resize( indexBufferSize );

  // add vertex buffer
  info.mGeometry->AddPropertyBuffer( vertexBuffer );
  info.mVertexBuffer = vertexBuffer;

  mImpl->mUpdateManager->AddPropertyBuffer( vertexBuffer );
  mImpl->mUpdateManager->AddGeometry( info.mGeometry );
  info.mDirtyFlag = true;
}

void GeometryBatcher::TransformItemVertices( RenderItem* item,
                                             BatchInfo &info,
                                             const Vector< unsigned short >* itemIndexBuffer,
                                             unsigned int& indexIter,
                                             unsigned int& vertexIter,
                                             char* outVertexPtr,
                                             unsigned int bufferIndex )
{
  // set input buffers
  const Vector<char>& inVertexBuffer = item->mRenderer->GetGeometry()->GetPropertyBuffer(0)->GetData();
  const float* inVertexPtr = reinterpret_cast<const float*>( &inVertexBuffer[0] );

  // update indices
  for( unsigned int j = 0; j < itemIndexBuffer->Size(); ++j )
  {
    info.mIndexBuffer->PushBack( (*itemIndexBuffer)[j] + indexIter );
  }
  indexIter += itemIndexBuffer->Size();

  const unsigned int inVertexBufferSize = inVertexBuffer.Size();
  const unsigned int inVertexCount = inVertexBufferSize / info.mVertexComponentSize;

  std::copy( inVertexBuffer.Begin(), inVertexBuffer.End(), &outVertexPtr[ vertexIter ] );

  const Matrix& worldMatrix = item->mNode->GetWorldMatrix( bufferIndex );
  Matrix invWorldMatrix( item->mBatchParentNode->GetWorldMatrix( bufferIndex ) );
  invWorldMatrix.Invert();

  // set input for vertex transform function
  TransformVertexBufferData transformParameters =
  {
    &outVertexPtr[ vertexIter ],
    inVertexPtr,
    worldMatrix.AsFloat(),
    invWorldMatrix.AsFloat(),
    info.mVertexComponentSize,
    inVertexCount,
    (item->mNode->GetSize(bufferIndex)).AsFloat(),
  };

  // Perform vertex transform based on the vertex format
  switch( info.mVertexPositionType )
  {
    case Dali::Property::VECTOR2:
    {
      TransformVertexBuffer<Vec2>( transformParameters );
      break;
    }
    case Dali::Property::VECTOR3:
    {
      TransformVertexBuffer<Vec3>( transformParameters );
      break;
    }
    case Dali::Property::VECTOR4:
    {
      TransformVertexBuffer<Vec4>( transformParameters );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( true && "Incorrect vertex format! Use Vector2, Vector3 or Vector4 as position!" );
    }
  }
  vertexIter += inVertexBufferSize;
}

void GeometryBatcher::RebuildBatches( RenderItemContainer& container, unsigned int bufferIndex )
{
  GenerateBatchMarkers( container );

  // return if there are no batches
  if ( mImpl->mMarkers.Empty() )
    return;

  RenderItem **itemsPtr = static_cast<RenderItem**>( container.Begin() );

  Impl::MarkerArray::Iterator iter = mImpl->mMarkers.Begin();
  Impl::MarkerArray::Iterator end = mImpl->mMarkers.End();

  for( ; iter != end; ++iter )
  {
    // collect batch common input data
    BatchStreamMarker& marker = *iter;
    BatchInfo& info = mImpl->mBatchList[ marker.batchInfoIndex ];

    RenderItem* firstItem = itemsPtr[ marker.from ];

    // clone vertex format, skip when format cannot be cloned
    const Render::Geometry* geometry = firstItem->mRenderer->GetGeometry();
    if( !CloneVertexFormat( geometry, info ) )
    {
      mImpl->EraseBatchInfo( info.mCRC );
      continue;
    }

    info.mDirtyFlag = false;

    // If there's no geometry set created yet, then make one
    if( !info.mGeometry )
    {
      CreateGeometry( marker, itemsPtr, info );
    }

    // second phase, recompute data and add indices into the index buffer
    unsigned int vertexIter = 0;
    unsigned int indexIter = 0;

    char* outVertexPtr = info.mVertexBuffer->GetDataTypedPtr<char>();

    for( unsigned k = marker.from; k < marker.to; ++k )
    {
      RenderItem* item = itemsPtr[ k ];
      // make only first render item renderable, all further items will be skipped during rendering
      if ( k > marker.from )
      {
        item->mBatched = true;
        item->mBatchRenderGeometry = NULL;
      }
      else
      {
        item->mBatched = false;
        item->mBatchRenderGeometry = info.mGeometry;
      }

      if( item->mNode->GetDirtyFlags() )
      {
        info.mDirtyFlag = true;
        break;
      }
    }

    // update only if batch has been modified
    if( info.mDirtyFlag )
    {
      for( uint32_t k = marker.from; k < marker.to; ++k )
      {
        RenderItem*                       item = itemsPtr[ k ];
        const Render::Geometry*           itemGeometry = item->mRenderer->GetGeometry();
        const Vector< unsigned short >*   itemIndexBuffer = itemGeometry->GetIndexBuffer();
        TransformItemVertices( item, info, itemIndexBuffer, indexIter, vertexIter, outVertexPtr, bufferIndex );
      }

      // update GPU buffers
      info.mVertexBuffer->UpdateData();
      info.mGeometry->SetIndexBuffer( *info.mIndexBuffer );
    }
  }
}

void GeometryBatcher::DiscardDeadBatches()
{
  if( !mImpl->mDiscardList.Empty() )
  {
    const unsigned size = mImpl->mDiscardList.Size();
    for( unsigned i = 0; i < size; ++i )
    {
      BatchInfo* info = mImpl->FindBatchInfo( mImpl->mDiscardList[i] );
      if( !info )
      {
        continue;
      }
      // delete attached resources
      mImpl->mUpdateManager->RemovePropertyBuffer( info->mVertexBuffer );
      mImpl->mUpdateManager->RemoveGeometry( info->mGeometry );

      delete info->mIndexBuffer;

      // delete batch info object
      mImpl->EraseBatchInfo( mImpl->mDiscardList[i] );
    }
    mImpl->mDiscardList.Clear();
  }
}

void GeometryBatcher::Update( BufferIndex bufferIndex, RenderInstructionContainer& instructions )
{
  const unsigned int instructionCount = instructions.Count( bufferIndex );
  for( Impl::BatchList::Iterator iter = mImpl->mBatchList.Begin(); iter != mImpl->mBatchList.End(); ++iter )
  {
    (*iter).mDirtyFlag = false;
  }
  for( unsigned i = 0; i < instructionCount; ++i )
  {
    RenderInstruction& instruction = instructions.At( bufferIndex, i );
    unsigned renderListCount = instruction.RenderListCount();
    if( renderListCount )
    {
      for( unsigned j = 0; j < renderListCount; ++j )
      {
        RenderList* list = const_cast<RenderList*>(instruction.GetRenderList( j ));
        RebuildBatches( list->GetContainer(), bufferIndex );
      }
    }
  }
}

void GeometryBatcher::DiscardBatch( Node* node )
{
  Impl::BatchList::Iterator iter = mImpl->mBatchList.Begin();
  Impl::BatchList::Iterator end = mImpl->mBatchList.End();
  for( ; iter != end; ++iter )
  {
    BatchInfo& info = (*iter);
    if( info.mBatchParent == node )
    {
      mImpl->mDiscardList.PushBack( info.mCRC );
    }
  }
}

void GeometryBatcher::UpdateBatchList(
    BatchInfo& currentBatchInfo,
    uint32_t crc,
    uint32_t startBatchIndex,
    uint32_t endBatchIndex )
{
  if( currentBatchInfo )
  {
    currentBatchInfo.mCRC = crc;
    int batchInfoIndex = mImpl->FindBatchInfoIndex( crc );
    if( batchInfoIndex < 0 )
    {
      batchInfoIndex = mImpl->PushBatchInfo( currentBatchInfo );
    }
    BatchStreamMarker marker = {
      static_cast<uint32_t>( batchInfoIndex ),
      static_cast<uint16_t>( startBatchIndex ),
      static_cast<uint16_t>( endBatchIndex )
    };
    mImpl->mMarkers.PushBack( marker );
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
