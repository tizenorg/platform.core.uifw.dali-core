#include "geometry-batcher.h"

#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>

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

    if( batchParentNode )
      item->mSkipIfBatched = true;

    const SceneGraph::Renderer* sgRenderer = node->GetRendererAt( 0 );

    BatchInfo info( batchParentNode, &sgRenderer->GetMaterial(), NULL );

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

  // new indices for batch
  std::vector< unsigned > indices;
  indices.reserve( 1024 );

  for( ; it != end; ++it )
  {
    // collect batch common input data
    Marker& marker = *it;
    BatchInfo& info = marker.info;

    RenderItem* firstItem = ppItems[ marker.from ];
    const Render::PropertyBuffer::Format *format = firstItem->GetNode().GetRendererAt(0)->GetGeometry().GetVertexBuffers()[0]->GetFormat();
    const Render::PropertyBuffer::Format *indexFormat = firstItem->GetNode().GetRendererAt(0)->GetGeometry().GetIndexBuffer()->GetFormat();
    if( !format || !indexFormat )
      continue;

    if( !firstItem->GetBatchGeometry() )
    {
      // -----------------------------------------------------------------------------------------
      // calculate size of buffers
      size_t vertexBufferSize = 0;
      size_t indexBufferSize = 0;
      for( size_t k = marker.from; k < marker.to; ++k )
      {
        RenderItem* item = ppItems[ k ];
        SceneGraph::Renderer* sgRenderer = item->GetNode().GetRendererAt( 0 );
        Render::PropertyBuffer* vertexBuffer = sgRenderer->GetGeometry().GetVertexBuffers()[0];
        Render::PropertyBuffer* indexBuffer = sgRenderer->GetGeometry().GetIndexBuffer();

        vertexBufferSize += vertexBuffer->GetSize();
        indexBufferSize += indexBuffer->GetSize();
      }
      // -----------------------------------------------------------------------------------------
      // initialise batch geometry for each new batch ( what happens if render item goes? should remove own batch too )

      SceneGraph::RenderGeometry *geometry = new SceneGraph::RenderGeometry( Dali::Geometry::TRIANGLES, false );

      Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
      Render::PropertyBuffer::Format* clonedIndexFormat = new Render::PropertyBuffer::Format( *indexFormat );

      // create buffers
      Render::PropertyBuffer* vertexBuffer = new Render::PropertyBuffer();
      Render::PropertyBuffer* indexBuffer = new Render::PropertyBuffer();;

      // assign formats using cloned values
      vertexBuffer->SetFormat( clonedVertexFormat );
      indexBuffer->SetFormat( clonedIndexFormat );

      vertexBuffer->SetSize( vertexBufferSize );
      indexBuffer->SetSize( indexBufferSize );

      // set vertex data buffer
      Vector<char>* vertexData = new Vector<char>();
      vertexData->Resize( vertexBufferSize*16 );
      vertexBuffer->SetData( vertexData );

      // set index data buffer
      Vector<char>* indexData = new Vector<char>();
      indexData->Resize( indexBufferSize*4 );
      indexBuffer->SetData( indexData );

      // add buffers - one vertex, one index

      geometry->AddPropertyBuffer( vertexBuffer, false );
      geometry->AddPropertyBuffer( indexBuffer, true );

      // enable batch geometry
      firstItem->SetBatchGeometry( geometry );

      // store geometry
      info.mGeometry = geometry;
      info.mVertexBuffer = vertexBuffer;
      info.mIndexBuffer = indexBuffer;
      firstItem->mBatchVertexBuffer = vertexBuffer;
      firstItem->mBatchIndexBuffer = indexBuffer;
    }
    else
    {
      info.mGeometry = firstItem->GetBatchGeometry();
      if(!firstItem->mBatchVertexBuffer)
      {
        puts("dammit!");
      }
      info.mVertexBuffer = firstItem->mBatchVertexBuffer;
      info.mIndexBuffer = firstItem->mBatchIndexBuffer;
      puts("Item has geometry!");
    }

    // second phase, recompute data and add indices into the index buffer

    size_t vertexIter = 0;
    size_t indexIter = 0;

    for( size_t k = marker.from; k < marker.to; ++k )
    {
      RenderItem* item = ppItems[ k ];

      if ( k > marker.from )
      {
        item->mSkipIfBatched = true;
        item->SetBatchGeometry( info.mGeometry );
      }
      else
      {
        item->mSkipIfBatched = false;
      }

      Matrix modelMatrix = item->GetNode().GetBatchParent()->GetWorldMatrix( bufferIndex );
      Matrix::Multiply( item->GetModelViewMatrix(),
                        modelMatrix,
                        viewMatrix );

      // need to use SG renderer, as Render::Renderer may not have the values set yet

      // set output buffers
      Vector<char>& outIndexBuffer = info.mIndexBuffer->GetData();
      unsigned* outIndexPtr = (unsigned*)&outIndexBuffer[0];
      Vector<char>& outVertexBuffer = info.mVertexBuffer->GetData();
      float* outVertexPtr = (float*)&outVertexBuffer[0];

      // set input buffers
      Vector<char>& inIndexBuffer = item->GetNode().GetRendererAt(0)->GetGeometry().GetIndexBuffer()->GetData();
      Vector<char>& inVertexBuffer = item->GetNode().GetRendererAt(0)->GetGeometry().GetVertexBuffers()[0]->GetData();

      float* inVertexPtr = (float*)&inVertexBuffer[0];

      // save indices
      for( size_t j = 0; j < 6; ++j )
      {
        outIndexPtr[j + indexIter] = j + indexIter;
      }
      indexIter += 6; // 2 tris

      // save vertices
      memcpy(&outVertexPtr[ vertexIter*4 ], inVertexPtr, 16*6);
      TransformVertices( &outVertexPtr[ vertexIter*4 ], inVertexPtr, &item->GetNode(), 16, 6, bufferIndex );
      vertexIter += 6;
    }

    info.mVertexBuffer->UpdateData();
    info.mIndexBuffer->UpdateData();
  }

  fflush(stdout);
  return 0;
}

bool GeometryBatcher::UpdateGeometry( BatchInfo* batchInfo )
{
  // no update needed, return

  return true;
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
