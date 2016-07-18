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
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/common/math.h>
#include <dali/devel-api/common/crc32.h>
#include <string.h>
#include <stdio.h>
namespace
{
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

// TODO: find better way than using a singleton?
Dali::Internal::SceneGraph::GeometryBatcher* gInstance( NULL );


} //Unnamed namespace

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

struct VertexDescriptor
{
  VertexDescriptor()
    : mVertexComponentSize( 0 ),
      mVertexPositionType( Dali::Property::NONE ),
      mVertexFormat( NULL )
  {
  }

  unsigned int mVertexComponentSize; ///< Vertex component size in bytes
  Dali::Property::Type mVertexPositionType;  ///< Vertex position type ( may be Vector2, Vector3, Vector4 )
  Dali::Internal::Render::PropertyBuffer::Format* mVertexFormat; ///< Vertex format cloned from the very first batched item
};

struct BatchKey
{

  BatchKey() :
    mDirty( true ),
    mCRC( 0 ),
    mRendered( false )
  {

  }

  BatchKey( Node* node )
  {
    mKey.mBatchParent = node->GetBatchParent();

    Renderer* renderer = node->GetRendererAt( 0 );
    mKey.mShader = &renderer->GetShader();
    mKey.mTextureSet = renderer->GetTextures();
    mKey.mDepthIndex = renderer->GetDepthIndex();
    mCRC = 0;
    mDirty = true;
    mRendered = false ;
    mCRC = CalculateCRC32( &mKey, sizeof(struct Key) );
  }

  const Node* GetBatchParent()
  {
    return mKey.mBatchParent;
  }

  bool                  mDirty;
  uint32_t              mCRC;
  bool                  mRendered;

private:
  struct Key
  {
    const Node*           mBatchParent;
    const Shader*         mShader;
    const TextureSet*     mTextureSet;
    int                   mDepthIndex;
  } mKey;
};

struct BatchParent
{
  Node*           mBatchParent;
  Vector<Node*>   mBatchedChildren;
  bool            mNeedsUpdate;
};

struct Batch
{
  typedef Vector<unsigned short> IndexArray;
  BatchKey                mBatchKey;
  Render::Geometry*       mGeometry;
  IndexArray              mIndices; // index buffer per batch
  bool                    mDirty;
};

struct Impl
{
  Impl()
  {}

  std::vector<BatchParent>         mBatchParents; /// list of all batch parents
  std::vector<Batch>               mBatches;

  int GetBatchKeyIndex( const BatchKey& key )
  {
    for( unsigned int i = 0; i < mBatches.size(); ++i )
    {
      if( mBatches[i].mBatchKey.mCRC == key.mCRC )
      {
        return i;
      }
    }
    return -1;
  }

  GeometryBatcher*      mGeometryBatcher; ///< Associated GeometryBatcher object
  UpdateManager*        mUpdateManager;

};

GeometryBatcher::GeometryBatcher() :
  mImpl( NULL )
{
  DALI_ASSERT_ALWAYS( !gInstance && "There's already instance of GeometryBatcher created!");
  mImpl = new Impl();
  mImpl->mGeometryBatcher = this;
  gInstance = this;
}

void GeometryBatcher::SetUpdateManager( UpdateManager* updateManager )
{
  mImpl->mUpdateManager = updateManager;
}

GeometryBatcher::~GeometryBatcher()
{
  DALI_ASSERT_ALWAYS( (gInstance == this) && "Destroying wrong instance of the GeometryBatcher!");
  gInstance = NULL;
  delete mImpl;
}


void GeometryBatcher::CloneVertexFormat( const Render::Geometry* sourceGeometry, VertexDescriptor& vertexDescriptor )
{
  const Render::Geometry* geometry = sourceGeometry;
  const Render::PropertyBuffer::Format* format = NULL;

  format = geometry->GetPropertyBuffer( 0 )->GetFormat();

  Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
  Render::PropertyBuffer::Component& firstComponent = clonedVertexFormat->components[0];

  vertexDescriptor.mVertexPositionType = firstComponent.type;
  vertexDescriptor.mVertexComponentSize = clonedVertexFormat->size;
  vertexDescriptor.mVertexFormat = clonedVertexFormat;
}


void GeometryBatcher::DiscardDeadBatches( int updateBufferIndex )
{
  /*
  Vector<uint32_t>& currentDiscardList = mImpl->mDiscardList[updateBufferIndex];
  if( !currentDiscardList.Empty() )
  {
    const unsigned size = currentDiscardList.Size();
    for( unsigned i = 0; i < size; ++i )
    {
      BatchInfo* info = mImpl->FindBatchInfo( currentDiscardList[i] );
      if( !info )
      {
        continue;
      }

      // delete attached resources
      if( info->mVertexBuffer )
      {
        mImpl->mUpdateManager->RemovePropertyBuffer( info->mVertexBuffer );
      }
      if( info->mGeometry )
      {
        mImpl->mUpdateManager->RemoveGeometry( info->mGeometry );
      }

      if( info->mIndexBuffer )
      {
        delete info->mIndexBuffer;
      }

      // delete batch info object
      mImpl->EraseBatchInfo( currentDiscardList[i] );
    }
    currentDiscardList.Clear();
  }
  */
}

