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

/**
 * @brief The VertexDescriptor struct
 * Holds details of vertex format used for batching
 */
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
    mRendered = false ;
    mCRC = CalculateCRC32( &mKey, sizeof(struct KeyData) );
  }

  const Node* GetBatchParent()
  {
    return mKey.mBatchParent;
  }

  uint32_t              mCRC;
  bool                  mRendered;

private:
  struct KeyData
  {
    const Node*           mBatchParent;
    const Shader*         mShader;
    const TextureSet*     mTextureSet;
    int                   mDepthIndex;
  } mKey;
};

/**
 * @brief The BatchParent struct
 * Stores list of children of single batch parent
 */
struct BatchParent
{
  Node*                   mBatchParent;
  Vector<Node*>           mBatchedChildren;
  Render::PropertyBuffer* mVertexBuffer;
  bool                    mNeedsUpdate;
};

/**
 * @brief The Batch struct
 * Stores details of single batch
 */
struct Batch
{
  Batch()
   : mGeometry( NULL ),
     mDirty( true )
  {}

  typedef Vector<unsigned short> IndexArray;
  BatchKey                mBatchKey; /// Unique batch key
  IndexArray              mIndices; /// index buffer per batch
  Render::Geometry*       mGeometry; /// Batch geometry
  bool                    mDirty; /// 'dirty' flag per batch
};

struct Impl
{
  Impl()
  {}

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

  std::vector<BatchParent>          mBatchParents; /// non-trivial type, hence std::vector
  std::vector<Batch>                mBatches; /// non-trivial type, hence std::vector
  Vector<unsigned int>              mDiscardList; /// List of batches to be discarded
  GeometryBatcher*                  mGeometryBatcher; /// Associated GeometryBatcher object
  UpdateManager*                    mUpdateManager;
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
  if( !format )
    return;

  Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
  Render::PropertyBuffer::Component& firstComponent = clonedVertexFormat->components[0];

  vertexDescriptor.mVertexPositionType = firstComponent.type;
  vertexDescriptor.mVertexComponentSize = clonedVertexFormat->size;
  vertexDescriptor.mVertexFormat = clonedVertexFormat;
}


void GeometryBatcher::DiscardDeadBatches( int updateBufferIndex )
{
  size_t size = mImpl->mDiscardList.Size();
  //int indexAdjust = 0;
  for( size_t i = 0; i < size; ++i )
  {
    //mImpl->mBatches.erase( mImpl->mDiscardList[i] -  )
  }
}

