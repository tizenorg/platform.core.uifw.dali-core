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

// CLASS HEADER
#include "scene-graph-renderer2.h"

// INTERNAL HEADERS
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-material.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/resources/resource-tracker.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/common/internal-constants.h>


namespace // unnamed namespace
{

const unsigned int UNIFORM_MAP_READY      = 0;
const unsigned int COPY_UNIFORM_MAP       = 1;
const unsigned int REGENERATE_UNIFORM_MAP = 2;

void AddMappings( Dali::Internal::SceneGraph::CollectedUniformMap& localMap, const Dali::Internal::SceneGraph::UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  // Any maps that aren't in localMap should be added in a single step
  Dali::Internal::SceneGraph::CollectedUniformMap newUniformMappings;

  unsigned int uniformMapCount( uniformMap.Count() );
  unsigned int localMapCount( localMap.Count() );
  for( unsigned int i(0); i<uniformMapCount; ++i )
  {
    bool found( false );
    for( unsigned int j(0); j<localMapCount; ++j )
    {
      if( localMap[i]->uniformNameHash == uniformMap[i].uniformNameHash )
      {
        if( localMap[i]->uniformName == uniformMap[i].uniformName )
        {
          found = true;
          break;
        }
      }
    }
    if( !found )
    {
      // it's a new mapping. Add raw ptr to temporary list
      newUniformMappings.PushBack( &uniformMap[i] );
    }
  }

  if( newUniformMappings.Count() > 0 )
  {
    localMap.Resize( localMap.Count() + newUniformMappings.Count() );

    unsigned int newUniformMappingsCount( newUniformMappings.Size() );
    for( unsigned int i(0); i<newUniformMappingsCount; ++i )
    {
      localMap[i] = newUniformMappings[i];
    }
    //@todo MESH_REWORK Use memcpy to copy ptrs from one array to the other
  }
}
}

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

R3nderer::R3nderer()
:mSceneController(0),
 mRenderer(NULL),
 mMaterial(NULL),
 mGeometry(NULL),
 mReferenceCount(0),
 mRegenerateUniformMap(0),
 mResendDataProviders(false),
 mResendGeometry(false),
 mDepthIndex(0)
{
  mUniformMapChanged[0]=false;
  mUniformMapChanged[1]=false;

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}

R3nderer::~R3nderer()
{
  if (mMaterial)
  {
    mMaterial->RemoveConnectionObserver(*this);
    mMaterial=NULL;
  }
  if (mGeometry)
  {
    mGeometry->RemoveConnectionObserver(*this);
    mGeometry=NULL;
  }
}

void R3nderer::PrepareRender( BufferIndex updateBufferIndex )
{
  mMaterial->PrepareRender( updateBufferIndex );

  if( mRegenerateUniformMap > UNIFORM_MAP_READY )
  {
    DALI_ASSERT_DEBUG( mGeometry != NULL && "No geometry available in DoPrepareRender()" );
    DALI_ASSERT_DEBUG( mMaterial != NULL && "No geometry available in DoPrepareRender()" );

    if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP)
    {
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      localMap.Resize(0);

      const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
      AddMappings( localMap, rendererUniformMap );

      AddMappings( localMap, mMaterial->GetUniformMap() );
      Vector<Sampler*>& samplers = mMaterial->GetSamplers();
      unsigned int samplerCount( samplers.Size() );
      for( unsigned int i(0); i<samplerCount; ++i )
      {
        AddMappings( localMap, samplers[i]->GetUniformMap() );
      }

      AddMappings( localMap, mMaterial->GetShader()->GetUniformMap() );
      AddMappings( localMap, mGeometry->GetUniformMap() );

      Vector<PropertyBuffer*>& vertexBuffers = mGeometry->GetVertexBuffers();
      unsigned int vertexBufferCount( vertexBuffers.Size() );
      for( unsigned int i(0); i<vertexBufferCount; ++i )
      {
        AddMappings( localMap, vertexBuffers[i]->GetUniformMap() );
      }

      PropertyBuffer* indexBuffer = mGeometry->GetIndexBuffer();
      if( indexBuffer )
      {
        AddMappings( localMap, indexBuffer->GetUniformMap() );
      }
    }
    else if( mRegenerateUniformMap == COPY_UNIFORM_MAP )
    {
      // Copy old map into current map
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-updateBufferIndex ];

      localMap.Resize( oldMap.Count() );

      unsigned int index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }

    mUniformMapChanged[updateBufferIndex] = true;
    mRegenerateUniformMap--;
  }

  if( mResendDataProviders )
  {
    RenderDataProvider* dataProvider = NewRenderDataProvider();

    // Tell renderer about a new provider
    // @todo MESH_REWORK Should we instead create a new renderer when these change?

    typedef MessageValue1< NewRenderer, OwnerPointer<RenderDataProvider> > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetRenderDataProvider, dataProvider );
    mResendDataProviders = false;
  }

  if( mResendGeometry )
  {
    // The first call to GetRenderGeometry() creates the geometry and sends it in a message
    RenderGeometry* geometry = mGeometry->GetRenderGeometry( mSceneController );

    typedef MessageValue1< NewRenderer, RenderGeometry* > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    new (slot) DerivedType( mRenderer, &NewRenderer::SetGeometry, geometry );
    mResendGeometry = false;
  }
}

