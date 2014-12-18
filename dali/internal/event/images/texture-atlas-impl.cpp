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

TextureAtlasPtr TextureAtlas::New( unsigned int width, unsigned int height, Pixel::Format pixelformat )
{

  TextureAtlasPtr internal = new TextureAtlas( width, height, pixelformat );
  internal->Initialize();
  return internal;
}

TextureAtlas::TextureAtlas( unsigned int width, unsigned int height, Pixel::Format pixelformat )
: Image(),
  mSize( Vector2( static_cast< float >( width), static_cast< float >( height ) ) ),
  mSpacing( 0u ),
  mFreeBlockCount( 1u ),
  mResizePolicy( Dali::TextureAtlas::RESIZE_AUTOMATICALLY ),
  mShowBorders( false )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  printf("Creating TextureAtlas %i x %i\n", width, height );
  const ImageTicketPtr& t = mResourceClient->AllocateBitmapImage(width, height, width, height, pixelformat);
  mTicket = t.Get();
  mTicket->AddObserver(*this);

  Integration::Bitmap* bitmap = mResourceClient->GetBitmap(mTicket);
  mBuffer = bitmap->GetBuffer();
  mBufferSize = bitmap->GetBufferSize();
  unsigned int* buffer = reinterpret_cast< unsigned int* >( mBuffer );
  for ( unsigned int i=0; i< mBufferSize / sizeof( unsigned int ); i++ )
  {
    *(buffer++) = ATLAS_BACKGROUND;
  }
  // Create the first free block
  FreeBlock freeBlock;
  freeBlock.block.x = 0u;
  freeBlock.block.y = 0u;
  freeBlock.block.w = width;
  freeBlock.block.h = height;
  freeBlock.alternates[ 0 ] = 0u;
  freeBlock.alternates[ 1 ] = 0u;
  mFreeBlocks.push_back( freeBlock );
}

TextureAtlas::~TextureAtlas()
{
}

void TextureAtlas::Update()
{
}

PixelBuffer* TextureAtlas::GetAtlasBuffer()
{
  PixelBuffer* buffer = NULL;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    buffer = bitmap->GetBuffer();
  }
  return buffer;
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


Dali::TextureAtlas::TextureAtlasResult TextureAtlas::Add( const unsigned int width, const unsigned int height )
{
   Dali::TextureAtlas::TextureAtlasResult result;
  printf("\nAdding Texture: %i x %i + border of %i to Atlas\n", width, height, mSpacing );
  SelectedBlock block = FindSmallestBlock( width, height );
  if ( block.block.w )
  {
    printf("Storing Texture: at position: %i, %i\n", block.block.x, block.block.y );
    CopyInPlace( width, height, block );
  }
  else
  {
    printf("CANNOT PLACE TEXTURE!!!\n");
    result.result = Dali::TextureAtlas::ADD_FAILED;
    return result;

  }
  result.result = Dali::TextureAtlas::SUCCESS;
  result.x = block.block.x + mSpacing;
  result.y = block.block.y + mSpacing;
  return result;
}

Dali::TextureAtlas::TextureAtlasResult TextureAtlas::Remove( const Dali::BitmapImage& bitmap )
{
  printf("Removing Image from Atlas\n");

  Dali::TextureAtlas::TextureAtlasResult result;
  return result;
}

void TextureAtlas::SetSpacing( unsigned int pixels )
{
  mSpacing = pixels;
}

