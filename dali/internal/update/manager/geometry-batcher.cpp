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

#include <cstdio>
#include <cstring>
namespace
{
static uint32_t crc32_tab[] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
  0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
  0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
  0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t
CRC(uint32_t crc, const void *buf, size_t size)
{
  const uint8_t* p = (const uint8_t*)buf;
  crc = crc ^ ~0U;

  while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

  uint32_t retval = crc ^ ~0U;
  //printf("  >>> CRC: 0x%X\n", retval);
  //fflush(stdout);
  return retval;
}
/*
uint32_t CRC(unsigned long crc, const unsigned char* data, size_t s)
{
  unsigned long hash = !crc ? 5381 : crc;
  int c;

  while ( s )
  {
    c = *data++;
    hash = ((hash << 5) + hash) + c;
    --s;
  }

  return (uint32_t)hash;
}
*/
/*
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
*/
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

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
  // -----------------------------------------------------------------------------
  // Phase 1.
  // Analyze how many batches we have and what goes in
  for( BatchMap::iterator it = mBatchMap.begin(); it != mBatchMap.end(); ++it )
  {
    (it->second)->mTouched = false;
  }


  // batch markers store ony indices from and to where the batch is identified
  // from ids the crc is computed for further storing
  typedef std::vector< Marker > VecMarker;
  VecMarker batchMarkers;
  batchMarkers.reserve( 128 );
  size_t i = 0;
  size_t batchesRepeated = 0;
  for( i = 0; i < count; ++i )
  {
    RenderItem* item = ppItems[ i ];
    const Node* node = &item->GetNode();
    const Node* batchParentNode = node->GetBatchParent();

    //printf("item: %p\n", item);
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
        //crc = (intptr_t)currentBatchInfo.mBatchParent;
        currentBatchInfo.mCRC = crc;

        if( mBatchMap.find( crc ) != mBatchMap.end() )
        {
          //printf("repeated CRC: 0x%x [%d - %d]\n ", (unsigned)crc, (int)startBatchIndex, (int)i);
          batchesRepeated++;
        }
        else
        {
          //printf("added    CRC: 0x%x [%d - %d]\n ", (unsigned)crc, (int)startBatchIndex, (int)i);
          mBatchMap[ crc ] = new BatchInfo( currentBatchInfo );
        }

        mBatchMap[ crc ]->mTouched = true;

        batchMarkers.push_back( Marker( *mBatchMap[ crc ], startBatchIndex, i ) );
      }

