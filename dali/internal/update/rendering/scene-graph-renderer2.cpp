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

//  //@TODO:FERRAN: Instead of setting flag into the actor, I think the sensible thing to do here is notify update manager that the scene needs to be updated!
//  if( mParent )
//  {
//    // only do this if we are on-stage. Ensures the render lists are re-sorted
//    mParent->SetDirtyFlag( SortModifierFlag );
//  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
