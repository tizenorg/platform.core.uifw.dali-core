

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
#include <dali/internal/event/images/texture-atlas-impl.h>

namespace Dali
{

TextureAtlas::TextureAtlas()
{
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas::TextureAtlas( Internal::TextureAtlas* internal )
: Image( internal )
{
}

TextureAtlas TextureAtlas::DownCast( BaseHandle handle )
{
return TextureAtlas( dynamic_cast< Dali::Internal::TextureAtlas* >( handle.GetObjectPtr() ) );
}

TextureAtlas TextureAtlas::New( unsigned int width, unsigned int height, unsigned int spacing, Pixel::Format pixelformat )
{
  Internal::TextureAtlasPtr internal = Internal::TextureAtlas::New( width, height, spacing, pixelformat );
  return TextureAtlas( internal.Get() );
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

void TextureAtlas::InstallPackingAlgorithm( PackingAlgorithm& packingAlgorithm )
{
	return GetImplementation(*this).InstallPackingAlgorithm( packingAlgorithm );
}

TextureAtlas::AddResult TextureAtlas::Add( unsigned int width, unsigned int height )
{
	return GetImplementation(*this).Add( width, height );

}

//std::vector< TextureAtlas::AddResult > TextureAtlas::Add( const std::vector< const BitmapImage& >& bitmaps )
//{
//	return GetImplementation(*this).Add( bitmaps );
//}

void TextureAtlas::Remove( unsigned int id )
{
	return GetImplementation(*this).Remove( id );
}

Vector2 TextureAtlas::GetAtlasSize() const
{
  return GetImplementation(*this).GetAtlasSize();
}

TextureAtlas::Result TextureAtlas::ForceSort()
{
	return GetImplementation(*this).ForceSort();
}

UVBlock TextureAtlas::GetUVCoordinates( unsigned int id )
{
  return GetImplementation(*this).GetUVCoordinates( id );
}

} // namespace Dali
