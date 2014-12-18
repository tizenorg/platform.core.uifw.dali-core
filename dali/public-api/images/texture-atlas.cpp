

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/texture-atlas.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/images/texture-atlas-impl.h>
#include <dali/public-api/images/image.h>


namespace Dali
{

TextureAtlas::TextureAtlas()
{
}

TextureAtlas::TextureAtlas( Internal::TextureAtlas* internal )
  : Image( internal )
{
}

TextureAtlas TextureAtlas::DownCast( BaseHandle handle )
{
  return TextureAtlas( dynamic_cast<Dali::Internal::TextureAtlas*>( handle.GetObjectPtr() ) );
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas::TextureAtlas( const TextureAtlas& handle )
: Image( handle )
{
}

TextureAtlas& TextureAtlas::operator=( const TextureAtlas& rhs )
{
  BaseHandle::operator=( rhs );
  return *this;
}

TextureAtlas TextureAtlas::New( unsigned int width, unsigned int height, Pixel::Format pixelformat )
{
  Internal::TextureAtlasPtr internal = Internal::TextureAtlas::New( width, height, pixelformat );
  return TextureAtlas( internal.Get() );
}

TextureAtlas TextureAtlas::New( const Vector2& size, Pixel::Format pixelformat )
{
  return TextureAtlas::New( static_cast< unsigned int>( size.x ), static_cast< unsigned int >( size.y ), pixelformat );
}

PixelBuffer* TextureAtlas::GetAtlasBuffer()
{
  return GetImplementation(*this).GetAtlasBuffer();
}

unsigned int TextureAtlas::GetAtlasBufferSize() const
{
  return GetImplementation(*this).GetAtlasBufferSize();
}

const Vector2 TextureAtlas::GetAtlasSize() const
{
  return GetImplementation(*this).GetAtlasSize();
}

void TextureAtlas::Update()
{
  GetImplementation(*this).Update();
}

TextureAtlas::TextureAtlasResult TextureAtlas::Add( const unsigned int width, const unsigned int height )
{
  return GetImplementation(*this).Add( width, height );
}

TextureAtlas::TextureAtlasResult TextureAtlas::Remove( const BitmapImage& bitmap )
{
  return GetImplementation(*this).Remove( bitmap );
}

void TextureAtlas::SetSpacing( unsigned int pixels )
{
  return GetImplementation(*this).SetSpacing( pixels );
}

void TextureAtlas::ShowBorders( const bool show )
{
  return GetImplementation(*this).ShowBorders( show );
}

} // namespace Dali
