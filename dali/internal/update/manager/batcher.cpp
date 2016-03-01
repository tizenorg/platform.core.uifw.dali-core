#include "geometry-batcher.h"

#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>

#include <cstdio>
#include <cstring>
namespace
{
#define CRC_MAGIC_NUMBER 0x82f63b78
uint32_t CRC( uint32_t crc, const unsigned char *buf, size_t len )
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ CRC_MAGIC_NUMBER : crc >> 1;
    }
    return ~crc;
}

}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/*
// The first call to GetRenderGeometry() creates the geometry and sends it in a message
RenderGeometry* geometry = mGeometry->GetRenderGeometry( mSceneController );

typedef MessageValue1< Render::Renderer, RenderGeometry* > DerivedType;
unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

new (slot) DerivedType( mRenderer, &Render::Renderer::SetGeometry, geometry );
mResendFlag &= ~RESEND_GEOMETRY;
*/

// ------------------------------------------------------------------------
// helper struct describing common properties of single batch
struct BatchInfo
{
  BatchInfo(
      const Node* node = NULL,
      const Material* material = NULL,
      const Geometry* geometry = NULL
      ) :
      mBatchParent( node )
    , mMaterial( material )
    , mGeometry( geometry ) {}


  bool operator==( const BatchInfo& rhs ) const
  {
    return ( rhs.mBatchParent == mBatchParent ) &&
        (rhs.mMaterial == mMaterial ) &&
        (rhs.mGeometry == mGeometry );
  }

  bool operator!=( const BatchInfo& rhs ) const
  {
    return !(( rhs.mBatchParent == mBatchParent ) &&
        (rhs.mMaterial == mMaterial ) &&
        (rhs.mGeometry == mGeometry ));
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
    return ( mBatchParent && mMaterial && mGeometry );
  }

  bool operator!() const
  {
    return !mBatchParent || !mMaterial || !mGeometry;
  }

  void Reset()
  {
    mBatchParent = NULL;
    mMaterial = NULL;
    mGeometry = NULL;
  }

  const Node*       mBatchParent;
  const Material*   mMaterial;
  const Geometry*   mGeometry;

  uint32_t          mCRC;
};

struct Marker
{
  Marker( BatchInfo i, size_t f, size_t t ) :
    info( i ), from( f ), to( t ) {}

  BatchInfo info;
  size_t from;
  size_t to;
};

// ------------------------------------------------------------------------

