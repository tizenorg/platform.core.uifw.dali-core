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
{
}

R3nderer::~R3nderer()
{
}

void R3nderer::SetMaterial( BufferIndex bufferIndex, Material* material)
{
  DALI_ASSERT_DEBUG( material != NULL && "Material pointer is NULL" );

  std::cout<<"SetMaterial"<<std::endl;
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

void R3nderer::OnStageConnect(Node* node)
{
  std::cout<<"R3nderer::OnStageConnect"<<std::endl;

  RenderDataProvider* dataProvider = NewRenderDataProvider();

  RenderGeometry* renderGeometry = mGeometry->GetRenderGeometry(mSceneController);
  mRenderer = NewRenderer::New( *node, dataProvider, renderGeometry );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
}

void R3nderer::OnStageDisconnect(const Node* node)
{
  std::cout<<"R3nderer::OnStageDisconnect"<<std::endl;


}

void R3nderer::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  mSceneController = &sceneController;

  //Create a NewRenderer and add it to RenderManager
}

void R3nderer::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
  //Remove renderer from RenderManager
  std::cout<<"R3nderer::DisconnectFromSceneGraph"<<std::endl;
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

NewRenderer* R3nderer::GetRenderer()
{
  return mRenderer;
}



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
