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

#include "geometry-batcher.h"

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
#include <dali/devel-api/common/crc32.h>

namespace
{
const int DEFAULT_BATCH_MARKERS_POOL_CAPACITY( 128 );

inline void MulVecSize( Dali::Vector2& result, const Dali::Vector2& lhs, const Dali::Vector3& size )
{
  result.x = lhs.x * size.x;
  result.y = lhs.y * size.y;
}

inline void MulVecSize( Dali::Vector3& result, const Dali::Vector3& lhs, const Dali::Vector3& size )
{
  result.x = lhs.x * size.x;
  result.y = lhs.y * size.y;
  result.z = lhs.z * size.z;
}

inline void MulVecSize( Dali::Vector4& result, const Dali::Vector4& lhs, const Dali::Vector3& size )
{
  result.x = lhs.x * size.x;
  result.y = lhs.y * size.y;
  result.z = lhs.z * size.z;
  result.w = 1.0f;
}

inline void MulVecMat4( Dali::Vector4& result, const Dali::Matrix& lhs, const Dali::Vector4& rhs )
{
  const float *matrix = (const float*)&lhs;
  result.x = rhs.x * matrix[0] + rhs.y * matrix[4] + rhs.z * matrix[8]  +  rhs.w * matrix[12];
  result.y = rhs.x * matrix[1] + rhs.y * matrix[5] + rhs.z * matrix[9]  +  rhs.w * matrix[13];
  result.z = rhs.x * matrix[2] + rhs.y * matrix[6] + rhs.z * matrix[10] +  rhs.w * matrix[14];
  result.w = rhs.x * matrix[3] + rhs.y * matrix[7] + rhs.z * matrix[11] +  rhs.w * matrix[15];
}

inline void MulVecMat4( Dali::Vector3& result, const Dali::Matrix& lhs, const Dali::Vector3& rhs )
{
  const float *matrix = (const float*)&lhs;
  result.x = rhs.x * matrix[0] + rhs.y * matrix[4] + rhs.z * matrix[8]  +  matrix[12];
  result.y = rhs.x * matrix[1] + rhs.y * matrix[5] + rhs.z * matrix[9]  +  matrix[13];
  result.z = rhs.x * matrix[2] + rhs.y * matrix[6] + rhs.z * matrix[10] +  matrix[14];
}

inline void MulVecMat4( Dali::Vector2& result, const Dali::Matrix& lhs, const Dali::Vector2& rhs )
{
  const float *matrix = (const float*)&lhs;
  result.x = rhs.x * matrix[0] + rhs.y * matrix[4] + matrix[12];
  result.y = rhs.x * matrix[1] + rhs.y * matrix[5] + matrix[13];
}

/**
 * The TransformVertexBufferInfo struct
 * Must be filled before transforming vertices
 */
struct TransformVertexBufferData
{
  void*                 mDestinationPtr;       //< pointer to the destination vertex buffer
  void*                 mSourcePtr;            //< pointer to the source vertex buffer
  const Dali::Matrix&   mWorldMatrix;          //< model/world matrix of node being batched
  const Dali::Matrix&   mParentInvWorldMatrix; //< inv world matrix of batch parent
  unsigned              mComponentSize;        //< size of component, may be 0 if not interleaved packing used
  unsigned              mVertexCount;          //< number of vertices to process
  const Dali::Vector3   mPremulSize;           //< premultiply vertex size
};

/**
 * @brief function transforms vertices from 'source' and writes into 'destination'
 * @param[in] info filled TransformVertexBufferInfo structure
 */
template <typename PositionType >
void TransformVertexBuffer( const TransformVertexBufferData& info )
{
  PositionType* source       = reinterpret_cast<PositionType*>( info.mSourcePtr );
  PositionType* destination  = reinterpret_cast<PositionType*>( info.mDestinationPtr );

  Dali::Matrix transform( info.mWorldMatrix );
  Dali::Matrix::Multiply( transform, info.mWorldMatrix, info.mParentInvWorldMatrix );

  bool interleaved( true );
  if( !info.mComponentSize || info.mComponentSize == sizeof( PositionType ) )
  {
    interleaved = false;
  }

  if( interleaved )
  {
    for( unsigned i = 0; i < info.mVertexCount; ++i )
    {
      MulVecSize( *destination, *source, info.mPremulSize );
      MulVecMat4( *destination, transform, *destination );
      *(reinterpret_cast<char**>(&source)) += info.mComponentSize;
      *(reinterpret_cast<char**>(&destination)) += info.mComponentSize;
    }
  }
  else
  {
    for( unsigned i = 0; i < info.mVertexCount; ++i )
    {
      MulVecSize( *destination, *source, info.mPremulSize );
      MulVecMat4( *destination, transform, *destination );
      ++source;
      ++destination;
    }
  }
}

}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// ------------------------------------------------------------------------
// BatchInfo
// Helper structure. Descriptor for single batch.
// ------------------------------------------------------------------------
struct BatchInfo
{
  BatchInfo( Node* node = NULL,
             const TextureSet* textureSet = NULL,
             Dali::Internal::SceneGraph::Shader* shader = NULL,
             Render::Geometry* geometry = NULL )
  : mBatchParent( node ),
  mTextureSet( textureSet ),
  mShader( shader ),
  mGeometry( geometry ),
  mVertexBuffer( NULL ),
  mVertexComponentSize( 0 ),
  mVertexPositionType( Property::NONE ),
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
    return !(( rhs.mBatchParent == mBatchParent )
        && ( rhs.mTextureSet == mTextureSet )
        && ( rhs.mShader == mShader ) );
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