size_t GeometryBatcher::RebuildBatches( RenderItem **ppItems, size_t count,
                                        const Matrix& viewMatrix,
                                        unsigned int bufferIndex )
{
  BatchInfo currentBatchInfo;
  size_t startBatchIndex = 0;
  //GeometryBatchInfo* batchInfo = NULL;
  intptr_t crc = 0;

  mVertexBufferMapping.clear();

  // -----------------------------------------------------------------------------
  // Phase 1.
  // Analyze how many batches we have and what goes in

  // batch markers store ony indices from and to where the batch is identified
  // from ids the crc is computed for further storing
  typedef std::vector< Marker > VecMarker;
  VecMarker batchMarkers;
  batchMarkers.reserve( 128 );
  size_t i = 0;
  for( i = 0; i < count; ++i )
  {
    RenderItem* item = ppItems[ i ];
    const Node* node = &item->GetNode();
    const Node* batchParentNode = node->GetBatchParent();
    const SceneGraph::Renderer* sgRenderer = node->GetRendererAt( 0 );
    /*
    Render::Renderer* rgRenderer = &item->GetRenderer();

    Render::Renderer* rgRenderer2 = &(const_cast<SceneGraph::Renderer*>(sgRenderer))->GetRenderer();
    rgRenderer = rgRenderer;
    rgRenderer2 = rgRenderer2;
    */
    BatchInfo info( batchParentNode, &sgRenderer->GetMaterial(), &sgRenderer->GetGeometry() );

    // ------------------------------------------------------------
    if( !currentBatchInfo || currentBatchInfo != info ) // nobatch, or batch swapped
    {
      // push batch marker
      if( currentBatchInfo )
      {
        // update CRC
        currentBatchInfo.mCRC = crc;
        batchMarkers.push_back( Marker( currentBatchInfo, startBatchIndex, i ) );
      }

      currentBatchInfo.Reset();
      if( batchParentNode ) // new batch
      {
        startBatchIndex = i;
        currentBatchInfo = info;
        crc = CRC( 0, *(uint8_t**)&item, sizeof(void*) );
      }
    }
    else
    {
      crc = CRC( crc, *(uint8_t**)&item, sizeof(void*) );
      // push index
    }
  }

  // last batch remained
  if( currentBatchInfo )
  {
    batchMarkers.push_back( Marker( currentBatchInfo, startBatchIndex, i ) );
  }

  printf( "Batch markers: %d\n", (int)batchMarkers.size() );

  // -----------------------------------------------------------------------------
  // Phase 1a ( optional )
  // Having CRCs check cached batches to reuse them ( not implemented )

  // -----------------------------------------------------------------------------
  // Phase 2.
  // For each batch create element buffer maintaining the order ( batching should
  // not affect the drawing order )
  VecMarker::iterator it = batchMarkers.begin();
  VecMarker::iterator end = batchMarkers.end();

  std::vector< unsigned > indices;
  indices.reserve( 1024 ); // 4k
  for( ; it != end; ++it )
  {
    // collect batch common input data
    Marker& marker = *it;
    const BatchInfo& info = marker.info;
    Render::PropertyBuffer* indexBuffer = info.mGeometry->GetIndexBuffer();
    Render::PropertyBuffer* vertexBuffer = const_cast<Geometry*>( info.mGeometry )->GetVertexBuffers()[0];

    // verify if buffers are ready ( format must be set )
    if( !vertexBuffer->GetFormat() )
    {
      continue; // skip batch
    }

    //Render::PropertyBuffer* vertexBufferTransformed = NULL;

    // create transformed vertex buffer
    if( mVertexBufferMapping.find( vertexBuffer ) == mVertexBufferMapping.end() )
    {
      /*
      vertexBufferTransformed = new Render::PropertyBuffer();
      mVertexBufferMapping[ vertexBuffer ] = vertexBufferTransformed;

      // clone setup
      const Render::PropertyBuffer::Format* pFormat = vertexBuffer->GetFormat();
      vertexBufferTransformed->SetFormat( new Render::PropertyBuffer::Format(*pFormat) );
      vertexBufferTransformed->SetSize( vertexBuffer->GetElementCount() );

      // set data ( make a copy of original for now )
      Dali::Vector< char >* vertexData = new Dali::Vector< char >( vertexBuffer->GetData() );
      vertexBufferTransformed->SetData( vertexData ); // pass ownership
      */
    }
    else
    {
      //vertexBufferTransformed = mVertexBufferMapping[ vertexBuffer ];
    }

    // to fill with transformed coordinates
    Vector< char >& vec = vertexBuffer->GetBaseData();
    int s = vec.Size();
    s = s;
    float* vertexDataSrc = (float*)&vertexBuffer->GetBaseData()[0];
    float* vertexDataDst = (float*)&vertexBuffer->GetData()[0];

    indices.clear();

    // -----------------------------------------------------------------------------
    // Phase 2a.
    // for each item update indices
    // TODO: implement triangle strips
    for( size_t k = marker.from; k < marker.to; ++k )
    {
      RenderItem* item = ppItems[ k ];

      if ( k > marker.from )
      {
        item->mSkipIfBatched = true;
      }
      else
      {
        item->mSkipIfBatched = false;

        // save MV matrix onto the item
        Matrix modelMatrix = item->GetNode().GetBatchParent()->GetWorldMatrix( bufferIndex );
        Matrix::Multiply( item->GetModelViewMatrix(),
                          modelMatrix,
                          viewMatrix );
      }

      // need to use SG renderer, as Render::Renderer may not have the values set yet
      Renderer* sgRenderer = item->GetNode().GetRendererAt( 0 );

      size_t elementOffset, elementRange;
      sgRenderer->GetElementRange( elementOffset, elementRange );

      // copy indices
      unsigned* loopStart = (unsigned*)&indexBuffer->GetData()[ elementOffset*sizeof( unsigned ) ];
      indices.insert( indices.end(), loopStart, loopStart + elementRange );

      unsigned vertexStartIndex = *loopStart;

      // -----------------------------------------------------------------------------
      // update vertices
      // temporarily format is hardcoded and 2 first float attributes are
      // position
      // FIXME: 16 - should be size of format, better to pass a format informaiton telling
      // what's the size of position. for batching may be better to represent position with Vec4 to avoid
      // transformation, more bandwidth, less calculation
      TransformVertices( &vertexDataDst[ vertexStartIndex*4 ], &vertexDataSrc[ vertexStartIndex*4 ], &item->GetNode(), 16, 6, bufferIndex );

      float* ff = &vertexDataDst[ vertexStartIndex ];
      ff ++;

      printf("item: %p, idx from = %d, to = %d\n", item, (int)elementOffset, (int)elementRange );
    }

    // -----------------------------------------------------------------------------
    // For each batch, first renderer set vertex buffer ( transformed )
    //
    //RenderItem* baseItem = ppItems[ marker.from ];
    vertexBuffer->UpdateData();

    float* f = (float*)&(vertexBuffer->GetData()[0]);
    f=f + 1;

    // tell first render item, to use new index buffer. have no idea how :(
    //RenderItem* baseItem = ppItems[ marker.from ];
    // set new index buffer

    // override buffers in the render::renderer
    //Render::Renderer* rgRenderer = &baseItem->GetRenderer();

    // render all
    //rgRenderer->SetElementsRange( 0, indices.size() );

    // we need transformed geometry to be set, it will override ( should be managed
    // by batch manager )


    /*
    RenderGeometry* transformedGeometry = new RenderGeometry( Dali::Geometry::TRIANGLES, false );
    Render::PropertyBuffer* elementBuffer = new Render::PropertyBuffer();
    Render::PropertyBuffer* arrayBuffer = new Render::PropertyBuffer();

    // ugly
    Dali::Vector< char > *tmp = new Dali::Vector< char >();
    tmp->Resize( indices.size() * sizeof( unsigned ) );

    // starting hackaton! not sure if this will work, Dali::Vector<> is missing
    // operating on sets of elements
    memcpy( &(*tmp)[0], (char*)indices.data(), indices.size() * sizeof( unsigned ) );

    elementBuffer->SetData( tmp ); // pass ownership

    // add transformed vertics
    transformedGeometry->AddPropertyBuffer( arrayBuffer, false );

    // add indices
    transformedGeometry->AddPropertyBuffer( elementBuffer, true );

    // set geometry for batch by sending message
    //rgRenderer->SetGeometry( transformedGeometry );

    // mark other items not renderable

    */

  }

  return 0;
}

