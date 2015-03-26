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
 */

#include "scene-graph-renderer-attachment.h"
#include <dali/internal/update/effects/scene-graph-material.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>

#if defined(DEBUG_ENABLED)
Debug::Filter* gImageAttachmentLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SCENE_GRAPH_IMAGE_ATTACHMENT");

#define ATTACHMENT_LOG(level)                                                 \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p\n", __FUNCTION__, this)
#define ATTACHMENT_LOG_FMT(level, format, args...) \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p " format, __FUNCTION__, this, ## args)

#else

#define ATTACHMENT_LOG(level)
#define ATTACHMENT_LOG_FMT(level, format, args...)

#endif


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RendererAttachment* RendererAttachment::New()
{
  return new RendererAttachment();
}


RendererAttachment::RendererAttachment()
: RenderableAttachment( false ),
  mRenderer(NULL),
  mMaterial(NULL),
  mGeometry(NULL),
  mDepthIndex(0),
  mRegenerateUniformMap(true)
{
  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}


RendererAttachment::~RendererAttachment()
{
  mMaterial->RemoveConnectionObserver(*this);
  mGeometry->RemoveConnectionObserver(*this);

  mMaterial=NULL;
  mGeometry=NULL;
}

void RendererAttachment::Initialize2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mSceneController );

  mRenderer = NewRenderer::New( *mParent, *this, mGeometry, mMaterial );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
}

void RendererAttachment::OnDestroy2()
{
  DALI_ASSERT_DEBUG( mSceneController );
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
  mRenderer = NULL;
}

void RendererAttachment::ConnectedToSceneGraph()
{
  mRegenerateUniformMap = true;
  mParent->AddUniformMapObserver( *this );

  // @todo MESH_REWORK - Create renderer only when staged (and have all connected objects?)
}

void RendererAttachment::DisconnectedFromSceneGraph()
{
  mRegenerateUniformMap = false;
  mParent->RemoveUniformMapObserver( *this );

  // @todo MESH_REWORK - Destroy renderer when off stage?
}

void RendererAttachment::SetMaterial( BufferIndex updateBufferIndex, const Material* material)
{
  DALI_ASSERT_DEBUG( material != NULL && "Material pointer is NULL" );

  mMaterial = const_cast<Material*>(material); // Need this to be non-const to add observer only.
  mMaterial->AddConnectionObserver( *this );
  mRegenerateUniformMap = true;

  // Tell renderer about a new provider
  if( mRenderer )
  {
    typedef MessageValue1< NewRenderer, const MaterialDataProvider*> DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetMaterialDataProvider, material );
  }
}

const Material& RendererAttachment::GetMaterial() const
{
  return *mMaterial;
}

void RendererAttachment::SetGeometry( BufferIndex updateBufferIndex, const Geometry* geometry)
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");

  mGeometry = const_cast<Geometry*>(geometry); // Need this to be non-const to add observer only
  mGeometry->AddConnectionObserver( *this ); // Observe geometry connections / uniform mapping changes
  mRegenerateUniformMap = true;

  // Tell renderer about a new provider
  if( mRenderer )
  {
    typedef MessageValue1< NewRenderer, const GeometryDataProvider*> DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetGeometryDataProvider, geometry );
  }
}

const Geometry& RendererAttachment::GetGeometry() const
{
  return *mGeometry;
}

void RendererAttachment::SetDepthIndex( int index )
{
  // @todo MESH_REWORK
  mDepthIndex = index;
}

int RendererAttachment::GetDepthIndex() const
{
  return mDepthIndex;
}


Renderer& RendererAttachment::GetRenderer()
{
  return *mRenderer;
}

const Renderer& RendererAttachment::GetRenderer() const
{
  return *mRenderer;
}