void R3nderer::SetMaterial( BufferIndex bufferIndex, Material* material)
{
  DALI_ASSERT_DEBUG( material != NULL && "Material pointer is NULL" );

  mMaterial = material;
  mMaterial->AddConnectionObserver( *this );
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  mResendDataProviders = true;
}

void R3nderer::SetGeometry( BufferIndex bufferIndex, Geometry* geometry)
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");
  if( mGeometry)
  {
    mGeometry->RemoveConnectionObserver(*this);
    mGeometry->OnRendererDisconnect();
  }

  mGeometry = geometry;
  mGeometry->AddConnectionObserver( *this ); // Observe geometry connections / uniform mapping changes
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  if( mRenderer )
  {
    mResendGeometry = true;
  }
}

void R3nderer::SetDepthIndex( int depthIndex )
{
  mDepthIndex = depthIndex;
}

//Called when a node with this renderer is added to the stage
void R3nderer::OnStageConnect()
{
  ++mReferenceCount;
  if( !mRenderer)
  {
    RenderDataProvider* dataProvider = NewRenderDataProvider();

    RenderGeometry* renderGeometry = mGeometry->GetRenderGeometry(mSceneController);
    mRenderer = NewRenderer::New( dataProvider, renderGeometry );
    mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
    mResendDataProviders = false;
    mResendGeometry = false;
  }
}

//Called when the node with this renderer has gone out of the stage
void R3nderer::OnStageDisconnect()
{
  --mReferenceCount;
  if( mReferenceCount == 0 )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = NULL;
  }
}

//Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void R3nderer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
  mSceneController = &sceneController;
}


//Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void R3nderer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  //Remove renderer from RenderManager
  if( mRenderer )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = NULL;
    mSceneController = NULL;
  }
}

RenderDataProvider* R3nderer::NewRenderDataProvider()
{
  RenderDataProvider* dataProvider = new RenderDataProvider();

  dataProvider->mMaterialDataProvider = mMaterial;
  dataProvider->mUniformMapDataProvider = this;
  dataProvider->mShader = mMaterial->GetShader();

  Vector<Sampler*>& samplers = mMaterial->GetSamplers();
  unsigned int sampleCount( samplers.Count() );
  dataProvider->mSamplers.Resize( sampleCount );
  for( unsigned int i(0); i<sampleCount; ++i )
  {
    dataProvider->mSamplers[i] = samplers[i]; // Convert from derived type to base type
  }

  return dataProvider;
}

Renderer& R3nderer::GetRenderer()
{
  return *mRenderer;
}

const CollectedUniformMap& R3nderer::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mCollectedUniformMap[bufferIndex];
};

