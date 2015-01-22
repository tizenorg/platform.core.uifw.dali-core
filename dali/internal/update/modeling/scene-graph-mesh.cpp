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
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-mesh.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Mesh* Mesh::New( ResourceId id,
                 RenderQueue& renderQueue,
                 MeshData* meshData,
                 RenderMesh* renderMesh )
{
  return new Mesh( id, renderQueue, meshData, renderMesh );
}


Mesh::Mesh( ResourceId id,
            RenderQueue& renderQueue,
            MeshData* meshData,
            RenderMesh* renderMesh )
: mResourceId( id ),
  mRenderQueue( renderQueue ),
  mRenderMesh( renderMesh ),
  mMeshData( meshData ),
  mNumberOfVertices( meshData->GetVertexCount() ),
  mNumberOfFaces( meshData->GetFaceCount() )
{
}

Mesh::~Mesh()
{
}

void Mesh::SetMeshData( BufferIndex bufferIndex, MeshData* meshData )
{
  mMeshData = meshData;
  mNumberOfVertices = meshData->GetVertexCount();
  mNumberOfFaces = meshData->GetFaceCount();

  // Send a message to the render mesh
  typedef MessageValue1< RenderMesh, OwnerPointer<MeshData> > LocalType;
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( bufferIndex, sizeof( LocalType ) );
  new (slot) LocalType( mRenderMesh, &RenderMesh::MeshDataReplaced, mMeshData );
}

const MeshData& Mesh::GetMeshData( BufferIndex /*bufferIndex*/ ) const
{
  DALI_ASSERT_DEBUG(mMeshData);
  return *mMeshData;
}

MeshData& Mesh::GetMeshData( BufferIndex bufferIndex )
{
  return const_cast<MeshData&>( const_cast<const Mesh*>(this)->GetMeshData( bufferIndex ) );
}

void Mesh::MeshDataUpdated( BufferIndex bufferIndex )
{
  // Send a message to self in render thread
  typedef Message< RenderMesh > LocalType;
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( bufferIndex, sizeof( LocalType ) );
  new (slot) LocalType( mRenderMesh, &RenderMesh::RefreshVertexBuffer);
}

bool Mesh::HasGeometry( BufferIndex bufferIndex ) const
{
  return GetMeshData( bufferIndex ).GetVertexCount() > 0u;
}

RenderMesh* Mesh::GetRenderMesh()
{
  return mRenderMesh;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
