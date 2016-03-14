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
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace
{
//Memory pool used to allocate new RenderItems. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderItem> gRenderItemPool;
}
namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderItem* RenderItem::New()
{
  return new ( gRenderItemPool.AllocateRaw() ) RenderItem();
}

RenderItem::RenderItem()
: mModelMatrix( false ),
  mModelViewMatrix( false ),
  mRenderer( NULL ),
  mNode( NULL ),
  mDepthIndex( 0 ),
  mIsOpaque( true ),
  mSkipIfBatched( false ),
  mBatchUpdated( false ),
  mBatchRenderGeometry( NULL ),
  mBatchVertexBuffer( NULL ),
  mBatchIndexBuffer( NULL )
{
}

RenderItem::~RenderItem()
{
}

void RenderItem::operator delete( void* ptr )
{
  gRenderItemPool.Free( static_cast<RenderItem*>( ptr ) );
}

void RenderItem::Reset()
{
  mBatchRenderGeometry = NULL;
  mBatchVertexBuffer = NULL;
  mBatchIndexBuffer = NULL;
  mRenderer = NULL;
}

void RenderItem::SetRenderer( Render::Renderer* renderer )
{
  mRenderer = renderer;
}

void RenderItem::SetBatchGeometry( SceneGraph::RenderGeometry* geometry )
{
  mBatchRenderGeometry = geometry;
}

SceneGraph::RenderGeometry* RenderItem::GetBatchGeometry() const
{
  return mBatchRenderGeometry;
}

void RenderItem::SetNode( Node* node )
{
  mNode = node;
}

Render::Renderer& RenderItem::GetRenderer() const
{
  return *mRenderer;
}

Matrix& RenderItem::GetModelMatrix()
{
  return mModelMatrix;
}

const Matrix& RenderItem::GetModelMatrix() const
{
  return mModelMatrix;
}

Matrix& RenderItem::GetModelViewMatrix()
{
  return mModelViewMatrix;
}

const Matrix& RenderItem::GetModelViewMatrix() const
{
  return mModelViewMatrix;
}

void RenderItem::SetDepthIndex( int depthIndex )
{
  mDepthIndex = depthIndex;
}

void RenderItem::SetIsOpaque( bool isOpaque )
{
  mIsOpaque = isOpaque;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
