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
#include <dali/internal/render/renderers/render-mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderMesh* RenderMesh::New(MeshData* meshData)
{
  DALI_ASSERT_DEBUG(meshData);
  return new RenderMesh(meshData);
}

RenderMesh::~RenderMesh()
{
}

const MeshData& RenderMesh::GetMeshData() const
{
  DALI_ASSERT_DEBUG( mMeshData );
  return *mMeshData;
}

void RenderMesh::RefreshVertexBuffer()
{
    mRefreshVertexBuffer = true;
}

void RenderMesh::MeshDataReplaced( MeshData* meshData )
{
  // Called from a message, the old MeshData will be release and the new one is saved.
  DALI_ASSERT_DEBUG(meshData);
  mMeshData = meshData;
  RefreshVertexBuffer();
}

void RenderMesh::UploadVertexData( Context& context )
{
  // Short-circuit if nothing has changed
  if ( !mRefreshVertexBuffer )
  {
    return;
  }

  DoUpload(context);

  mMeshData->Discard();
  mRefreshVertexBuffer = false;
}

void RenderMesh::DoUpload( Context& context )
{
  DALI_ASSERT_DEBUG(mMeshData);
  const MeshData::VertexContainer& vertices = mMeshData->GetVertices();

  DALI_ASSERT_DEBUG( !vertices.empty() );
  if ( !mVertexBuffer )
  {
    mVertexBuffer = new GpuBuffer(context,GpuBuffer::ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
  }
  DALI_ASSERT_DEBUG(mVertexBuffer);

  mVertexBuffer->UpdateDataBuffer( vertices.size() * sizeof(MeshData::Vertex), &vertices.at(0) );
  mNumberOfVertices = mMeshData->GetVertexCount();

  if ( size_t numberOfIndices = mMeshData->GetFaceIndexCount() )
  {
    const MeshData::FaceIndices& faces = mMeshData->GetFaces();
    DALI_ASSERT_DEBUG(!faces.empty());

    if ( !mIndicesBuffer )
    {
      mIndicesBuffer = new GpuBuffer(context,GpuBuffer::ELEMENT_ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
    }

    mIndicesBuffer->UpdateDataBuffer( numberOfIndices * sizeof(GLushort), &(faces.at(0)) );
    mNumberOfFaces = mMeshData->GetFaceCount();
  }
}

void RenderMesh::BindBuffers(Context& context)
{
  // Short-circuit if nothing has changed
  if ( !mVertexBuffer )
  {
    return;
  }

  DALI_ASSERT_DEBUG( ( mIndicesBuffer && mIndicesBuffer->BufferIsValid() )
                     || mMeshData->GetVertexGeometryType() == Dali::MeshData::POINTS );

  // Try and recover from context loss using retained data.
  if( ! mVertexBuffer->BufferIsValid() && ! mMeshData->GetVertices().empty() )
  {
    DoUpload( context );
  }

  if( mVertexBuffer->BufferIsValid() )
  {
    mVertexBuffer->Bind();
  }

  if( mIndicesBuffer && mIndicesBuffer->BufferIsValid())
  {
    mIndicesBuffer->Bind();
  }
}


size_t RenderMesh::GetFaceIndexCount() const
{
  size_t faceCount= 0;
  switch( mMeshData->GetVertexGeometryType() )
  {
    case Dali::MeshData::POINTS:
    {
      faceCount = mNumberOfVertices;
      break;
    }
    case Dali::MeshData::LINES:
    {
      faceCount = mNumberOfFaces*2;
      break;
    }
    case Dali::MeshData::TRIANGLES:
    {
      faceCount = mNumberOfFaces*3;
      break;
    }
  }

  return faceCount;
}

bool RenderMesh::HasGeometry( ) const
{
  DALI_ASSERT_DEBUG( mMeshData );
  return mMeshData->GetVertexCount() > 0;
}

void RenderMesh::GlContextDestroyed()
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

void RenderMesh::GlCleanup()
{
  mVertexBuffer = NULL;
  mIndicesBuffer = NULL;
}


RenderMesh::RenderMesh( MeshData* meshData )
: mMeshData( meshData ),
  mVertexBuffer( NULL ),
  mIndicesBuffer( NULL ),
  mNumberOfVertices(0),
  mNumberOfFaces(0),
  mRefreshVertexBuffer( true )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
