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
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/sampler.h> // Dali::Internal::Sampler
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                    |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "minification-filter",   INTEGER,   true, false,  true, Dali::Sampler::Property::MINIFICATION_FILTER )
DALI_PROPERTY( "magnification-filter",  INTEGER,   true, false,  true, Dali::Sampler::Property::MAGNIGICATION_FILTER )
DALI_PROPERTY( "u-wrap",                INTEGER,   true, false,  true, Dali::Sampler::Property::U_WRAP )
DALI_PROPERTY( "v-wrap",                INTEGER,   true, false,  true, Dali::Sampler::Property::V_WRAP )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> SAMPLER_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Sampler ), typeid( Dali::Handle ), Create );

} // unnamed namespace

SamplerPtr Sampler::New( )
{
  SamplerPtr sampler( new Sampler() );
  sampler->Initialize();
  return sampler;
}

void Sampler::SetFilterMode( Dali::Sampler::FilterMode minFilter, Dali::Sampler::FilterMode magFilter )
{
  if( NULL != mRenderObject )
  {
    SetFilterModeMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject, (unsigned int)minFilter, (unsigned int)magFilter );
  }
}

void Sampler::SetWrapMode( Dali::Sampler::WrapMode uWrap, Dali::Sampler::WrapMode vWrap )
{
  if( NULL != mRenderObject )
  {
    SetWrapModeMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject, (unsigned int)uWrap, (unsigned int)vWrap );
  }
}

Render::Sampler* Sampler::GetSamplerRenderObject()
{
  return mRenderObject;
}


Sampler::Sampler()
:mEventThreadServices( *Stage::GetCurrent() ),
 mRenderObject( NULL )
{
}

void Sampler::Initialize()
{
  SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();

  mRenderObject = new Render::Sampler();
  AddSamplerMessage( updateManager, *mRenderObject );
}

Sampler::~Sampler()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();
    RemoveSamplerMessage( updateManager, *mRenderObject );
  }
}

} // namespace Internal
} // namespace Dali