void GeometryBatcher::Update( BufferIndex bufferIndex, RenderInstructionContainer& instructions )
{
  UpdateNew( bufferIndex, instructions );
}

void GeometryBatcher::UpdateNew( BufferIndex bufferIndex, RenderInstructionContainer& instructions )
{
  if( !mImpl->mBatchParents.empty() )
  {

    std::vector<BatchParent>::iterator iter = mImpl->mBatchParents.begin();
    std::vector<BatchParent>::iterator end = mImpl->mBatchParents.end();

    for( ; iter != end; ++iter )
    {
      BatchParent& batchParentData = *iter;
      if( !batchParentData.mNeedsUpdate )
      {
        continue;
      }
      Node* batchParentNode = batchParentData.mBatchParent;

      size_t size = batchParentData.mBatchedChildren.Size();
      if( !size )
      {
        batchParentData.mNeedsUpdate = false;
        continue;
      }

      bool batchingFailed( false );

      VertexDescriptor vertexDescriptor;
      int batchKeyIndex( -1 );
      BatchKey oldKey;
      //vertexBufferDest.Clear();
      Vector<char>& vertexBufferDest = *(new Vector<char>());

      Render::PropertyBuffer* batchVertexBuffer = new Render::PropertyBuffer();

      size_t currentElementIndex = 0;
      for( size_t i = 0; i < size; ++i )
      {
        // this list should be sorted
        Node* node = batchParentData.mBatchedChildren[i];

        // get renderer
        const SceneGraph::Renderer* renderer = node->GetRendererAt( 0 );

        // geometry
        const Render::Geometry* geometry = &renderer->GetGeometry();

        BatchKey key( node );

        // format of first property buffer
        const Render::PropertyBuffer* vertexBuffer = geometry->GetPropertyBuffer( 0 );
        if( !vertexBuffer )
        {
          batchingFailed = true;
          break;
        }

        // ----------------------------------------------------------------------------
        if( oldKey.mCRC != key.mCRC )
        {
          oldKey.mCRC = key.mCRC;
          batchKeyIndex = mImpl->GetBatchKeyIndex( key );
          if( batchKeyIndex < 0 )
          {
            batchKeyIndex = mImpl->mBatches.size();

            // create new geometry
            Render::Geometry* newGeometry = new Render::Geometry();

            // describe new batch
            Batch batch;
            batch.mGeometry = newGeometry;
            batch.mBatchKey = key;
            batch.mDirty = true;

            mImpl->mBatches.push_back( batch );
            newGeometry->AddPropertyBuffer( batchVertexBuffer );
            mImpl->mUpdateManager->AddGeometry( newGeometry );
          }

          {
            if( !mImpl->mBatches[ batchKeyIndex ].mBatchKey.mDirty )
            {
              continue;
            }
          }
        }

        node->mBatchIndex = batchKeyIndex;

        if( !vertexDescriptor.mVertexFormat )
        {
          CloneVertexFormat( geometry, vertexDescriptor );
        }

        const uint32_t vertexBufferSize = vertexBuffer->GetDataSize();
        const char* vertexDataSource = &vertexBuffer->GetData()[ 0 ];

        uint32_t currentSize = vertexBufferDest.Size();
        vertexBufferDest.Resize( currentSize + vertexBufferSize );
        char* vertexDataDest = &vertexBufferDest[ currentSize ];

        // copy data as they are
        std::copy( vertexDataSource, vertexDataSource + vertexBufferSize, vertexDataDest );

        // transform node
        const Matrix& worldMatrix = node->GetWorldMatrix( bufferIndex );
        Matrix invWorldMatrix( batchParentNode->GetWorldMatrix( bufferIndex ) );
        invWorldMatrix.Invert();

        // vertex count
        const unsigned int sourceVertexCount = vertexBufferSize / vertexDescriptor.mVertexComponentSize;

        TransformVertexBufferData transformParameters;
        transformParameters.mDestinationPtr = vertexDataDest;
        transformParameters.mSourcePtr = vertexDataSource;
        transformParameters.mWorldMatrix = worldMatrix.AsFloat();
        transformParameters.mParentInvWorldMatrix = invWorldMatrix.AsFloat();
        transformParameters.mComponentSize = vertexDescriptor.mVertexComponentSize;
        transformParameters.mVertexCount = sourceVertexCount;
        transformParameters.mSize = node->GetSize( bufferIndex ).AsFloat();

        // Perform vertex transform based on the vertex format
        switch( vertexDescriptor.mVertexPositionType )
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

        // update index buffer
        Batch& batch = mImpl->mBatches[ batchKeyIndex ];
        uint32_t currentIndexOffset = batch.mIndices.Size();
        batch.mIndices.Resize( batch.mIndices.Size() + sourceVertexCount );
        for( size_t k = 0; k < sourceVertexCount; ++k )
        {
          size_t index = currentElementIndex + k;
          batch.mIndices[k + currentIndexOffset] = (unsigned short)index;
        }

        currentElementIndex += sourceVertexCount;
      }
      if( batchingFailed )
      {
        delete &vertexBufferDest;
        delete batchVertexBuffer;
        continue;
      }

      mImpl->mUpdateManager->AddPropertyBuffer( batchVertexBuffer );
      batchVertexBuffer->SetFormat( vertexDescriptor.mVertexFormat );
      batchVertexBuffer->SetData( &vertexBufferDest, vertexBufferDest.Size()/vertexDescriptor.mVertexComponentSize );

      mImpl->mBatches[ batchKeyIndex ].mBatchKey.mDirty = false;

      batchParentData.mNeedsUpdate = false;

      // update index buffers for all batches own by that batch parent
      for( size_t k = 0; k < mImpl->mBatches.size(); ++k )
      {
        Batch& batch = mImpl->mBatches[k];
        if( batch.mBatchKey.GetBatchParent() == batchParentData.mBatchParent )
        {
          batch.mGeometry->SetIndexBuffer( batch.mIndices );
        }
      }
    }

    PrepareToRender();
  }

}

