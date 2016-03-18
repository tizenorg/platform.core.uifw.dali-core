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
//todor
#include <iostream>

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
  //todor test
  return new ( gRenderItemPool.AllocateRaw() ) RenderItem();
  RenderItem* ri = new ( gRenderItemPool.AllocateRaw() ) RenderItem();
  std::cout << "todor: ...................................................... STATIC-NEW:   " << (void *)( ri ) << std::endl;
}

RenderItem::RenderItem()
: mModelViewMatrix( false ),
  mRenderer( NULL ),
  mNode( NULL ),
  mClippingMode( Dali::Renderer::CLIPPING_DISABLED ),
  mClippingId( -1 ),
  mDepthIndex( 0 ),
  mIsOpaque( true ),
  mVisitedId( 0 )
{
  std::cout << "todor: ...................................................... CONSTRUCTING: " << (void *)( this ) << std::endl;
  mName2 = "ghjkl";//todor
}

RenderItem::~RenderItem()
{
  std::cout << "todor: ...................................................... @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ DESTRUCTING:  " << (void *)( this ) << std::endl;
}

void RenderItem::operator delete( void* ptr )
{
  std::cout << "todor: ...................................................... @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ DELETING:     " << (void *)( ptr ) << std::endl;
  gRenderItemPool.Free( static_cast<RenderItem*>( ptr ) );
  std::cout << "todor: ...................................................... DELETING:     DONE" << std::endl;
}

void RenderItem::Reset()
{
  std::cout << "todor: ...................................................... RESETTING:    " << (void *)( this ) << std::endl;
  mRenderer = NULL;
}

void RenderItem::SetRenderer( Render::Renderer* renderer )
{
  mRenderer = renderer;
}

void RenderItem::SetNode( Node* node )
{
  mNode = node;
}

Render::Renderer& RenderItem::GetRenderer() const
{
  return *mRenderer;
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
