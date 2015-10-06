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
#include "scene-graph-sampler.h"

// EXTERNAL HEADERS


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Sampler::Sampler( const std::string& textureUnitUniformName )
: mMinFilter( Dali::Sampler::DEFAULT ),
  mMagFilter( Dali::Sampler::DEFAULT ),
  mUWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mVWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mTextureUnitUniformName( textureUnitUniformName ),
  mTextureId(0),
  mFullyOpaque(true)
{
  //mTextureId[ 0 ] = mTextureId[ 1 ] = 0u;
}

Sampler::~Sampler()
{
}

void Sampler::SetTextureUnitUniformName( const std::string& textureUnitUniformName )
{
  mTextureUnitUniformName = textureUnitUniformName;
}

void Sampler::SetTexture( BufferIndex bufferIndex, Integration::ResourceId textureId )
{
  mTextureId = textureId;
}

void Sampler::SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter )
{
  mMinFilter = minFilter;
  mMagFilter = magFilter;
}

void Sampler::SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap )
{
}

const std::string& Sampler::GetTextureUnitUniformName() const
{
  return mTextureUnitUniformName;
}

Integration::ResourceId Sampler::GetTextureId( BufferIndex bufferIndex ) const
{
  return mTextureId;
}

Sampler::FilterMode Sampler::GetMinifyFilterMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::FilterMode>(mMinFilter);
}

Sampler::FilterMode Sampler::GetMagnifyFilterMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::FilterMode>(mMagFilter);
}

Sampler::WrapMode Sampler::GetUWrapMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::WrapMode>(mUWrapMode);
}

Sampler::WrapMode Sampler::GetVWrapMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::WrapMode>(mVWrapMode);
}

bool Sampler::AffectsTransparency( BufferIndex bufferIndex ) const
{
  return true;
  //return mAffectsTransparency;
}

void Sampler::SetFullyOpaque( bool fullyOpaque )
{
  mFullyOpaque = fullyOpaque;
}

bool Sampler::IsFullyOpaque( BufferIndex bufferIndex ) const
{
  return mFullyOpaque;
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
