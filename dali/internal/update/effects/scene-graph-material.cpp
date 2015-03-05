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
#include <dali/internal/render/renderers/sampler-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Material::Material()
: mShader(NULL),
  mCullFaceMode( CullNone ),
  mBlendingOptions()
{
}

Material::~Material()
{
}

void Material::SetShader( Shader* shader )
{
}

Shader* Material::GetShader()
{
  return mShader;
}

void Material::AddSampler( Sampler* sampler )
{
  // @todo MESH_REWORK
}

void Material::RemoveSampler( Sampler* sampler )
{
  // @todo MESH_REWORK
}

const Material::Samplers& Material::GetSamplers() const
{
  // @todo MESH_REWORK
  return mSamplers;
}

void Material::SetFaceCulling( BufferIndex updateBufferIndex, Dali::CullFaceMode mode )
{
  // @todo MESH_REWORK
}

CullFaceMode Material::GetFaceCulling() const
{
  // @todo MESH_REWORK
  return mCullFaceMode;
}

void Material::SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options )
{
  // @todo MESH_REWORK
}

void Material::SetBlendColor( BufferIndex updateBufferIndex, const Vector4& color )
{
  // @todo MESH_REWORK
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