TextureAtlas::SelectedBlock TextureAtlas::FindSmallestBlock( const unsigned int width, const unsigned int height )
{

  printf("\tCurrent Free Blocks\n");

  DisplayFreeBlocks();
  unsigned int targetWidth = width + ( mSpacing << 1 );
  unsigned int targetHeight = height + ( mSpacing << 1 );

  unsigned int area = targetWidth * targetHeight;
  std::vector< unsigned int > possibleBlocks;

  for ( unsigned int i = 0; i < mFreeBlockCount; i++ )
  {
    // Check the area of the free blocks to be possibles for allocation in this pass ( width of zero indicates block is no longer being used )
    if ( mFreeBlocks[ i ].block.w && area <= ( mFreeBlocks[ i ].block.w * mFreeBlocks[ i ].block.h ) )
    {
      possibleBlocks.push_back( i );
      printf("\tFound possible block:(%i) x: %i, y: %i, width: %i, height: %i\n", i,mFreeBlocks[i].block.x, mFreeBlocks[i].block.y, mFreeBlocks[i].block.w, mFreeBlocks[i].block.h);
    }
  }

  if ( !possibleBlocks.size() )
  {
    // No candidates for block placement, indicate with zero width
    SelectedBlock result;
    result.block.w = 0u;
    return result;
  }

  // Check to see if this texture will fit in either orientation
  std::vector< FittingBlock > fittingBlocks;
  FittingBlock fblock;
  bool isRotated = false;

  for ( unsigned int i =0; i < possibleBlocks.size() ; i++ )
  {
    unsigned int index = possibleBlocks[ i ];
    unsigned int w = mFreeBlocks[ index ].block.w;
    unsigned int h = mFreeBlocks[ index ].block.h;

    if ( targetWidth <= w && targetHeight <= h  )
    {
      fblock.index = index;
      fblock.isRotated = false;
      fittingBlocks.push_back( fblock );
    }
    else
    {
      if ( targetWidth <= h && targetHeight <= w )
      {
        fblock.index = index;
        fblock.isRotated = true;
        fittingBlocks.push_back( fblock );
      }
    }
  }

  if ( !fittingBlocks.size() )
  {
    // No candidates for block placement, indicate with zero width
    SelectedBlock result;
    result.block.w = 0u;
    return result;
  }

  printf("\tWill fit in %lu Free Block(s)\n", fittingBlocks.size() );

  // Get the smallest free block that there is
  unsigned int size = -1 ;
  unsigned int selectedBlock = 0u;
  for ( unsigned int i = 0; i < fittingBlocks.size(); i++ )
  {
    unsigned int blockSize = ( mFreeBlocks[ fittingBlocks[ i ].index ].block.w ) * mFreeBlocks[ fittingBlocks[ i ].index ].block.h;
    if ( blockSize < size )
    {
      size = blockSize;
      selectedBlock = fittingBlocks[ i ].index;
      isRotated = fittingBlocks[ i ].isRotated;
    }
  }

  unsigned int w = mFreeBlocks[ selectedBlock ].block.w;
  unsigned int h = mFreeBlocks[ selectedBlock ].block.h;
  printf("\tChoosing block: %i size: %i x %i%s", selectedBlock, w, h, isRotated ? " rotated\n" : "\n");

  // Save the block details that we have chosen ( subdividing will destroy the current data )
  FreeBlock thisBlock = mFreeBlocks[ selectedBlock ];
  FreeBlock& currentBlock = mFreeBlocks[ selectedBlock ];

  // Is the texture to be rotated to fit this block?
  if ( isRotated )
  {
    unsigned int temp = targetWidth;
    targetWidth = targetHeight;
    targetHeight = temp;
  }

  // Have we fit flush to the size of this free block?
  if ( w == targetWidth && h == targetHeight )
  {
    printf("\tFits flush!\n");
    mFreeBlocks[ selectedBlock ].block.w = 0u;
  }
  else
  {
    if ( targetWidth < w && targetHeight < h )
    {
      // Sub-divide this free block ( replace the current entry and add for both orientation splits )
      printf("\tSubdividing!\n");

      unsigned int x = currentBlock.block.x;
      unsigned int y = currentBlock.block.y;

      // Split original block ( current y and height don't change )
      currentBlock.block.x += targetWidth;
      currentBlock.block.w -= targetWidth;

      // Point to the next alternates ( their index + 1 )
      currentBlock.alternates[ 0 ] = mFreeBlockCount + 2;
      currentBlock.alternates[ 1 ] = mFreeBlockCount + 3;

      // Create new vertical split
      FreeBlock newBlock;
      newBlock.block.x = x;
      newBlock.block.y = y + targetHeight;
      newBlock.block.w = targetWidth;
      newBlock.block.h = h - targetHeight;
      newBlock.alternates[ 0 ] = currentBlock.alternates[ 0 ];
      newBlock.alternates[ 1 ] = currentBlock.alternates[ 1 ];

      // If our new block is going to go over the end of the container, then it needs to grow
      if ( mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
        printf("Increasing Container!\n");
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }

      // Create alternative splits
      newBlock.block.x = x + targetWidth;
      newBlock.block.y = y;
      newBlock.block.w = w - targetWidth;
      newBlock.block.h = targetHeight;
      newBlock.alternates[ 0 ] = selectedBlock + 1;
      newBlock.alternates[ 1 ] = mFreeBlockCount + 1;

      if ( ++mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
        printf("Increasing Container!\n");
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }

      newBlock.block.x = x;
      newBlock.block.y = y + targetHeight;
      newBlock.block.w = w;
      newBlock.block.h = h - targetHeight;

      if ( ++mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
        printf("Increasing Container!\n");
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }
      mFreeBlockCount++;
      DisplayFreeBlocks();
    }
    else
    {
      if ( targetWidth < w )
      {
        printf("\tAdjusting block vertically!\n");
        // Replace current with a vertical split along the right edge
        currentBlock.block.x += targetWidth;
        currentBlock.block.w -= targetWidth;
      }
      else
      {
        printf("\tAdjusting block horizontally!\n");
        // Replace current with an horizontal split across the lower edge
        currentBlock.block.y += targetHeight;
        currentBlock.block.h -= targetHeight;
      }
    }
  }

  // Make sure the current old alternates are removed
  DiscardBlock( thisBlock.alternates[ 0 ] );
  DiscardBlock( thisBlock.alternates[ 1 ] );

  DisplayFreeBlocks();
  printf("\tFreeBlocks Capacity = %lu\n", mFreeBlocks.size() );

  SelectedBlock resultBlock;
  resultBlock.block = thisBlock.block;
  resultBlock.block.w = targetWidth;
  resultBlock.block.h = targetHeight;
  resultBlock.isRotated = isRotated;
  return resultBlock;
}

