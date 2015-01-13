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
#include <dali/internal/event/images/texture-atlas-impl.h>
#include <dali/internal/event/images/bitmap-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/default-texture-packer.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/images/bitmap-external.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>

#define DIAG_ATLAS

namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::TextureAtlas ), typeid( Dali::Image ), NULL );
const unsigned int ATLAS_BACKGROUND = 0xFF101010;
} // unnamed namespace

TextureAtlasPtr TextureAtlas::New( unsigned int width, unsigned int height, unsigned int spacing, Pixel::Format pixelformat )
{

  TextureAtlasPtr internal = new TextureAtlas( width, height, spacing, pixelformat );
  return internal;
}

TextureAtlas::TextureAtlas( unsigned int width, unsigned int height, unsigned int spacing, Pixel::Format pixelformat )
: Image(),
  mSize( Vector2( static_cast< float >( width), static_cast< float >( height ) ) ),
  mSpacing( spacing ),
  mResizePolicy( Dali::TextureAtlas::RESIZE_AUTOMATICALLY ),
  mShowBorders( false ),
  mPackingAlgorithm( new DefaultTexturePacker() )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  printf("Creating TextureAtlas %i x %i\n", width, height );
  const ImageTicketPtr& t = mResourceClient->AllocateBitmapImage(width, height, width, height, pixelformat);
  mTicket = t.Get();
  mTicket->AddObserver(*this);
  mBitmapCached = mResourceClient->GetBitmap(mTicket);
  mBuffer = mBitmapCached->GetBuffer();
  mBufferSize = mBitmapCached->GetBufferSize();

  unsigned int* buffer = reinterpret_cast< unsigned int* >( mBuffer );
  for ( unsigned int i=0; i< mBufferSize / sizeof( unsigned int ); i++ )
  {
    *(buffer++) = ATLAS_BACKGROUND;
  }
  mPackingAlgorithm->Initialize( width, height, spacing );
}

TextureAtlas::~TextureAtlas()
{
}

void TextureAtlas::Update()
{
}

unsigned int TextureAtlas::GetAtlasBufferSize() const
{
  unsigned int bufferSize = 0;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    bufferSize = bitmap->GetBufferSize();
  }
  return bufferSize;
}

void TextureAtlas::Connect()
{
  ++mConnectionCount;

  if (mConnectionCount == 1)
  {
    if (!mTicket && mBitmapCached)
    {
      const ImageTicketPtr& t = mResourceClient->AddBitmapImage(mBitmapCached.Get());
      mTicket = t.Get();
      mTicket->AddObserver(*this);
    }
  }
}

void TextureAtlas::Disconnect()
{
  if (!mTicket)
  {
    return;
  }

  --mConnectionCount;

  if (mConnectionCount == 0 && mReleasePolicy == Dali::Image::Unused)
  {
    mBitmapCached = mResourceClient->GetBitmap(mTicket);
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket->RemoveObserver(*this);
    mTicket.Reset();
  }
}

Integration::Bitmap* TextureAtlas::GetBitmap() const
{
  Integration::Bitmap* bitmap = NULL;

  if (mTicket)
  {
    bitmap = mResourceClient->GetBitmap(mTicket);
  }
  else
  {
    // off screen and freeing memory was requested
    bitmap = mBitmapCached.Get();
  }

  DALI_ASSERT_DEBUG(bitmap);

  return bitmap;
}


