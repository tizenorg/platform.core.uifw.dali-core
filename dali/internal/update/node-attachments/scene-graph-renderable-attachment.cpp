/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>

// INTERNAL INCLUDES
#include <dali/integration-api/resource-declarations.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/internal/update/manager/prepare-render-instructions.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/common/image-sampler.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderableAttachment::RenderableAttachment( bool usesGeometryScaling )
: mSceneController(NULL), //@todo MESH_REWORK Pass in where required rather than store
  mShader( NULL ),
  mSortModifier( 0.0f ),
  mBlendingMode( Dali::ImageActor::DEFAULT_BLENDING_MODE ),
  mUsesGeometryScaling( usesGeometryScaling ),
  mScaleForSizeDirty( true ),
  mHasSizeAndColorFlag( false ),
  mResourcesReady( false ),
  mFinishedResourceAcquisition( false )
{
}

RenderableAttachment::~RenderableAttachment()
{
}

void RenderableAttachment::Initialize( SceneController& sceneController, BufferIndex updateBufferIndex )
{
  mSceneController = &sceneController;

  // Chain to derived attachments
  Initialize2( updateBufferIndex );
}

void RenderableAttachment::OnDestroy()
{
  // Chain to derived attachments
  OnDestroy2();

  // SceneController is no longer valid
  mSceneController = NULL;
}

void RenderableAttachment::SetRecalculateScaleForSize()
{
  mScaleForSizeDirty = true;
}

void RenderableAttachment::GetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  DoGetScaleForSize( nodeSize, scaling );
  mScaleForSizeDirty = false;
}

bool RenderableAttachment::ResolveVisibility( BufferIndex updateBufferIndex )
{
  mHasSizeAndColorFlag = mParent->ResolveVisibility(updateBufferIndex);
  return mHasSizeAndColorFlag;
}

void RenderableAttachment::DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  scaling = Vector3::ONE;
}

void RenderableAttachment::PrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  if( mShader )
  {
    Integration::ResourceId id = mShader->GetEffectTextureResourceId();

    if( id != 0 )
    { // TODO this code is dead and to be removed soon
      mFinishedResourceAcquisition = false;
      mResourcesReady = false;
    }
  }

  mResourcesReady = DoPrepareResources( updateBufferIndex, resourceManager );
}

void RenderableAttachment::GetReadyAndComplete(bool& ready, bool& complete) const
{
  ready = mResourcesReady;
  complete = mFinishedResourceAcquisition;
}

RenderableAttachment* RenderableAttachment::GetRenderable()
{
  return this;
}

void RenderableAttachment::SetSortModifier(float modifier)
{
  // Setting sort modifier makes the node dirty, i.e. we cannot reuse previous frames render items
  if( mParent )
  {
    // only do this if we are on-stage
    mParent->SetDirtyFlag( SortModifierFlag );
  }
  mSortModifier = modifier;
}

void RenderableAttachment::SetSortAttributes( BufferIndex bufferIndex, RendererWithSortAttributes& sortAttributes )
{
  sortAttributes.shader = mShader;
  sortAttributes.textureResourceId = Integration::InvalidResourceId;
  sortAttributes.geometry = NULL;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
