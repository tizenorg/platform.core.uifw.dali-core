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
#include <dali/internal/event/common/fixed-size-memory-pool-impl.h>


namespace Dali
{

namespace Internal
{

FixedSizeMemoryPoolPtr FixedSizeMemoryPool::New( size_t size )
{
  return FixedSizeMemoryPoolPtr( new FixedSizeMemoryPool( size ) );
}

FixedSizeMemoryPoolPtr FixedSizeMemoryPool::New( size_t size, size_t initialCapacity, size_t maximumBlockCapacity   )
{
  return FixedSizeMemoryPoolPtr( new FixedSizeMemoryPool( size, initialCapacity, maximumBlockCapacity ) );
}

FixedSizeMemoryPool::FixedSizeMemoryPool( size_t fixedSize, size_t initialCapacity, size_t maximumBlockCapacity )
:  mFixedSize( fixedSize ),
   mMemoryBlocks( initialCapacity * mFixedSize ),
   mMaximumBlockCapacity( maximumBlockCapacity ),
   mCurrentBlock( &mMemoryBlocks ),
   mCurrentBlockCapacity( initialCapacity ),
   mCurrentBlockSize( 0 ),
   mDeletedObjects( NULL )
{}

FixedSizeMemoryPool::~FixedSizeMemoryPool()
{
  // Clean up memory block linked list (mMemoryBlocks will be auto-destroyed by its destructor)
  Block* block = mMemoryBlocks.nextBlock;
  while( block )
  {
    Block* nextBlock = block->nextBlock;
    delete block;
    block = nextBlock;
  }
}

void* FixedSizeMemoryPool::NewAllocation()
{
  // First, recycle deleted objects
  if( mDeletedObjects )
  {
    void* recycled = mDeletedObjects;
    mDeletedObjects = *( reinterpret_cast< void** >( mDeletedObjects ) );  // Pop head off front of deleted objects list
    return recycled;
  }

  // Check if current block is full
  if( mCurrentBlockSize >= mCurrentBlockCapacity )
  {
    AllocateNewBlock();
  }

  // Placement new the object in block memory
  unsigned char* objectAddress = static_cast< unsigned char* >( mCurrentBlock->blockMemory );
  objectAddress += mCurrentBlockSize * mFixedSize;
  mCurrentBlockSize++;

  return objectAddress;
}

void FixedSizeMemoryPool::DeleteAllocation( void* memory )
{
  // Add memory to head of deleted objects list. Store next address in the same memory space as the old object.
  *( reinterpret_cast< void** >( memory ) ) = mDeletedObjects;
  mDeletedObjects = memory;
}

void FixedSizeMemoryPool::AllocateNewBlock()
{
  // Double capacity for the new block
  size_t size = mCurrentBlockCapacity * 2;
  if( size > mMaximumBlockCapacity || size < mCurrentBlockCapacity )    // Check for overflow of size type
  {
    size = mMaximumBlockCapacity;
  }

  mCurrentBlockCapacity = size;

  // Allocate
  Block* block = new Block( mCurrentBlockCapacity * mFixedSize );
  mCurrentBlock->nextBlock = block;       // Add to end of linked list
  mCurrentBlock = block;

  mCurrentBlockSize = 0;
}

FixedSizeMemoryPool::Block::Block( size_t size )
: nextBlock( NULL )
{
  blockMemory = ::operator new( size );
  DALI_ASSERT_DEBUG( blockMemory );
}

FixedSizeMemoryPool::Block::~Block()
{
  ::operator delete( blockMemory );
}

} // namespace Internal

} // namespace Dali