void GeometryBatcher::Update( BufferIndex bufferIndex )
{
  if( !mImpl->mBatchParents.empty() )
  {
    //printf("batches: %d\n", (int)mImpl->mBatches.size());
    //fflush(stdout);
    std::vector<BatchParent>::iterator iter = mImpl->mBatchParents.begin();
    std::vector<BatchParent>::iterator end = mImpl->mBatchParents.end();

    // for each Batch Parent
    for( ; iter != end; ++iter )
    {
      BatchParent& batchParentData = *iter;
      Node* batchParentNode = batchParentData.mBatchParent;

      // Skip update if batch parent doesn't need it
      if( !batchParentData.mNeedsUpdate )
      {
        continue;
      }

      // Skip if batch parent doesn't have batched children
      size_t size = batchParentData.mBatchedChildren.Size();
      if( !size )
      {
        batchParentData.mNeedsUpdate = false;
        continue;
      }

      bool  batchingFailed( false );
      int   batchKeyIndex( -1 );

      BatchKey oldKey;
      VertexDescriptor vertexDescriptor;

      // Destination vertex buffer per batch parent
      Vector<char>& vertexBufferDest = *(new Vector<char>());
      Render::PropertyBuffer* batchVertexBuffer = new Render::PropertyBuffer();
      puts("New vertex buffer");
      size_t currentElementIndex = 0;

      Matrix invWorldMatrix( batchParentNode->GetWorldMatrix( bufferIndex ) );
      invWorldMatrix.Invert();

      // For each batched child of this batch parent...
      for( size_t i = 0; i < size; ++i )
      {
        Node* node = batchParentData.mBatchedChildren[i];

        // Renderer
        const SceneGraph::Renderer* renderer = node->GetRendererAt( 0 );

        // GFometry
        const Render::Geometry* geometry = &renderer->GetGeometry();

        // Generate batch key
        BatchKey key( node );

        // format of first property buffer
        const Render::PropertyBuffer* vertexBuffer = geometry->GetPropertyBuffer( 0 );

        // Geometry of the node may not be ready, in that case we discard whole batch
        if( !vertexBuffer )
        {
          batchingFailed = true;
          break;
        }

        CloneVertexFormat( geometry, vertexDescriptor );
        if( !vertexDescriptor.mVertexFormat )
        {
          batchingFailed = true;
          break;
        }

        // Instantiate new batch
        if( oldKey.mCRC != key.mCRC )
        {
          oldKey.mCRC = key.mCRC;
          batchKeyIndex = mImpl->GetBatchKeyIndex( key );
          if( batchKeyIndex < 0 )
          {
            batchKeyIndex = mImpl->mBatches.size();

            // Create new batch geometry
            Render::Geometry* newGeometry = new Render::Geometry();
            puts("New geometry");
            Batch batch;
            batch.mGeometry = newGeometry;
            batch.mBatchKey = key;
            batch.mDirty = true;

            mImpl->mBatches.push_back( batch );

            // Vertex buffer may be set before it's filled with data
            newGeometry->AddPropertyBuffer( batchVertexBuffer );

            // Register geometry with update manager
            mImpl->mUpdateManager->AddGeometry( newGeometry );
          }
        }
        fflush(stdout);
        // Tell node which batch it belongs to
        node->mBatchIndex = batchKeyIndex;

        if( !mImpl->mBatches[ batchKeyIndex ].mDirty )
        {
          continue;
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

      // Add shared property buffer
      mImpl->mUpdateManager->AddPropertyBuffer( batchVertexBuffer );
      batchVertexBuffer->SetFormat( vertexDescriptor.mVertexFormat );
      batchVertexBuffer->SetData( &vertexBufferDest, vertexBufferDest.Size()/vertexDescriptor.mVertexComponentSize );

      batchParentData.mNeedsUpdate = false;
      batchParentData.mVertexBuffer = batchVertexBuffer;

      // Update index buffers for all batches own by that batch parent
      for( size_t k = 0; k < mImpl->mBatches.size(); ++k )
      {
        Batch& batch = mImpl->mBatches[k];
        if( batch.mBatchKey.GetBatchParent() == batchParentData.mBatchParent )
        {
          batch.mGeometry->SetIndexBuffer( batch.mIndices );
        }
        batch.mDirty = false;
      }
    }

    // mark all batches as not rendered yet
    PrepareToRender();
  }
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

void GeometryBatcher::RemoveBatchParent( Node* node )
{
  for( size_t i = 0; i < mImpl->mBatchParents.size(); ++i )
  {
    BatchParent& batchParent = mImpl->mBatchParents[i];
    if( batchParent.mBatchParent == node )
    {
      // reset all children and mark all batches to be discarded
      for( size_t j = 0; j < mImpl->mBatches.size(); ++j )
      {
        Batch& batch = mImpl->mBatches[j];
        if( batchParent.mBatchParent == batch.mBatchKey.GetBatchParent() )
        {
          AddToDiscardList( j );
        }
      }

      // remove batch parent
      mImpl->mBatchParents.erase( mImpl->mBatchParents.begin()+i );
      return;
    }
  }
}

void GeometryBatcher::AddToDiscardList( int batchIndex )
{
  mImpl->mDiscardList.PushBack( batchIndex );
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
  // delete all batches own by the batch parent of node
  for( size_t i = 0; i < mImpl->mBatches.size(); ++i )
  {
    Batch& batch = mImpl->mBatches[i];
    if( batch.mBatchKey.GetBatchParent() == node->GetBatchParent() )
    {
      mImpl->mUpdateManager->RemoveGeometry( batch.mGeometry );
      mImpl->mBatches.erase( mImpl->mBatches.begin()+i );
      i--;
    }
  }

  // tell batch parent that it's been modified
  for( size_t i = 0; i < mImpl->mBatchParents.size(); ++i )
  {
    BatchParent& batchParentData = mImpl->mBatchParents[i];
    if( batchParentData.mBatchParent == node->GetBatchParent() )
    {
      // find node in batched children
      for( size_t k = 0; k < batchParentData.mBatchedChildren.Size(); ++k )
      {
        if( batchParentData.mBatchedChildren[k] == node )
        {
          batchParentData.mBatchedChildren.Erase( batchParentData.mBatchedChildren.Begin()+k );
          break;
        }
      }
      batchParentData.mNeedsUpdate = true;
      if( batchParentData.mVertexBuffer )
      {
        mImpl->mUpdateManager->RemovePropertyBuffer( batchParentData.mVertexBuffer );
        batchParentData.mVertexBuffer = NULL;
      }
    }
  }
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