void R3nderer::PrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  //@TODO:FERRAN Implement
  mTrackedResources.Clear(); // Resource trackers are only needed if not yet completea
  DALI_ASSERT_DEBUG( mSceneController );

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();
  mResourcesReady = false;
  mHasUntrackedResources = false;

  mFinishedResourceAcquisition = false;

  // Can only be considered ready when all the scene graph objects are connected to the renderer
  if( ( mGeometry ) && ( mGeometry->GetVertexBuffers().Count() > 0 ) &&
      ( mMaterial ) && ( mMaterial->GetShader() != NULL ) )
  {
    unsigned int completeCount = 0;
    unsigned int neverCount = 0;
    unsigned int frameBufferCount = 0;

    Vector<Sampler*>& samplers = mMaterial->GetSamplers();
    unsigned int samplerCount( samplers.Size() );
    for( unsigned int i(0); i<samplerCount; ++i )
    {
      ResourceId textureId = samplers[i]->GetTextureId( updateBufferIndex );
      BitmapMetadata metaData = resourceManager.GetBitmapMetadata( textureId );

      samplers[i]->SetFullyOpaque( metaData.IsFullyOpaque() );

      switch( completeStatusManager.GetStatus( textureId ) )
      {
        case CompleteStatusManager::NOT_READY:
        {
          if( metaData.GetIsFramebuffer() )
          {
            frameBufferCount++;
          }

          if( completeStatusManager.FindResourceTracker(textureId) != NULL )
          {
            bool found = false;
            std::size_t numTrackedResources = mTrackedResources.Count();
            for( size_t i=0; i < numTrackedResources; ++i )
            {
              if(mTrackedResources[i] == textureId)
              {
                found = true;
                break;
              }
            }
            if( ! found )
            {
              mTrackedResources.PushBack( textureId );
            }
          }
          else
          {
            mHasUntrackedResources = true;
          }
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
    // We are complete if all samplers are either complete or will nmResendGeometryever complete
    mResourcesReady = ( completeCount + frameBufferCount >= samplers.Count() ) ;
    mFinishedResourceAcquisition = ( completeCount + neverCount >= samplers.Count() );
  }
}

void R3nderer::GetReadyAndComplete(bool& ready, bool& complete) const
{
  ready = mResourcesReady;
  complete = false;

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  std::size_t numTrackedResources = mTrackedResources.Count();
  if( mHasUntrackedResources || numTrackedResources == 0 )
  {
    complete = mFinishedResourceAcquisition;
  }
  else
  {
    // If there are tracked resources and no untracked resources, test the trackers
    bool trackersComplete = true;
    for( size_t i=0; i < numTrackedResources; ++i )
    {
      ResourceId id = mTrackedResources[i];
      ResourceTracker* tracker = completeStatusManager.FindResourceTracker(id);
      if( tracker  && ! tracker->IsComplete() )
      {
        trackersComplete = false;
        break;
      }
    }

    complete = mFinishedResourceAcquisition || trackersComplete;
  }
}

// Called by ProcessRenderTasks after DoPrepareRender
bool R3nderer::IsFullyOpaque( BufferIndex updateBufferIndex, const Node& node ) const
{
  bool opaque = false;

  if( mMaterial != NULL )
  {
    Material::BlendPolicy blendPolicy = mMaterial->GetBlendPolicy();
    switch( blendPolicy )
    {
      case Material::OPAQUE:
      {
        opaque = true;
        break;
      }
      case Material::TRANSPARENT:
      {
        opaque = false;
        break;
      }
      case Material::USE_ACTOR_COLOR:
      {
        opaque = node.GetWorldColor( updateBufferIndex ).a >= FULLY_OPAQUE;
        break;
      }
    }
  }

  return opaque;
}

void R3nderer::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  // Ensure the child object pointers get re-sent to the renderer
  mResendDataProviders = true;
}

void R3nderer::ConnectedUniformMapChanged()
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void R3nderer::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are either from PropertyOwner base class, or the Actor
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
