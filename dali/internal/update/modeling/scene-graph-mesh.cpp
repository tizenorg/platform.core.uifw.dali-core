/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/modeling/scene-graph-mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/post-process-resource-dispatcher.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/queue/render-queue.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Mesh::Mesh( ResourceId id,
            PostProcessResourceDispatcher& postProcessResourceDispatcher,
            RenderQueue& renderQueue,
            MeshData* meshData )
:
  mPostProcessResourceDispatcher(postProcessResourceDispatcher),
  mRenderQueue(renderQueue),
  mUpdateMeshData(meshData),
  mRenderMeshData(meshData),
  mVertexBuffer(NULL),
  mIndicesBuffer(NULL),
  mNumberOfVertices(0u),
  mNumberOfFaces(0u),
  mResourceId ( id ),
  mRefreshVertexBuffer(true)
{
  CalculateBoundingBox(0, meshData);
  mBoundingBox[1] = mBoundingBox[0];
}

Mesh::~Mesh()
{
}

void Mesh::SetMeshData(BufferIndex updateBufferIndex, MeshData* meshData)
{
  // Called only from Update thread
  mUpdateMeshData = meshData;

  CalculateBoundingBox(updateBufferIndex, meshData);
  // MeshDataUpdated() will follow shortly in Render thread.
}

MeshData& Mesh::GetMeshData( Mesh::ThreadBuffer threadBuffer )
{
  return const_cast<MeshData&>(static_cast<const Mesh*>(this)->GetMeshData(threadBuffer));
}

const MeshData& Mesh::GetMeshData( Mesh::ThreadBuffer threadBuffer ) const
{
  MeshData* meshDataPtr = NULL;

  switch(threadBuffer)
  {
    case Mesh::UPDATE_THREAD:
    {

      meshDataPtr = mUpdateMeshData;
    }
    break;

    case Mesh::RENDER_THREAD:
    {
      meshDataPtr = &(*mRenderMeshData);
    }
    break;
  }

  DALI_ASSERT_DEBUG( meshDataPtr );
  return *meshDataPtr;
}

void Mesh::RefreshVertexBuffer()
{
    mRefreshVertexBuffer = true;
}

void Mesh::MeshDataUpdated( BufferIndex bufferIndex, Mesh::ThreadBuffer threadBuffer, MeshData* meshData )
{
  if ( threadBuffer == Mesh::RENDER_THREAD )
  {
    // Called from a message, the old MeshData will be release and the new one is saved.
    mRenderMeshData = meshData;
    RefreshVertexBuffer();
  }
  else // threadBuffer == Mesh::UPDATE_THREAD
  {
    // Dynamics and animatable meshes don't create new mesh data.
    DALI_ASSERT_DEBUG( threadBuffer == Mesh::UPDATE_THREAD );
    DALI_ASSERT_DEBUG( meshData == NULL );
    CalculateBoundingBox(bufferIndex, mUpdateMeshData);

    // Send a message to self in render thread
    typedef Message< Mesh > LocalType;
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( bufferIndex, sizeof( LocalType ) );
    new (slot) LocalType( this, &Mesh::RefreshVertexBuffer);
  }
}

