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

// helper macros to deal with handles
#define BATCH_LOCAL_INDEX(x) (x&0xFFFF)
#define BATCH_PARENT_INDEX(x) ((x>>16)&0xFFFF)

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
  BatchKey()
  : mBatchParent( NULL ),
    mShader( NULL ),
    mTextureSet( NULL ),
    mDepthIndex( 0 ),
    mRendered( false )
  {
  }

  ~BatchKey()
  {

  }

  BatchKey( Node* node )
  {
    mBatchParent = node->GetBatchParent();
    Renderer* renderer = node->GetRendererAt( 0 );
    mShader = &renderer->GetShader();
    mTextureSet = renderer->GetTextures();
    mDepthIndex = renderer->GetDepthIndex();
    mRendered = false ;
  }

  inline bool operator==( const BatchKey& key )
  {
    return mBatchParent == key.mBatchParent && mShader == key.mShader && mTextureSet == key.mTextureSet && mDepthIndex == key.mDepthIndex;
  }

  inline bool operator!=( const BatchKey& key )
  {
    return !( *this == key );
  }

  const Node*           mBatchParent;
  const Shader*         mShader;
  const TextureSet*     mTextureSet;
  int                   mDepthIndex;
  bool                  mRendered;
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

typedef std::vector<Batch> BatchList;
/**
 * @brief The BatchParent struct
 * Stores list of children of single batch parent
 */
struct BatchParent
{
  Node*                   mBatchParent;
  Vector<Node*>           mBatchedChildren;
  BatchList               mBatches; /// non-trivial type, hence std::vector
  Render::PropertyBuffer* mVertexBuffer;
  bool                    mNeedsUpdate;
};

} // SceneGraph

} // Internal

namespace Internal
{
namespace SceneGraph
{
// --------------------------------------------------------------------------------------
/*
struct BatchList
{
  BatchList()
    : firstFreeIndex( -1 )
  {
  }

  ~BatchList()
  {

  }

  void Push( const Batch& batch )
  {
    batch.mNextFreeIndex = -1;
    if( firstFreeIndex > -1 )
    {
      int nextFreeIndex = data[firstFreeIndex].mNextFreeIndex;
      data[firstFreeIndex] = batch;
      firstFreeIndex = nextFreeIndex;
      return;
    }
    data.push_back( batch );
  }

  void Remove( unsigned int index )
  {
    if( firstFreeIndex > -1 )
    {
      data[index].mNextFreeIndex = firstFreeIndex;
    }
    firstFreeIndex = index;
  }

  void Remove( const Batch& batch )
  {
    for( size_t i = 0; i < data.size(); ++i )
    {
      if ( data[i].mBatchKey == batch.mBatchKey )
      {
        Remove( i );
        return;
      }
    }
  }

  Batch& operator[]( size_t index )
  {
    return data[index];
  }

  size_t Size()
  {
    return data.size();
  }

  std::vector<Batch>    data;
  int                   firstFreeIndex;
};
*/
struct Impl
{
  Impl()
  {}

  int GetBatchKeyIndex( const BatchKey& key )
  {
    for( size_t i = 0; i < mBatchParents.size(); ++i )
    {
      BatchParent& batchParent = mBatchParents[i];
      for( size_t j = 0; j < batchParent.mBatches.size(); ++j )
      {
        if( batchParent.mBatches[j].mBatchKey == key )
        {
          return ((i&0xFFFF)<<16)|(j&0xFFFF);
        }
      }
    }
    return -1;
  }


