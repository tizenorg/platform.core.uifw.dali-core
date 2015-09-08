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
const int REGENERATE_UNIFORM_MAP = 2;
const int COPY_UNIFORM_MAP       = 1;
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
 mUseBlend(false),
 mDepthIndex(0)
{
  std::cout<<"Creating update thread renderer (SceneGraph::Renderer)"<<std::endl;
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
  std::cout<<"Destroying update thread renderer (SceneGraph::Renderer)"<<std::endl;
}

void R3nderer::AddMappings( CollectedUniformMap& localMap, const UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in localMap should be added in a single step
  CollectedUniformMap newUniformMappings;

  for( unsigned int i=0, count=uniformMap.Count(); i<count; ++i )
  {
    UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( CollectedUniformMap::Iterator iter = localMap.Begin() ; iter != localMap.End() ; ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
      if( map->uniformNameHash == nameHash )
      {
        if( map->uniformName == uniformMap[i].uniformName )
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
    localMap.Reserve( localMap.Count() + newUniformMappings.Count() );

    for( CollectedUniformMap::Iterator iter = newUniformMappings.Begin(),
           end = newUniformMappings.End() ;
         iter != end ;
         ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
      localMap.PushBack( map );
    }
    //@todo MESH_REWORK Use memcpy to copy ptrs from one array to the other
  }
}
void R3nderer::PrepareRender( BufferIndex updateBufferIndex, Node* node )
{
  mMaterial->PrepareRender( updateBufferIndex );

  DALI_ASSERT_DEBUG( mGeometry != NULL && "No geometry available in DoPrepareRender()" );
  DALI_ASSERT_DEBUG( mMaterial != NULL && "No geometry available in DoPrepareRender()" );

  CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
  localMap.Resize(0);

  const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
  AddMappings( localMap, rendererUniformMap );

  //@TODO:FERRAN Node mappings?? const UniformMap& actorUniformMap = mParent->GetUniformMap();
  //AddMappings( localMap, actorUniformMap );

  AddMappings( localMap, mMaterial->GetUniformMap() );
  Vector<Sampler*>& samplers = mMaterial->GetSamplers();
  for( Vector<Sampler*>::ConstIterator iter = samplers.Begin(), end = samplers.End();
      iter != end ;
      ++iter )
  {
    const SceneGraph::Sampler* sampler = (*iter);
    AddMappings( localMap, sampler->GetUniformMap() );
  }

  AddMappings( localMap, mMaterial->GetShader()->GetUniformMap() );

  AddMappings( localMap, mGeometry->GetUniformMap() );

  Vector<PropertyBuffer*>& vertexBuffers = mGeometry->GetVertexBuffers();
  for( Vector<PropertyBuffer*>::ConstIterator iter = vertexBuffers.Begin(), end = vertexBuffers.End() ;
      iter != end ;
      ++iter )
  {
    const SceneGraph::PropertyBuffer* vertexBuffer = *iter;
    AddMappings( localMap, vertexBuffer->GetUniformMap() );
  }

  PropertyBuffer* indexBuffer = mGeometry->GetIndexBuffer();
  if( indexBuffer )
  {
    AddMappings( localMap, indexBuffer->GetUniformMap() );
  }

  mUniformMapChanged[updateBufferIndex] = true;

//  bool blend = NeedsBlending( updateBufferIndex, node );
//  if( mUseBlend != blend )
//  {
//    mUseBlend = blend;
//    mResendDataProviders = true;
//  }

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

//  //@TODO:FERRAN: Instead of setting flag into the actor, I think the correct thing to do here is notify update manager that the scene needs to be updated
//  if( mParent )
//  {
//    // only do this if we are on-stage. Ensures the render lists are re-sorted
//    mParent->SetDirtyFlag( SortModifierFlag );
//  }
}

//Called when a node with this renderer is added to the stage
void R3nderer::OnStageConnect(Node* node)
{
  //std::cout<<"R3nderer::OnStageConnect"<<std::endl;

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
void R3nderer::OnStageDisconnect(Node* node)
{
  --mReferenceCount;
  if( mReferenceCount == 0 )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = NULL;
  }

//  std::cout<<"R3nderer::OnStageDisconnect"<<std::endl;
}

//Called when SceneGraph::Renderer is added to update manager ( that happens when an "event-thread renderer" is created )
void R3nderer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  mSceneController = &sceneController;
}


//Called just before destroying the scene-graph renderer ( when the "event-thread renderer" is no longer referenced )
void R3nderer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  //Remove renderer from RenderManager
  std::cout<<"R3nderer::DisconnectFromSceneGraph"<<std::endl;

    if( mRenderer )
    {
      mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
      mRenderer = NULL;
    }
}

RenderDataProvider* R3nderer::NewRenderDataProvider()
{
  RenderDataProvider* dataProvider = new RenderDataProvider();

  dataProvider->mMaterialDataProvider = mMaterial;
  dataProvider->mUniformMapDataProvider = this;
  dataProvider->mShader = mMaterial->GetShader();
  dataProvider->mUseBlend = mUseBlend;

  Vector<Sampler*>& samplers = mMaterial->GetSamplers();
  dataProvider->mSamplers.Reserve( samplers.Count() );
  for( Vector<Sampler*>::Iterator iter = samplers.Begin() ;
       iter != samplers.End();
       ++iter )
  {
    dataProvider->mSamplers.PushBack(*iter); // Convert from derived type to base type
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
  if( ( mGeometry ) &&
      ( mGeometry->GetVertexBuffers().Count() > 0 ) &&
      ( mMaterial ) &&
      ( mMaterial->GetShader() != NULL ) )
  {
    unsigned int completeCount = 0;
    unsigned int neverCount = 0;
    unsigned int frameBufferCount = 0;

    Vector<Sampler*>& samplers = mMaterial->GetSamplers();
    for( Vector<Sampler*>::ConstIterator iter = samplers.Begin();
        iter != samplers.End(); ++iter )
    {
      Sampler* sampler = *iter;

      ResourceId textureId = sampler->GetTextureId( updateBufferIndex );
      BitmapMetadata metaData = resourceManager.GetBitmapMetadata( textureId );

      sampler->SetFullyOpaque( metaData.IsFullyOpaque() );

      switch( completeStatusManager.GetStatus( textureId ) )
      {
        case CompleteStatusManager::NOT_READY:
        {
          if( metaData.GetIsFramebuffer() )
          {
            frameBufferCount++;
          }
          /****/
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
          /****/
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
bool R3nderer::IsFullyOpaque( BufferIndex updateBufferIndex, Node* node )
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
        opaque = node->GetWorldColor( updateBufferIndex ).a >= FULLY_OPAQUE;
        break;
      }
    }
  }

  return opaque;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
