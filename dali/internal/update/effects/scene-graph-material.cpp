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
#include "scene-graph-material.h"

// INTERNAL HEADERS
#include <dali/internal/render/data-providers/sampler-data-provider.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Material::Material()
: mShader(NULL),
  mColor( Color::WHITE )
{
}

Material::~Material()
{
}

void Material::SetShader( const Shader* shader )
{
  mShader = shader;
  // Need to inform renderer in render thread about this shader
}

Shader* Material::GetShader() const
{
  // @todo - Fix this - move shader setup to the Renderer connect to stage... or summit
  return const_cast<Shader*>(mShader);
}

void Material::AddSampler( const Sampler* sampler )
{
  const SamplerDataProvider* sdp = static_cast< const SamplerDataProvider*>( sampler );
  mSamplers.PushBack( sdp );
}

void Material::RemoveSampler( const Sampler* sampler )
{
  const SamplerDataProvider* samplerDataProvider = sampler;

  for( Samplers::Iterator iter = mSamplers.Begin(); iter != mSamplers.End(); ++iter )
  {
    if( *iter == samplerDataProvider )
    {
      mSamplers.Erase(iter);
      return;
    }
  }
  DALI_ASSERT_DEBUG( 0 && "Sampler not found" );
}

const Material::Samplers& Material::GetSamplers() const
{
  return mSamplers;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