void GeometryBatcher::TransformVertices( void* dst, void* src, const Node* node, size_t componentSize, size_t numVerts, unsigned int bufferIndex )
{
  char* source = (char*)src;
  char* dest = (char*)dst;

  Matrix parentMatrix = node->GetBatchParent()->GetWorldMatrix( bufferIndex );
  Matrix nodeMatrix = node->GetWorldMatrix( bufferIndex );
  parentMatrix.Invert();

  Matrix finalMatrix;
  Matrix::Multiply( finalMatrix, nodeMatrix, parentMatrix );

  for( size_t i = 0; i < numVerts; ++i )
  {
    Vector2& sourcePos = *(Vector2*)source;
    Vector4 pos4( sourcePos.x, sourcePos.y, 0.0f, 1.0f );
    Vector4 finalPos4;
    Matrix::Multiply( finalPos4, finalMatrix, pos4 );
    *((Vector2*)dest) = Vector2( finalPos4.x, finalPos4.y );
    source += componentSize;
    dest += componentSize;
  }
}

RenderGeometry* GeometryBatcher::GetBatchedGeometry( Render::PropertyBuffer* vertexBuffer, Render::PropertyBuffer* indexBuffer )
{
  /*
  RenderGeometry* transformedGeometry = NULL;
  // find by key
  if( 0 ) // geometry not found
  {
    transformedGeometry = new RenderGeometry( Dali::Geometry::TRIANGLES, false );
    indexBuffer = new Render::PropertyBuffer();
    vertexBuffer = new Render::PropertyBuffer();

    transformedGeometry->AddPropertyBuffer( vertexBuffer, false );
    transformedGeometry->AddPropertyBuffer( indexBuffer, true );

  }
  */
  return NULL;//transformedGeometry;
}

} // SceneGraph

} // Internal

} // Dali