// Uniform maps are checked in the following priority order:
//   Actor
//     Renderer (this object)
//       Geometry
//         VertexBuffers
//       Material
//         Samplers
//         Shader
void RendererAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
  mUniformMapChanged[0] = false;
  mUniformMapChanged[1] = false;

  // @todo MESH_REWORK UNIFORM_MAP re-generate uniform maps if they have changed
  if( mRegenerateUniformMap )
  {
    DALI_ASSERT_DEBUG( mGeometry != NULL && "No geometry available in DoPrepareRender()" );
    DALI_ASSERT_DEBUG( mMaterial != NULL && "No geometry available in DoPrepareRender()" );

    UniformMap& localMap = mUniformMap[ updateBufferIndex ];
    localMap.Reset();

    const UniformMap& actorUniformMap = mParent->GetUniformMap();
    localMap.AddMappings( actorUniformMap );
    const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
    localMap.AddMappings( rendererUniformMap );
    localMap.AddMappings( mGeometry->GetUniformMap() );

    const GeometryDataProvider::VertexBuffers& vertexBuffers = mGeometry->GetVertexBuffers();
    for( GeometryDataProvider::VertexBuffers::ConstIterator iter = vertexBuffers.Begin(), end = vertexBuffers.End() ;
         iter != end ;
         ++iter )
    {
      localMap.AddMappings( (*iter)->GetUniformMap() );
    }

    localMap.AddMappings( mMaterial->GetUniformMap() );
    const MaterialDataProvider::Samplers& samplers = mMaterial->GetSamplers();
    for( MaterialDataProvider::Samplers::ConstIterator iter = samplers.Begin(), end = samplers.End();
         iter != end ;
         ++iter )
    {
      const SceneGraph::Sampler* sampler = static_cast<const SceneGraph::Sampler*>( *iter );
      localMap.AddMappings( sampler->GetUniformMap() );
    }

    localMap.AddMappings( mMaterial->GetShader()->GetUniformMap() );

    mUniformMapChanged[updateBufferIndex] = true;
  }
  mRegenerateUniformMap = false;
}

bool RendererAttachment::IsFullyOpaque( BufferIndex updateBufferIndex )
{
  bool opaque = false;

  if( mParent )
  {
    opaque = mParent->GetWorldColor( updateBufferIndex ).a >= FULLY_OPAQUE;
  }

  if( mMaterial != NULL )
  {
    // Require that all affecting samplers are opaque
    unsigned int opaqueCount=0;
    unsigned int affectingCount=0;

    const Material::Samplers& samplers = mMaterial->GetSamplers();
    for( Material::Samplers::ConstIterator iter = samplers.Begin();
         iter != samplers.End(); ++iter )
    {
      const Sampler* sampler = static_cast<const Sampler*>(*iter);
      if( sampler != NULL )
      {
        if( sampler->AffectsTransparency( updateBufferIndex ) )
        {
          affectingCount++;
          if( sampler->IsFullyOpaque( updateBufferIndex ) )
          {
            opaqueCount++;
          }
        }
      }
    }
    opaque = (opaqueCount == affectingCount);
  }

  return opaque;
}

void RendererAttachment::SizeChanged( BufferIndex updateBufferIndex )
{
  // Do nothing.
}

bool RendererAttachment::DoPrepareResources(
  BufferIndex updateBufferIndex,
  ResourceManager& resourceManager )
{
  DALI_ASSERT_DEBUG( mSceneController );

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  bool ready = false;
  mFinishedResourceAcquisition = false;

  // Can only be considered ready when all the scene graph objects are connected to the renderer
  if( ( mGeometry ) &&
      ( mGeometry->GetVertexBuffers().Count() > 0 ) &&
      ( mMaterial ) &&
      ( mMaterial->GetShader() != NULL ) )
  {
    unsigned int completeCount = 0;
    unsigned int neverCount = 0;
    unsigned int frameBufferCount = 0;

    const Material::Samplers& samplers = mMaterial->GetSamplers();
    for( Material::Samplers::ConstIterator iter = samplers.Begin();
         iter != samplers.End(); ++iter )
    {
      const Sampler* sampler = static_cast<const Sampler*>(*iter);

      ResourceId textureId = sampler->GetTextureId( updateBufferIndex );
      switch( completeStatusManager.GetStatus( textureId ) )
      {
        case CompleteStatusManager::NOT_READY:
        {
          ready = false;
          BitmapMetadata metaData = resourceManager.GetBitmapMetadata( textureId );
          if( metaData.GetIsFramebuffer() )
          {
            frameBufferCount++;
          }
          FollowTracker( textureId ); // @todo MESH_REWORK Trackers per sampler rather than per actor?
        }
        break;

        case CompleteStatusManager::COMPLETE:
        {
          completeCount++;
        }
        break;

        case CompleteStatusManager::NEVER:
        {
          neverCount++;
        }
        break;
      }
    }

    // We are ready if all samplers are complete, or those that aren't are framebuffers
    // We are complete if all samplers are either complete or will never complete

    ready = ( completeCount + frameBufferCount >= samplers.Count() ) ;
    mFinishedResourceAcquisition = ( completeCount + neverCount >= samplers.Count() );
  }

  return ready;
}

void RendererAttachment::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = true;
}

void RendererAttachment::ConnectedUniformMapChanged()
{
  mRegenerateUniformMap = true;
}

void RendererAttachment::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are either from PropertyOwner base class, or the Actor
  mRegenerateUniformMap = true;
}

bool RendererAttachment::GetUniformMapChanged( BufferIndex bufferIndex ) const
{
  return mUniformMapChanged[bufferIndex];
}

const UniformMap& RendererAttachment::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mUniformMap[ bufferIndex ];
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