void Mesh::UploadVertexData( Context& context, BufferIndex renderBufferIndex )
{
  // Short-circuit if nothing has changed
  if ( !mRefreshVertexBuffer )
  {
    return;
  }

  const MeshData::VertexContainer& vertices = mRenderMeshData->GetVertices();

  DALI_ASSERT_DEBUG( !vertices.empty() );
  if ( !mVertexBuffer )
  {
    mVertexBuffer = new GpuBuffer(context,GpuBuffer::ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
  }
  DALI_ASSERT_DEBUG(mVertexBuffer);

  mVertexBuffer->UpdateDataBuffer( vertices.size() * sizeof(MeshData::Vertex), &vertices.at(0) );
  mNumberOfVertices = mRenderMeshData->GetVertexCount();

  if ( size_t numberOfIndices = mRenderMeshData->GetFaceIndexCount() )
  {
    const MeshData::FaceIndices& faces = mRenderMeshData->GetFaces();
    DALI_ASSERT_DEBUG(!faces.empty());

    if ( !mIndicesBuffer )
    {
      mIndicesBuffer = new GpuBuffer(context,GpuBuffer::ELEMENT_ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
    }

    mIndicesBuffer->UpdateDataBuffer( numberOfIndices * sizeof(GLushort), &(faces.at(0)) );
    mNumberOfFaces = mRenderMeshData->GetFaceCount();
  }

  // Note, dispatcher should only be used in Render Thread (as should the rest of this method!)
  ResourcePostProcessRequest ppRequest( mResourceId, ResourcePostProcessRequest::UPLOADED );
  mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);

  mRenderMeshData->Discard();
  mRefreshVertexBuffer = false;
}

void Mesh::BindBuffers(Context& context)
{
  // Short-circuit if nothing has changed
  if ( !mVertexBuffer )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mIndicesBuffer || mRenderMeshData->GetVertexGeometryType() == Dali::MeshData::POINTS );

  mVertexBuffer->Bind();

  if( mIndicesBuffer)
  {
    mIndicesBuffer->Bind();
  }
}

size_t Mesh::GetFaceIndexCount( ThreadBuffer threadBuffer ) const
{
  DALI_ASSERT_DEBUG( threadBuffer == Mesh::RENDER_THREAD );
  size_t faceCount= 0;
  switch( GetMeshData(threadBuffer).GetVertexGeometryType() )
  {
    case Dali::MeshData::POINTS:
      faceCount = mNumberOfVertices;
      break;
    case Dali::MeshData::LINES:
      faceCount = mNumberOfFaces*2;
      break;
    case Dali::MeshData::TRIANGLES:
      faceCount = mNumberOfFaces*3;
      break;
  }

  return faceCount;
}

bool Mesh::HasGeometry( ThreadBuffer threadBuffer ) const
{
  return GetMeshData(threadBuffer).GetVertexCount() > 0;
}

const BoundingBox& Mesh::GetBoundingBox(BufferIndex bufferIndex)
{
  return mBoundingBox[bufferIndex];
}

void Mesh::GlContextDestroyed()
{
  if( mVertexBuffer )
  {
    mVertexBuffer->GlContextDestroyed();
  }
  if( mIndicesBuffer )
  {
    mIndicesBuffer->GlContextDestroyed();
  }
}

void Mesh::GlCleanup()
{
  mVertexBuffer = NULL;
  mIndicesBuffer = NULL;
}



void Mesh::CalculateBoundingBox(BufferIndex bufferIndex, MeshData* meshData)
{
  /// @todo Should only calculate bounding box if it's going to be used...
  /// Though, currently no obvious way of checking this - it's in the SceneGraph::Renderer base class...

  Vector3& minVertex = mBoundingBox[bufferIndex].minVertex;
  Vector3& maxVertex = mBoundingBox[bufferIndex].maxVertex;

  minVertex = Vector3::ZERO;
  maxVertex = Vector3::ZERO;

  if( meshData != NULL )
  {
    const MeshData::VertexContainer& vertices = meshData->GetVertices();
    if( ! vertices.empty() )
    {
      minVertex = Vector3::ONE *  1e10f;
      maxVertex = Vector3::ONE * -1e10f;
      for( MeshData::VertexContainer::const_iterator iter = vertices.begin(), end = vertices.end() ;
           iter != end ; ++iter )
      {
        minVertex.x = min( minVertex.x, iter->x );
        minVertex.y = min( minVertex.y, iter->y );
        minVertex.z = min( minVertex.z, iter->z );
        maxVertex.x = max( maxVertex.x, iter->x );
        maxVertex.y = max( maxVertex.y, iter->y );
        maxVertex.z = max( maxVertex.z, iter->z );
      }
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