  Vector< unsigned short >        mIndexBuffer;

  Node*                           mBatchParent;
  const TextureSet*               mTextureSet;
  const Shader*                   mShader;
  Render::Geometry*               mGeometry;
  Render::PropertyBuffer*         mVertexBuffer;

  unsigned int                    mVertexComponentSize;
  Property::Type                  mVertexPositionType;
  Render::PropertyBuffer::Format* mVertexFormat;

  uint32_t                        mCRC;
  bool                            mDirtyFlag;
};

// ------------------------------------------------------------------------
// BatchStreamMarker
// Helper structure that binds part of stream of items with BatchInfo structure
// ------------------------------------------------------------------------
struct BatchStreamMarker
{
  BatchInfo* info;
  unsigned from;
  unsigned to;
};

// ------------------------------------------------------------------------
// Impl structure
// ------------------------------------------------------------------------
struct Impl
{
  typedef std::vector< BatchInfo* >           BatchList;
  typedef Vector< BatchStreamMarker >         MarkerArray;

  // fields
  BatchList             mBatchList; //< List of all the batches
  Vector<uint32_t>      mDiscardList; //< List of CRCs to be discarded
  MarkerArray           mMarkers; //< Array of markers used within a frame

  GeometryBatcher*      mGeometryBatcher; //< Associated GeometryBatcher object

  // ------------------------------------------------------------------------
  // Helper API, lookup and erase functions
  BatchInfo* FindBatchInfo( uint32_t crc )
  {
    BatchList::iterator it = mBatchList.begin();
    BatchList::iterator end = mBatchList.end();
    for( ; it != end; ++it )
    {
      if( (*it)->mCRC == crc )
      {
        return (*it);
      }
    }
    return NULL;
  }

  void EraseBatchInfo( uint32_t crc )
  {
    BatchList::iterator it = mBatchList.begin();
    BatchList::iterator end = mBatchList.end();
    for( ; it != end; ++it )
    {
      if( (*it)->mCRC == crc )
      {
        mBatchList.erase( it );
        return;
      }
    }
  }

};

// compare callback used for sorting
static bool BatchListCompare( const BatchInfo* lhs, const BatchInfo* rhs )
{
  return lhs->mCRC < rhs->mCRC;
}

// ------------------------------------------------------------------------
// helper struct describing common properties of single batch
// ------------------------------------------------------------------------

GeometryBatcher::GeometryBatcher()
{
  mImpl = new Impl();
  mImpl->mGeometryBatcher = this;
  mImpl->mMarkers.Reserve( DEFAULT_BATCH_MARKERS_POOL_CAPACITY );
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
    const Node* node = item->mNode;
    Node* batchParentNode = node->GetBatchParent();
    const SceneGraph::Renderer* nodeRenderer = node->GetRendererAt( 0 );

    bool batchable = nodeRenderer->IsBatchingEnabled();

    if( batchParentNode && batchable )
    {
      item->mSkipIfBatched = true;
    }

    BatchInfo info( batchParentNode, nodeRenderer->GetTextures(), &nodeRenderer->GetShader() );

    // ------------------------------------------------------------
    if( !batchable || !currentBatchInfo || currentBatchInfo != info ) // nobatch, or batch swapped
    {
      // push batch marker
      if( currentBatchInfo )
      {
        currentBatchInfo.mCRC = crc;
        BatchInfo* bi = mImpl->FindBatchInfo( crc );
        if( !bi )
        {
          bi = new BatchInfo( currentBatchInfo );
        }
        BatchStreamMarker marker = { bi, startBatchIndex, i };
        mImpl->mMarkers.PushBack( marker );
      }

      crc = 0;
      currentBatchInfo.Reset();
      if( batchParentNode && batchable ) // new batch
      {
        startBatchIndex = i;
        currentBatchInfo = info;
        crc = CalculateCRC32( 0, reinterpret_cast<const uint8_t**>( &node ), sizeof(void*) );
      }
    }
    else
    {
      crc = CalculateCRC32( crc, reinterpret_cast<const uint8_t**>( &node ), sizeof(void*) );
    }
  }

  // last batch remained
  if( currentBatchInfo )
  {
    currentBatchInfo.mCRC = crc;
    BatchInfo* bi = mImpl->FindBatchInfo( crc );
    if( !bi )
    {
      bi = new BatchInfo( currentBatchInfo );
    }
    BatchStreamMarker marker = { bi, startBatchIndex, i };
    mImpl->mMarkers.PushBack( marker );
  }

  // sort markers
  std::stable_sort( mImpl->mBatchList.begin(), mImpl->mBatchList.end(), BatchListCompare );
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