      crc = 0;
      currentBatchInfo.Reset();
      if( batchParentNode ) // new batch
      {
        startBatchIndex = i;
        currentBatchInfo = info;
        crc = CRC( 0, (uint8_t**)&node, sizeof(void*) );
      }
    }
    else
    {
      crc = CRC( crc, (uint8_t**)&node, sizeof(void*) );
      // push index
    }
  }

  // last batch remained
  if( currentBatchInfo )
  {
    //crc = (intptr_t)currentBatchInfo.mBatchParent;
    currentBatchInfo.mCRC = crc;
    if( mBatchMap.find( currentBatchInfo.mCRC ) != mBatchMap.end() )
    {
      //printf("repeated CRC: 0x%x [%d - %d]\n ", (unsigned)crc, (int)startBatchIndex, (int)i);
      batchesRepeated++;
    }
    else
    {
      //printf("added    CRC: 0x%x [%d - %d]\n ", (unsigned)crc, (int)startBatchIndex, (int)i);
      mBatchMap[ currentBatchInfo.mCRC ] = new BatchInfo( currentBatchInfo );
    }
    mBatchMap[ crc ]->mTouched = true;
    batchMarkers.push_back( Marker( *mBatchMap[ currentBatchInfo.mCRC ], startBatchIndex, i ) );
  }

  // delete what's not touched
  std::vector<uint32_t> keys;

  for( BatchMap::iterator it = mBatchMap.begin(); it != mBatchMap.end(); ++it )
  {
    if( !(it->second)->mTouched )
    {
      keys.push_back( it->first );
    }
  }

  printf("Keys to remove: %d\n", (int)keys.size());
  for( size_t t = 0; t < keys.size(); ++t )
  {
    //BatchInfo* info = mBatchMap[t];
    //mBatchMap.erase( keys[t] );
  }

  // -----------------------------------------------------------------------------
  // Phase 1a ( optional )
  // Having CRCs check cached batches to reuse them ( not implemented )

  // -----------------------------------------------------------------------------
  // Phase 2.
  // For each batch create element buffer maintaining the order ( batching should
  // not affect the drawing order )
  VecMarker::iterator it = batchMarkers.begin();
  VecMarker::iterator end = batchMarkers.end();

  size_t generated = 0;
  puts("---------------------------------------");
  for( ; it != end; ++it )
  {
    // collect batch common input data
    Marker& marker = *it;
    BatchInfo& info = marker.info;

    RenderItem* firstItem = ppItems[ marker.from ];
    printf("Batch crc: 0x%X, batchParent = %p, size = %d, mat = %p, shader = %p\n", info.mCRC, info.mBatchParent, (int)(marker.to - marker.from), info.mMaterial, info.mMaterial->GetShader() );

    //for( size_t p = marker.from; p < marker.to; ++p )
   // {
   //   printf("    >>> %p\n", &ppItems[p]->GetNode() );
   // }

    const Render::PropertyBuffer::Format *format = firstItem->GetNode().GetRendererAt(0)->GetGeometry().GetVertexBuffers()[0]->GetFormat();
    const Render::PropertyBuffer::Format *indexFormat = firstItem->GetNode().GetRendererAt(0)->GetGeometry().GetIndexBuffer()->GetFormat();
    if( !format || !indexFormat )
    {
      puts("   >>> continuing");
      // remove from map
      mBatchMap.erase( info.mCRC );
      continue;
    }
    if( !info.mGeometry )
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

      ++generated;

      SceneGraph::RenderGeometry *geometry = new SceneGraph::RenderGeometry( Dali::Geometry::TRIANGLES, false );

      Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
      Render::PropertyBuffer::Format* clonedIndexFormat = new Render::PropertyBuffer::Format( *indexFormat );

      static int bufs = 0;
      bufs += 2;
      printf( "BUFS: Bufs created: %d\n", bufs );

      // create buffers
      Render::PropertyBuffer* vertexBuffer = new Render::PropertyBuffer();
      Render::PropertyBuffer* indexBuffer = new Render::PropertyBuffer();;

      // assign formats using cloned values
      vertexBuffer->SetFormat( clonedVertexFormat );
      indexBuffer->SetFormat( clonedIndexFormat );

      // set vertex data buffer
      Vector<char>* vertexData = new Vector<char>();
      vertexData->Resize( vertexBufferSize*16 );
      vertexBuffer->SetData( vertexData, vertexBufferSize );

      // set index data buffer
      Vector<char>* indexData = new Vector<char>();
      indexData->Resize( indexBufferSize*4 );
      indexBuffer->SetData( indexData, indexBufferSize );

      // add buffers - one vertex, one index
      geometry->AddPropertyBuffer( vertexBuffer, false );
      geometry->AddPropertyBuffer( indexBuffer, true );

      // enable batch geometry
      // store geometry
      info.mGeometry = geometry;
      info.mVertexBuffer = vertexBuffer;
      info.mIndexBuffer = indexBuffer;
      info.mTouched = true;
    }
    else
    {
      //info.mGeometry = firstItem->GetBatchGeometry();
      //puts("Item has geometry!");
    }


    //firstItem->SetBatchGeometry( info.mGeometry );

    // second phase, recompute data and add indices into the index buffer

    size_t vertexIter = 0;
    size_t indexIter = 0;

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
        item->SetBatchGeometry( info.mGeometry );
      }

      Matrix modelMatrix = item->GetNode().GetBatchParent()->GetWorldMatrix( bufferIndex );
      Matrix::Multiply( item->GetModelViewMatrix(),
                        modelMatrix,
                        viewMatrix );

      // need to use SG renderer, as Render::Renderer may not have the values set yet

      if( info.mTouched )
      {
        // set output buffers
        Vector<char>& outIndexBuffer = info.mIndexBuffer->GetData();
        unsigned* outIndexPtr = (unsigned*)&outIndexBuffer[0];
        Vector<char>& outVertexBuffer = info.mVertexBuffer->GetData();
        float* outVertexPtr = (float*)&outVertexBuffer[0];

        // set input buffers
        //Vector<char>& inIndexBuffer = item->GetNode().GetRendererAt(0)->GetGeometry().GetIndexBuffer()->GetData();
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
    }

    if( info.mTouched )
    {
      info.mVertexBuffer->UpdateData();
      info.mIndexBuffer->UpdateData();
    }
    info.mTouched = false;
  }

  puts("---------------------------------------");

  printf( "Batch markers: %d, repeated: %d, map size: %d, generated: %d\n", (int)batchMarkers.size(), (int)batchesRepeated, (int)mBatchMap.size(), (int)generated);

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

  const Matrix& nodeMatrix = node->GetWorldMatrix( bufferIndex );
  const Matrix& parentMatrix = node->GetBatchParent()->GetWorldInvertedMatrix( bufferIndex );
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
  return NULL;//transformedGeometry;
}

void GeometryBatcher::DiscardBatches()
{

}

void GeometryBatcher::Update( BufferIndex bufferIndex, RenderInstructionContainer& instructions )
{
  const size_t riCount = instructions.Count( bufferIndex );
  for( size_t i = 0; i < riCount; ++i )
  {
    RenderInstruction& instruction = instructions.At( bufferIndex, i );
    int renderListCount = instruction.RenderListCount();
    if( renderListCount )
    {
      RenderList* list = const_cast<RenderList*>(instruction.GetRenderList( 0 ));
      RebuildBatches(
            (RenderItem**)list->GetContainer().Begin(),
            list->GetContainer().Size(),
            *instruction.GetViewMatrix( bufferIndex ), bufferIndex );
    }
  }
}

} // SceneGraph

} // Internal

} // Dali