  std::vector<BatchParent>          mBatchParents; /// non-trivial type, hence std::vector
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

void GeometryBatcher::Update( BufferIndex bufferIndex )
{
  if( !mImpl->mBatchParents.empty() )
  {
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
      uint32_t batchKeyIndex( BATCH_NULL_HANDLE );

      BatchKey oldKey;
      VertexDescriptor vertexDescriptor;

      // Destination vertex buffer per batch parent
      Vector<char>& vertexBufferDest = *(new Vector<char>());
      Render::PropertyBuffer* batchVertexBuffer = new Render::PropertyBuffer();

      size_t currentElementIndex = 0;

      Matrix invWorldMatrix( batchParentNode->GetWorldMatrix( bufferIndex ) );
      invWorldMatrix.Invert();

      // For each batched child of this batch parent...
      for( size_t i = 0; i < size; ++i )
      {
        Node* node = batchParentData.mBatchedChildren[i];

        // Renderer
        if( !node->GetRendererCount() )
        {
          continue;
        }

        const SceneGraph::Renderer* renderer = node->GetRendererAt( 0 );

        // Geometry
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

        if( !vertexDescriptor.mVertexFormat )
        {
          CloneVertexFormat( geometry, vertexDescriptor );
        }
        if( !vertexDescriptor.mVertexFormat )
        {
          batchingFailed = true;
          break;
        }

        // Instantiate new batch
        if( oldKey != key )
        {
          oldKey = key;
          batchKeyIndex = mImpl->GetBatchKeyIndex( key );

          if( batchKeyIndex == BATCH_NULL_HANDLE )
          {
            // Create new batch geometry
            Render::Geometry* newGeometry = new Render::Geometry();

            Batch batch;
            batch.mGeometry = newGeometry;
            batch.mBatchKey = key;
            batch.mDirty = true;

            // push new batch
            batchParentData.mBatches.push_back( batch );

            // rebuild handle
            batchKeyIndex = mImpl->GetBatchKeyIndex( key );

            // Vertex buffer may be set before it's filled with data
            newGeometry->AddPropertyBuffer( batchVertexBuffer );

            // Register geometry with update manager
            mImpl->mUpdateManager->AddGeometry( newGeometry );
          }
        }

        // Tell node which batch it belongs to
        node->mBatchIndex = batchKeyIndex;

        uint32_t localIndex = BATCH_LOCAL_INDEX( batchKeyIndex );

        if( !batchParentData.mBatches[ localIndex ].mDirty )
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
        Batch& batch = batchParentData.mBatches[ localIndex ];
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
      std::vector<Batch>::iterator iter = batchParentData.mBatches.begin();
      std::vector<Batch>::iterator end = batchParentData.mBatches.end();
      for( ; iter != end; ++iter )
      {
        Batch& batch = (*iter);
        batch.mGeometry->SetIndexBuffer( batch.mIndices );
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
    if( node == batchParent.mBatchParent )
    {
      // tell children they're not batched anymore
      Vector<Node*>::Iterator iter = batchParent.mBatchedChildren.Begin();
      Vector<Node*>::Iterator end = batchParent.mBatchedChildren.End();
      for( ; iter != end; ++iter )
      {
        Node* child = *iter;
        child->mBatchIndex = BATCH_NULL_HANDLE;
      }

      // delete all resources that belongs to the batch parent
      for( size_t j = 0; j < batchParent.mBatches.size(); ++j )
      {
        Batch& batch = batchParent.mBatches[j];
        mImpl->mUpdateManager->RemoveGeometry( batch.mGeometry );
      }

      // delete main vertex buffer
      mImpl->mUpdateManager->RemovePropertyBuffer( batchParent.mVertexBuffer );

      return;
    }
  }
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
  if( node->mBatchIndex == BATCH_NULL_HANDLE )
  {
    return;
  }

  uint32_t parentIndex = BATCH_PARENT_INDEX( node->mBatchIndex );

  BatchParent& batchParent = mImpl->mBatchParents[ parentIndex ];

  // delete all batches from batch parent
  for( size_t i = 0; i < batchParent.mBatches.size(); ++i )
  {
    Batch& batch = batchParent.mBatches[ i ];

    // delete geometry
    mImpl->mUpdateManager->RemoveGeometry( batch.mGeometry );
  }

  batchParent.mBatches.clear();

  // for all children reset batch index to BATCH_NULL_HANDLE
  for( size_t i = 0; i < batchParent.mBatchedChildren.Size(); ++i )
  {
    Node* child = batchParent.mBatchedChildren[i];

    if( node == child )
    {
      batchParent.mBatchedChildren.Erase( batchParent.mBatchedChildren.Begin() + i );
      --i;
    }
    else
    {
      child->mBatchIndex = BATCH_NULL_HANDLE;
    }
  }

  mImpl->mUpdateManager->RemovePropertyBuffer( batchParent.mVertexBuffer );
  batchParent.mNeedsUpdate = true;
}

bool GeometryBatcher::HasRendered( uint32_t batchIndex )
{
  return mImpl->mBatchParents[ BATCH_PARENT_INDEX(batchIndex) ].mBatches[ BATCH_LOCAL_INDEX( batchIndex ) ].mBatchKey.mRendered;
}

void GeometryBatcher::SetRendered( uint32_t batchIndex )
{
  mImpl->mBatchParents[ BATCH_PARENT_INDEX(batchIndex) ].mBatches[ BATCH_LOCAL_INDEX( batchIndex ) ].mBatchKey.mRendered = true;
}

int GeometryBatcher::GetBatchIndexFromNode( Node* node )
{
  BatchKey key( node );
  return mImpl->GetBatchKeyIndex( key );
}

Render::Geometry* GeometryBatcher::GetGeometry( uint32_t batchIndex )
{
  return mImpl->mBatchParents[ BATCH_PARENT_INDEX(batchIndex) ].mBatches[ BATCH_LOCAL_INDEX( batchIndex ) ].mGeometry;
}

void GeometryBatcher::PrepareToRender()
{
  for( size_t i = 0; i < mImpl->mBatchParents.size(); ++i )
  {
    BatchParent& batchParent = mImpl->mBatchParents[i];
    for( size_t j = 0; j < batchParent.mBatches.size(); ++j )
    {
      Batch& batch = batchParent.mBatches[ j ];
      batch.mBatchKey.mRendered = false;
    }
  }
}



} // namespace SceneGraph

} // namespace Internal


} // namespace Dali
