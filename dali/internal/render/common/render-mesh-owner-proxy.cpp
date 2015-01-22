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
#include <dali/internal/render/common/render-mesh-owner-proxy.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-mesh.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderMeshOwnerProxy::RenderMeshOwnerProxy(
    RenderQueue& renderQueue,
    RenderMeshOwner& renderMeshOwner )
: mRenderQueue( renderQueue ),
  mRenderMeshOwner( renderMeshOwner )
{
}

void RenderMeshOwnerProxy::AddRenderMesh( BufferIndex updateBufferIndex, RenderMesh* renderMesh )
{
  // Define the type of message
  typedef  MessageValue1< RenderMeshOwner, OwnerPointer<RenderMesh> > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderMeshOwner, &RenderMeshOwner::AddRenderMesh, renderMesh );
}

/**
 * @brief Dispatch a message to the RenderMeshOwner to add a mesh
 */
void RenderMeshOwnerProxy::RemoveRenderMesh( BufferIndex updateBufferIndex, RenderMesh* renderMesh )
{
  // Define the type of message
  typedef  MessageValue1< RenderMeshOwner, OwnerPointer<RenderMesh> > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue.ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( &mRenderMeshOwner, &RenderMeshOwner::RemoveRenderMesh, renderMesh );
}

} // SceneGraph

} // Internal

} // Dali
