

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

namespace Dali
{

TextureAtlas::TextureAtlas()
{
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas TextureAtlas::New( unsigned int width, unsigned int height, Pixel::Format pixelformat )
{
  return TextureAtlas();
}

void TextureAtlas::InstallPackingAlgorithm( PackingAlgorithm* packingAlgorithm )
{
}

void TextureAtlas::SetResizePolicy( ResizePolicy policy )
{
}

TextureAtlas::TextureAtlasResult TextureAtlas::Add( const BitmapImage& bitmap )
{
  TextureAtlasResult result;
  result.result = TextureAtlas::SUCCESS;
  return result;
}

void TextureAtlas::Remove( unsigned int id )
{
}

Vector2 TextureAtlas::GetAtlasSize() const
{
  Vector2 size( 0.0f, 0.0f );
  return size;
}

TextureAtlas::AtlasResult TextureAtlas::ForceSort()
{
  return TextureAtlas::SUCCESS;
}

UVBlock TextureAtlas::GetUVCoordinates( unsigned int id )
{
  UVBlock block;
  return block;
}

} // namespace Dali