Dali::TextureAtlas::AddResult TextureAtlas::Add( unsigned int width, unsigned int height )
{
  Dali::TextureAtlas::AddResult result;
  Dali::TextureAtlas::SelectedBlock block = mPackingAlgorithm->Add( width, height );

  if ( block.block.size.width )
  {
    printf("Storing Texture: at position: %i, %i\n", block.block.x, block.block.y );
    CopyInPlace( width, height, block );
  }
  else
  {
    printf("CANNOT PLACE TEXTURE!!!\n");
    result.result = Dali::TextureAtlas::ADD_FAILED;
    mTextures.resize( 0u );

    unsigned int* buffer = reinterpret_cast< unsigned int* >( mBuffer );
    for ( unsigned int i=0; i< mBufferSize / sizeof( unsigned int ); i++ )
    {
      *(buffer++) = ATLAS_BACKGROUND;
    }
    RectArea area( 0, 0, mWidth, mHeight );
    mResourceClient->UpdateBitmapArea( mTicket, area );
    return result;
  }

  result.result = Dali::TextureAtlas::SUCCESS;

  Dali::TextureAtlas::Block entry;
  entry.x = block.block.x + mSpacing;
  entry.y = block.block.y + mSpacing;

  if ( block.isRotated )
  {
    entry.size.width = height;
    entry.size.height = width;
  }
  else
  {
    entry.size.width = width;
    entry.size.height = height;
  }

  mTextures.push_back( entry );
  result.id = mTextures.size();

  RectArea area( entry.x, entry.y, entry.size.width, entry.size.height );
  mResourceClient->UpdateBitmapArea( mTicket, area );
  return result;
}

void TextureAtlas::Remove( unsigned int id )
{
  printf("Removing Image %i from Atlas\n", id);
}

Dali::TextureAtlas::Result TextureAtlas::ForceSort()
{
  return Dali::TextureAtlas::SUCCESS;

}

Dali::UVBlock TextureAtlas::GetUVCoordinates( unsigned int id )
{
  Dali::UVBlock block;
  return block;
}

void TextureAtlas::CopyInPlace( unsigned int width, unsigned int height, const Dali::TextureAtlas::SelectedBlock& block )
{

  unsigned int blockWidth = block.block.size.width - ( mSpacing << 1 );
  unsigned int blockHeight = block.block.size.height - ( mSpacing << 1 );

  unsigned int* dest = ( reinterpret_cast< unsigned int* >( mBuffer ) ) + block.block.x + ( block.block.y * mWidth );
  unsigned int fill = block.isRotated ? 0xFF808080 : 0xFF606060;
  unsigned int border = 0XC00000FF;
  unsigned int stride = mWidth - block.block.size.width;

  if ( mShowBorders )
  {

    for ( unsigned int y = 0; y < mSpacing; y++ )
    {
      for ( unsigned int x = 0; x < block.block.size.width; x++ )
      {
        *(dest++) = border;
      }
      dest += stride;
    }

    for ( unsigned int y = 0; y < blockHeight; y++ )
    {
      for ( unsigned int x = 0; x < mSpacing; x++ )
      {
        *(dest++) = border;
      }

      for ( unsigned int x = 0; x < blockWidth; x++ )
      {
        *(dest++) = fill;
      }

      for ( unsigned int x = 0; x < mSpacing; x++ )
      {
        *(dest++) = border;
      }
      dest += stride;
    }

    for ( unsigned int y = 0; y < mSpacing; y++ )
    {
      for ( unsigned int x = 0; x < block.block.size.width; x++ )
      {
        *(dest++) = border;
      }
      dest += stride;
    }
  }
  else
  {
    if ( mSpacing )
    {
      dest += ( mSpacing * mWidth ) + mSpacing;
      stride += mSpacing << 1;
    }

    for ( unsigned int y = 0; y < blockHeight; y++ )
    {
      for ( unsigned int x = 0; x < blockWidth; x++ )
      {
        *(dest++) = fill;
      }
      dest += stride;
    }
  }
}

void TextureAtlas::Resize()
{

}

void TextureAtlas::ShowBorders( const bool show )
{
  mShowBorders = show;
}

const Vector2 TextureAtlas::GetAtlasSize() const
{
  return Vector2( mSize );
}

void TextureAtlas::InstallPackingAlgorithm( Dali::TextureAtlas::PackingAlgorithm& packingAlgorithm )
{
  delete mPackingAlgorithm;
  mPackingAlgorithm = packingAlgorithm.Clone();
  mPackingAlgorithm->Initialize( mWidth, mHeight, mSpacing );
}

const unsigned int TextureAtlas::Count() const
{
  return mTextures.size();
}


const unsigned int TextureAtlas::GetSpacing() const
{
  return mSpacing;
}

} // namespace Internal

} // namespace Dali