void TextureAtlas::DiscardBlock( const unsigned int block )
{
  if ( block )
  {
    unsigned int thisBlock = block - 1u;
    printf("\tDiscarding block %i\n", thisBlock );

    // Get the alternates for this block and remove any references
    unsigned int alt1 = mFreeBlocks[ thisBlock ].alternates[ 0 ];
    unsigned int alt2 = mFreeBlocks[ thisBlock ].alternates[ 1 ];
    mFreeBlocks[ thisBlock ].block.w = 0u;

    if ( alt1-- )
    {
      unsigned int alt3 = mFreeBlocks[ alt1 ].alternates[ 0 ];
      unsigned int alt4 = mFreeBlocks[ alt1 ].alternates[ 1 ];
      if ( alt3 == block )
      {
        mFreeBlocks[ alt1 ].alternates[ 0 ] = 0u;
      }
      if ( alt4 == block )
      {
        mFreeBlocks[ alt1 ].alternates[ 1 ] = 0u;
      }
    }

    if ( alt2-- )
    {
      unsigned int alt3 = mFreeBlocks[ alt2 ].alternates[ 0 ];
      unsigned int alt4 = mFreeBlocks[ alt2 ].alternates[ 1 ];
      if ( alt3 == block )
      {
        mFreeBlocks[ alt2 ].alternates[ 0 ] = 0u;
      }
      if ( alt4 == block )
      {
        mFreeBlocks[ alt2 ].alternates[ 1 ] = 0u;
      }
    }

    // Consolidate free blocks list
    unsigned int lastFreeBlock = mFreeBlockCount--;
    mFreeBlocks[ thisBlock ] = mFreeBlocks[ mFreeBlockCount ];

    // Fixup any references to this moved block
    alt1 = mFreeBlocks[ thisBlock ].alternates[ 0 ];
    alt2 = mFreeBlocks[ thisBlock ].alternates[ 1 ];

    if ( alt1-- )
    {
      if ( mFreeBlocks[ alt1 ].alternates[ 0 ] == lastFreeBlock )
      {
        mFreeBlocks[ alt1 ].alternates[ 0 ] = block;
      }
      else
      {
        if ( mFreeBlocks[ alt1 ].alternates[ 1 ] == lastFreeBlock )
        {
          mFreeBlocks[ alt1 ].alternates[ 1 ] = block;
        }
      }
    }

    if ( alt2-- )
    {
      if ( mFreeBlocks[ alt2 ].alternates[ 0 ] == lastFreeBlock )
      {
        mFreeBlocks[ alt2 ].alternates[ 0 ] = block;
      }
      else
      {
        if ( mFreeBlocks[ alt2 ].alternates[ 1 ] == lastFreeBlock )
        {
          mFreeBlocks[ alt2 ].alternates[ 1 ] = block;
        }
      }
    }
  }
}

void TextureAtlas::DisplayFreeBlocks()
{
  for (unsigned int i=0; i<mFreeBlockCount; i++)
  {
    if ( mFreeBlocks[ i ].block.w )
    {
      printf("\t\tblock(%i) x: %i, y: %i, width: %i, height: %i", i, mFreeBlocks[i].block.x, mFreeBlocks[i].block.y, mFreeBlocks[i].block.w, mFreeBlocks[i].block.h );
      if ( mFreeBlocks[i].alternates[ 0 ])
      {
        printf("\t\t\talternates: %i, %i", mFreeBlocks[i].alternates[0] - 1u, mFreeBlocks[i].alternates[1] -1u );
      }
      printf("\n");
    }
  }
  printf("\tFreeBlocks Count = %i\n", mFreeBlockCount );
}

void TextureAtlas::CopyInPlace( const unsigned int width, const unsigned int height, const SelectedBlock& block )
{

  unsigned int blockWidth = block.block.w - ( mSpacing << 1 );
  unsigned int blockHeight = block.block.h - ( mSpacing << 1 );
  mTextures.push_back( block );

  unsigned int* dest = ( reinterpret_cast< unsigned int* >( mBuffer ) ) + block.block.x + ( block.block.y * mWidth );
  unsigned int fill = block.isRotated ? 0xFF808080 : 0xFF606060;
  unsigned int border = 0XC00000FF;
  unsigned int stride = mWidth - block.block.w;

  if ( mShowBorders )
  {

    for ( unsigned int y = 0; y < mSpacing; y++ )
    {
      for ( unsigned int x = 0; x < block.block.w; x++ )
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
      for ( unsigned int x = 0; x < block.block.w; x++ )
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

void TextureAtlas::ShowBorders( const bool show )
{
  mShowBorders = show;
}

const Vector2 TextureAtlas::GetAtlasSize() const
{
  return Vector2( mSize );
}

} // namespace Internal

} // namespace Dali
