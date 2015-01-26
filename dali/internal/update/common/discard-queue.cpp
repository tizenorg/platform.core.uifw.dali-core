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
#include <dali/internal/update/common/discard-queue.h>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/render/renderers/scene-graph-image-renderer.h>
#include <dali/internal/render/renderers/scene-graph-text-renderer.h>
#include <dali/internal/render/renderers/scene-graph-mesh-renderer.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace // unnamed namespace
{

static void DoGlCleanup( BufferIndex updateBufferIndex, GlResourceOwner& owner, RenderQueue& renderQueue )
{
  typedef Message< GlResourceOwner > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = renderQueue.ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &owner, &GlResourceOwner::GlCleanup );
}

} // unnamed namespace

DiscardQueue::DiscardQueue( RenderQueue& renderQueue )
: mRenderQueue( renderQueue )
{
}

DiscardQueue::~DiscardQueue()
{
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Node* node )
{
  DALI_ASSERT_DEBUG( NULL != node );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mNodeQueue0.PushBack( node );
  }
  else
  {
    mNodeQueue1.PushBack( node );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, NodeAttachment* attachment )
{
  DALI_ASSERT_DEBUG( NULL != attachment );

  // The GL resources will now be freed in Render frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the attachment for destruction in Update frame N+2
  if ( 0u == updateBufferIndex )
  {
    mAttachmentQueue0.PushBack( attachment );
  }
  else
  {
    mAttachmentQueue1.PushBack( attachment );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Mesh* mesh )
{
  DALI_ASSERT_DEBUG( mesh );

  // Send message to clean-up GL resources in the next Render
  DoGlCleanup( updateBufferIndex, *mesh, mRenderQueue );

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mMeshQueue0.PushBack( mesh );
  }
  else
  {
    mMeshQueue1.PushBack( mesh );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, Shader* shader )
{
  DALI_ASSERT_DEBUG( NULL != shader );

  // Programs are cached for the lifetime of DALi so no need for GL cleanup for shader for now.

  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mShaderQueue0.PushBack( shader );
  }
  else
  {
    mShaderQueue1.PushBack( shader );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, ImageRenderer* renderer )
{
  DALI_ASSERT_DEBUG( renderer );

  // GL resources have been requested to be cleaned up in RenderManager::RemoveRenderer
  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mImageRendererQueue0.PushBack( renderer );
  }
  else
  {
    mImageRendererQueue1.PushBack( renderer );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, TextRenderer* renderer )
{
  DALI_ASSERT_DEBUG( renderer );

  // GL resources have been requested to be cleaned up in RenderManager::RemoveRenderer
  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mTextRendererQueue0.PushBack( renderer );
  }
  else
  {
    mTextRendererQueue1.PushBack( renderer );
  }
}

void DiscardQueue::Add( BufferIndex updateBufferIndex, MeshRenderer* renderer )
{
  DALI_ASSERT_DEBUG( renderer );

  // GL resources have been requested to be cleaned up in RenderManager::RemoveRenderer
  // The GL resources will now be freed in frame N
  // The Update for frame N+1 may occur in parallel with the rendering of frame N
  // Queue the node for destruction in frame N+2
  if ( 0u == updateBufferIndex )
  {
    mMeshRendererQueue0.PushBack( renderer );
  }
  else
  {
    mMeshRendererQueue1.PushBack( renderer );
  }
}

DiscardQueue::ImageRendererQueue& DiscardQueue::GetImageRendererQueue( BufferIndex updateBufferIndex )
{
  return ( 0u == updateBufferIndex ) ? mImageRendererQueue0 : mImageRendererQueue1;
}

DiscardQueue::TextRendererQueue& DiscardQueue::GetTextRendererQueue( BufferIndex updateBufferIndex )
{
  return ( 0u == updateBufferIndex ) ? mTextRendererQueue0 : mTextRendererQueue1;
}

DiscardQueue::MeshRendererQueue& DiscardQueue::GetMeshRendererQueue( BufferIndex updateBufferIndex )
{
  return ( 0u == updateBufferIndex ) ? mMeshRendererQueue0 : mMeshRendererQueue1;
}

void DiscardQueue::Clear( BufferIndex updateBufferIndex )
{
  // Destroy some discarded objects; these should no longer own any GL resources

  if ( 0u == updateBufferIndex )
  {
    mNodeQueue0.Clear();
    mAttachmentQueue0.Clear();
    mMeshQueue0.Clear();
    mShaderQueue0.Clear();
    mImageRendererQueue0.Clear();  // Will not delete pointed to objects
    mTextRendererQueue0.Clear();   // Will not delete pointed to objects
    mMeshRendererQueue0.Clear();   // Will not delete pointed to objects
  }
  else
  {
    mNodeQueue1.Clear();
    mAttachmentQueue1.Clear();
    mMeshQueue1.Clear();
    mShaderQueue1.Clear();
    mImageRendererQueue1.Clear();  // Will not delete pointed to objects
    mTextRendererQueue1.Clear();   // Will not delete pointed to objects
    mMeshRendererQueue1.Clear();   // Will not delete pointed to objects
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
