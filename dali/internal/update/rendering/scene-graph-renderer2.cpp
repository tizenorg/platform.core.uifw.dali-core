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
#include <dali/internal/update/nodes/node.h>



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
  std::cout<<"R3nderer::OnStageConnect"<<std::endl;

  ++mReferenceCount;
  if( !mRenderer)
  {
    RenderDataProvider* dataProvider = NewRenderDataProvider();

    RenderGeometry* renderGeometry = mGeometry->GetRenderGeometry(mSceneController);
    mRenderer = NewRenderer::New( *node, dataProvider, renderGeometry );
    mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
  }
}

//Called when the node with this renderer has gone out of the stage
void R3nderer::OnStageDisconnect(const Node* node)
{
  --mReferenceCount;
  if( mReferenceCount == 0 )
  {
    mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
    mRenderer = NULL;
  }
  std::cout<<"R3nderer::OnStageDisconnect"<<std::endl;
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

}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