void GeometryBatcher::DiscardBatch( Node* node, int updateBufferIndex )
{
  /*
  Impl::BatchList::Iterator iter = mImpl->mBatchList.Begin();
  Impl::BatchList::Iterator end = mImpl->mBatchList.End();
  for( ; iter != end; ++iter )
  {
    BatchInfo& info = (*iter);
    if( info.mBatchParent == node )
    {
      mImpl->mDiscardList[updateBufferIndex].PushBack( info.mCRC );
    }
  }
  */
}


GeometryBatcher* GeometryBatcher::Get()
{
  return gInstance;
}

void GeometryBatcher::AddBatchParent( Node* node )
{
  BatchParent batchParent;
  batchParent.mBatchParent = node;
  batchParent.mNeedsUpdate = true;
  batchParent.mBatchedChildren.Clear();

  mImpl->mBatchParents.push_back( batchParent );
}

void GeometryBatcher::ResetBatchParents()
{
  mImpl->mBatchParents.clear();
}

void GeometryBatcher::AddNode( Node* node )
{
  // look for batch parent
  Node* currentNode = node->GetParent();
  Node* batchParent = NULL;
  while( currentNode )
  {
    if( currentNode->mIsBatchParent )
    {
      batchParent = currentNode;
    }
    currentNode = currentNode->GetParent();
  }

  if( batchParent )
  {
    // find batch parent
    for( size_t i = 0; i < mImpl->mBatchParents.size(); ++i )
    {
      if( mImpl->mBatchParents[i].mBatchParent == batchParent )
      {
        mImpl->mBatchParents[i].mBatchedChildren.PushBack( node );
        node->SetBatchParent( batchParent );
        mImpl->mBatchParents[i].mNeedsUpdate = true;
        break;
      }
    }
  }
}

void GeometryBatcher::RemoveNode( Node* node )
{

}

Render::Geometry* GeometryBatcher::CreateBatchRenderGeometry( Render::PropertyBuffer::Format* format, Vector<char>* sourceData )
{
  Render::Geometry* geometry = new Render::Geometry();

  // create buffers
  Render::PropertyBuffer* vertexBuffer = new Render::PropertyBuffer();

  // assign formats using cloned values
  vertexBuffer->SetFormat( format );
  vertexBuffer->SetData( sourceData, sourceData->Size() );

  // add vertex buffer
  geometry->AddPropertyBuffer( vertexBuffer );

  // register objects with update manager
  mImpl->mUpdateManager->AddPropertyBuffer( vertexBuffer );
  mImpl->mUpdateManager->AddGeometry( geometry );

  return geometry;
}

bool GeometryBatcher::HasRendered( int batchIndex )
{
  return mImpl->mBatches[ batchIndex ].mBatchKey.mRendered;
}

void GeometryBatcher::SetRendered( int batchIndex )
{
  mImpl->mBatches[ batchIndex ].mBatchKey.mRendered = true;
}

int GeometryBatcher::GetBatchIndexFromNode( Node* node )
{
  BatchKey key( node );
  return mImpl->GetBatchKeyIndex( key );
}

Render::Geometry* GeometryBatcher::GetGeometry( int batchIndex )
{
  return mImpl->mBatches[ batchIndex ].mGeometry;
}

void GeometryBatcher::PrepareToRender()
{
  for( size_t i = 0; i < mImpl->mBatches.size(); ++i )
  {
    Batch& batch = mImpl->mBatches[i];
    batch.mBatchKey.mRendered = false;
  }
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