void GeometryBatcher::CreateGeometry( UpdateManager* updateManager, BatchStreamMarker& marker, RenderItem **itemsPtr, BatchInfo& info )
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
  info.mIndexBuffer.Resize( indexBufferSize );

  // add vertex buffer
  info.mGeometry->AddPropertyBuffer( vertexBuffer );
  info.mVertexBuffer = vertexBuffer;

  updateManager->AddPropertyBuffer( vertexBuffer );

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
  float* inVertexPtr = (float*)&inVertexBuffer[0];

  // update indices
  for( unsigned int j = 0; j < itemIndexBuffer->Size(); ++j )
  {
    info.mIndexBuffer.PushBack( (*itemIndexBuffer)[j] + indexIter );
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
    worldMatrix,
    invWorldMatrix,
    info.mVertexComponentSize,
    inVertexCount,
    (item->mNode->GetSize(bufferIndex))
  };

  // Perform vertex transform based on the vertex format
  switch( info.mVertexPositionType )
  {
    case Dali::Property::VECTOR2:
    {
      TransformVertexBuffer<Vector2>( transformParameters );
      break;
    }
    case Dali::Property::VECTOR3:
    {
      TransformVertexBuffer<Vector3>( transformParameters );
      break;
    }
    case Dali::Property::VECTOR4:
    {
      TransformVertexBuffer<Vector4>( transformParameters );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( true && "Incorrect vertex format! Use Vector2, Vector3 or Vector4 as position!" );
    }
  }
  vertexIter += inVertexBufferSize;
}

void GeometryBatcher::RebuildBatches( UpdateManager* updateManager, RenderItemContainer& container, unsigned int bufferIndex )
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
    BatchInfo& info = *marker.info;

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
      CreateGeometry( updateManager, marker, itemsPtr, info );
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
        item->mSkipIfBatched = true;
        item->mBatchRenderGeometry = NULL;
      }
      else
      {
        item->mSkipIfBatched = false;
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
      for( unsigned k = marker.from; k < marker.to; ++k )
      {
        RenderItem*                       item = itemsPtr[ k ];
        const Render::Geometry*           itemGeometry = item->mRenderer->GetGeometry();
        const Vector< unsigned short >*   itemIndexBuffer = itemGeometry->GetIndexBuffer();
        TransformItemVertices( item, info, itemIndexBuffer, indexIter, vertexIter, outVertexPtr, bufferIndex );
      }

      // update GPU buffers
      info.mVertexBuffer->UpdateData();
      info.mGeometry->SetIndexBuffer( info.mIndexBuffer );
    }
  }
}

void GeometryBatcher::DiscardDeadBatches( UpdateManager* updateManager )
{
  if( !mImpl->mDiscardList.Empty() )
  {
    const unsigned size = mImpl->mDiscardList.Size();
    for( unsigned i = 0; i < size; ++i )
    {
      BatchInfo* info = mImpl->mBatchList[ mImpl->mDiscardList[i] ];
      updateManager->RemovePropertyBuffer( info->mVertexBuffer );
      mImpl->EraseBatchInfo( mImpl->mDiscardList[i] );
      delete info;
    }
    mImpl->mDiscardList.Clear();
  }
}

void GeometryBatcher::Update( UpdateManager* updateManager, BufferIndex bufferIndex, RenderInstructionContainer& instructions )
{
  const unsigned int instructionCount = instructions.Count( bufferIndex );
  for( Impl::BatchList::iterator it = mImpl->mBatchList.begin(); it != mImpl->mBatchList.end(); ++it )
  {
    (*it)->mDirtyFlag = false;
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
        RebuildBatches( updateManager, list->GetContainer(), bufferIndex );
      }
    }
  }

  DiscardDeadBatches( updateManager );
}

void GeometryBatcher::DiscardBatch( Node* node )
{
  Impl::BatchList::iterator it = mImpl->mBatchList.begin();
  Impl::BatchList::iterator end = mImpl->mBatchList.end();
  for( ; it != end; ++it )
  {
    BatchInfo* info = (*it);
    if( info->mBatchParent == node )
    {
      mImpl->mDiscardList.PushBack( info->mCRC );
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
