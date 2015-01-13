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
#include <dali/public-api/images/default-texture-packer.h>

namespace Dali
{

DefaultTexturePacker::~DefaultTexturePacker()
{
}

void DefaultTexturePacker::Initialize( unsigned int width, unsigned int height, unsigned int spacing )
{

  // Create the first free block
  FreeBlock freeBlock;
  freeBlock.block.x = 0u;
  freeBlock.block.y = 0u;

  mWidth = width;
  mHeight = height;
  mSpacing = spacing;
  freeBlock.block.size.width = width;
  freeBlock.block.size.height = height;
  freeBlock.alternates[ 0 ] = 0u;
  freeBlock.alternates[ 1 ] = 0u;
  mFreeBlocks.push_back( freeBlock );
  mFreeBlockCount = 1u;
}

TextureAtlas::SelectedBlock DefaultTexturePacker::Add( unsigned int width, unsigned int height )
{
  Dali::TextureAtlas::SelectedBlock resultBlock;
  resultBlock.block.size.width = 0u;
  //resultBlock.flags = 0u;

  DisplayFreeBlocks();
  unsigned int targetWidth = width + ( mSpacing << 1 );
  unsigned int targetHeight = height + ( mSpacing << 1 );
  unsigned int area = targetWidth * targetHeight;

  printf("\nDefault Packer adding Texture: %i x %i + border of %i to Atlas\n", width, height, mSpacing );
  printf("\tCurrent Free Blocks Count: %i\n", mFreeBlockCount);

  std::vector< unsigned int > possibleBlocks;

  for ( unsigned int i = 0; i < mFreeBlockCount; i++ )
  {
    // Check the area of the free blocks to be possibles for allocation in this pass ( width of zero indicates block is no longer being used )
    if ( mFreeBlocks[ i ].block.size.width && area <= ( mFreeBlocks[ i ].block.size.width * mFreeBlocks[ i ].block.size.height ) )
    {
      possibleBlocks.push_back( i );
      //printf("\tFound possible block:(%i) x: %i, y: %i, width: %i, height: %i\n", i,mFreeBlocks[i].block.x, mFreeBlocks[i].block.y, mFreeBlocks[i].block.w, mFreeBlocks[i].block.h);
    }
  }

  if ( !possibleBlocks.size() )
  {
    // No candidates for block placement, indicate with zero width
    mFreeBlocks.resize( 0u );
    FreeBlock freeBlock;
    freeBlock.block.x = 0u;
    freeBlock.block.y = 0u;
    freeBlock.block.size.width = mWidth;
    freeBlock.block.size.height = mHeight;
    freeBlock.alternates[ 0 ] = 0u;
    freeBlock.alternates[ 1 ] = 0u;
    mFreeBlocks.push_back( freeBlock );
    mFreeBlockCount = 1u;
    return resultBlock;
  }

  // Check to see if this texture will fit in either orientation
  std::vector< FittingBlock > fittingBlocks;
  FittingBlock fblock;
  bool isRotated = false;

  for ( unsigned int i =0; i < possibleBlocks.size() ; i++ )
  {
    unsigned int index = possibleBlocks[ i ];
    unsigned int w = mFreeBlocks[ index ].block.size.width;
    unsigned int h = mFreeBlocks[ index ].block.size.height;

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
    mFreeBlocks.resize( 0u );
    FreeBlock freeBlock;
    freeBlock.block.x = 0u;
    freeBlock.block.y = 0u;
    freeBlock.block.size.width = mWidth;
    freeBlock.block.size.height = mHeight;
    freeBlock.alternates[ 0 ] = 0u;
    freeBlock.alternates[ 1 ] = 0u;
    mFreeBlocks.push_back( freeBlock );
    mFreeBlockCount = 1u;
    // No candidates for block placement, indicate with zero width
    return resultBlock;
  }

  printf("\tWill fit in %lu Free Block(s)\n", fittingBlocks.size() );

  // Get the smallest free block that there is
  unsigned int size = -1 ;
  unsigned int selectedBlock = 0u;
  for ( unsigned int i = 0; i < fittingBlocks.size(); i++ )
  {
    unsigned int blockSize = ( mFreeBlocks[ fittingBlocks[ i ].index ].block.size.width ) * mFreeBlocks[ fittingBlocks[ i ].index ].block.size.height;
    if ( blockSize <= size )
    {
      size = blockSize;
      selectedBlock = fittingBlocks[ i ].index;
      isRotated = fittingBlocks[ i ].isRotated;
    }
  }

  unsigned int w = mFreeBlocks[ selectedBlock ].block.size.width;
  unsigned int h = mFreeBlocks[ selectedBlock ].block.size.height;
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

  /*
  // Check to see if this will be placed at an atlas edge ?
  if ( !thisBlock.block.x )
  {
    resultBlock.flags |= FITS_FLUSH_TO_LEFT;
    targetWidth -= mSpacing;
  }
  if ( !thisBlock.block.y )
  {
    resultBlock.flags |= FITS_FLUSH_TO_TOP;
    targetHeight -= mSpacing;
  }
  */

  // Have we fit flush to the size of this free block?
  //if ( w == targetWidth && h == targetHeight )
  //{
  //  printf("\tFits flush!\n");
  //}
  //else
  {
    //if ( targetWidth < w && targetHeight < h )
    {
      // Sub-divide this free block ( replace the current entry and add for both orientation splits )
      unsigned int x = currentBlock.block.x;
      unsigned int y = currentBlock.block.y;

      // Split original block ( current y and height don't change )
      currentBlock.block.x += targetWidth;
      currentBlock.block.size.width -= targetWidth;

      // Point to the next alternates ( their index + 1 )
      currentBlock.alternates[ 0 ] = mFreeBlockCount + 2;
      currentBlock.alternates[ 1 ] = mFreeBlockCount + 3;

      // Create new vertical split
      FreeBlock newBlock;
      newBlock.block.x = x;
      newBlock.block.y = y + targetHeight;
      newBlock.block.size.width = targetWidth;
      newBlock.block.size.height = h - targetHeight;
      newBlock.alternates[ 0 ] = currentBlock.alternates[ 0 ];
      newBlock.alternates[ 1 ] = currentBlock.alternates[ 1 ];

      // If our new block is going to go over the end of the container, then it needs to grow
      if ( mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }

      // Create alternative splits
      newBlock.block.x = x + targetWidth;
      newBlock.block.y = y;
      newBlock.block.size.width = w - targetWidth;
      newBlock.block.size.height = targetHeight;
      newBlock.alternates[ 0 ] = selectedBlock + 1;
      newBlock.alternates[ 1 ] = mFreeBlockCount + 1;

      if ( ++mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }

      newBlock.block.x = x;
      newBlock.block.y = y + targetHeight;
      newBlock.block.size.width = w;
      newBlock.block.size.height = h - targetHeight;

      if ( ++mFreeBlockCount == mFreeBlocks.size() )
      {
        mFreeBlocks.push_back( newBlock );
      }
      else
      {
        mFreeBlocks[ mFreeBlockCount ] = newBlock;
      }
      mFreeBlockCount++;
      DisplayFreeBlocks();
    }
    //else
    //{
    //  if ( targetWidth < w )
    //  {
    //    // Replace current with a vertical split along the right edge
    //    currentBlock.block.x += targetWidth;
    //    currentBlock.block.w -= targetWidth;
    //  }
    //  else
    //  {
    //    // Replace current with an horizontal split across the lower edge
    //    currentBlock.block.y += targetHeight;
    //   currentBlock.block.h -= targetHeight;
    //  }
    //}
  }

  // Make sure the current old alternates are removed
  DiscardBlock( thisBlock.alternates[ 0 ] );
  DiscardBlock( thisBlock.alternates[ 1 ] );

  DisplayFreeBlocks();
  printf("\tFreeBlocks Capacity = %lu\n", mFreeBlocks.size() );

  resultBlock.block = thisBlock.block;
  resultBlock.block.size.width = targetWidth;
  resultBlock.block.size.height = targetHeight;
  resultBlock.isRotated = isRotated;
  return resultBlock;
}

void DefaultTexturePacker::DiscardBlock( const unsigned int block )
{
  if ( block )
  {
    unsigned int thisBlock = block - 1u;
    printf("\tDiscarding block %i\n", thisBlock );

    // Get the alternates for this block and remove any references
    unsigned int alt1 = mFreeBlocks[ thisBlock ].alternates[ 0 ];
    unsigned int alt2 = mFreeBlocks[ thisBlock ].alternates[ 1 ];
    mFreeBlocks[ thisBlock ].block.size.width = 0u;

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

    if ( block < mFreeBlockCount )
    {
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
}


void DefaultTexturePacker::DisplayFreeBlocks()
{
  return;
  for (unsigned int i=0; i<mFreeBlockCount; i++)
  {
    printf("\t\tblock(%i) x: %i, y: %i, width: %i, height: %i", i, mFreeBlocks[i].block.x, mFreeBlocks[i].block.y,
    			 mFreeBlocks[i].block.size.width, mFreeBlocks[i].block.size.height );

    if ( mFreeBlocks[i].alternates[ 0 ])
    {
      printf("\t\t\talternates: %i, %i", mFreeBlocks[i].alternates[0] - 1u, mFreeBlocks[i].alternates[1] -1u );
    }
    printf("\n");
  }
  printf("\tFreeBlocks Count = %i\n", mFreeBlockCount );
}

std::vector< TextureAtlas::SelectedBlock > DefaultTexturePacker::Add( std::vector< TextureAtlas::BlockSize >& blocks )
{
	std::vector< TextureAtlas::SelectedBlock > selectedBlocks;
	return selectedBlocks;
}

void DefaultTexturePacker::Remove( const TextureAtlas::Block& block )
{
}

TextureAtlas::SelectedBlock* DefaultTexturePacker::ForceSort( unsigned int count, TextureAtlas::BlockSize* )
{
	return NULL;
}

void DefaultTexturePacker::ForceSortComplete()
{
}


void SimpleTexturePacker::Initialize( unsigned int width, unsigned int height, unsigned int spacing )
{
	mWidth = width;
	mHeight = height;
	mSpacing = spacing;
  mNextX = 0;
  mNextY = 0;
}

SimpleTexturePacker::~SimpleTexturePacker()
{
}


TextureAtlas::SelectedBlock SimpleTexturePacker::Add( unsigned int width, unsigned int height )
{
  printf("\nSimple Packer adding Texture: %i x %i to Atlas\n", width, height );
  unsigned int w = 64u;
  unsigned int h = 64u;
  Dali::TextureAtlas::SelectedBlock resultBlock;
  resultBlock.block.x = mNextX;
  resultBlock.block.y = mNextY;
  resultBlock.block.size.width = w;
  resultBlock.block.size.height = h;
  resultBlock.isRotated = false;

  mNextX += w;
  if ( mNextX >= mWidth )
  {
    mNextX = 0;
    mNextY += h;
    if ( mNextY >= mHeight )
    {
      mNextY = 0;
      resultBlock.block.size.width = 0;
    }
  }

  return resultBlock;
}

std::vector< TextureAtlas::SelectedBlock > SimpleTexturePacker::Add( std::vector< TextureAtlas::BlockSize >& blocks )
{
	std::vector< TextureAtlas::SelectedBlock > selectedBlocks;
	return selectedBlocks;
}


void SimpleTexturePacker::Remove( const TextureAtlas::Block& block )
{
}

TextureAtlas::SelectedBlock* SimpleTexturePacker::ForceSort( unsigned int count, TextureAtlas::BlockSize* )
{
	return NULL;
}

void SimpleTexturePacker::ForceSortComplete()
{
}


} // namespace Dali

