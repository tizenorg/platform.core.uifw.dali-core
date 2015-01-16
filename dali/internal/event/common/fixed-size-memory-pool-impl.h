#ifndef __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__
#define __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__

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
 *
 */

// INTERNAL INCLUES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/common/fixed-size-memory-pool.h>

namespace Dali
{

namespace Internal
{

class FixedSizeMemoryPool;

typedef IntrusivePtr<FixedSizeMemoryPool> FixedSizeMemoryPoolPtr;


/**
 * @brief Memory pool for a given fixed size of memory
 */
class FixedSizeMemoryPool : public BaseObject
{
public:

  typedef Dali::FixedSizeMemoryPool::SizeType SizeType;

public:

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @return Return the newly allocated memory pool object
   */
  static FixedSizeMemoryPoolPtr New( SizeType size );

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @param initialCapacity The initial size of the memory pool
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated
   * @return Return the newly allocated memory pool object
   */
  static FixedSizeMemoryPoolPtr New( SizeType size, SizeType initialCapacity, SizeType maximumBlockCapacity );

  /**
   * @brief Allocate a new fixed size block of memory
   *
   * @return Return the newly allocated memory
   */
  void* Allocate();

  /**
   * @brief Delete a block of memory for the allocation that has been allocated by this memory pool
   *
   * @param memory The memory to be deleted. Must have been allocated by this memory pool
   */
  void Free( void* memory );

private:

  /**
   * @brief Constructor.
   *
   * @param fixedSize The fixed size of each memory allocation
   * @param initialCapacity The initial size of the memory pool
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated
   */
  explicit FixedSizeMemoryPool( SizeType fixedSize, SizeType initialCapacity = 32, SizeType maximumBlockCapacity = 1048576 );

  /**
   * @brief Destructor.
   */
  ~FixedSizeMemoryPool();

  /**
   * @breif Allocate a new block for allocating memory from
   */
  void AllocateNewBlock();

private:

  /**
   * @brief Struct to represent a block of memory from which allocations can be made.
   *
   * The block forms a linked list.
   */
  struct Block
  {
    void* blockMemory;      ///< The allocated memory from which allocations can be made
    Block* nextBlock;       ///< The next block in the linked list

    /**
     * @brief Construct a new block with given size
     *
     * @param size The size of the memory block to allocate in bytes. Must be non-zero.
     */
    Block( SizeType size );

    /**
     * @brief Destructor
     */
    ~Block();
  };

private:

  SizeType mFixedSize;                ///< The size of each allocation in bytes

  Block mMemoryBlocks;                ///< Linked list of allocated memory blocks
  SizeType mMaximumBlockCapacity;     ///< The maximum allowed capacity of allocations in a new memory block

  Block* mCurrentBlock;               ///< Pointer to the active block
  SizeType mCurrentBlockCapacity;     ///< The maximum number of allocations that can be allocated for the current block
  SizeType mCurrentBlockSize;         ///< The number of allocations allocated to the current block

  void* mDeletedObjects;              ///< Pointer to the head of the list of deleted objects. The addresses are stored in the allocated memory blocks.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::FixedSizeMemoryPool& GetImplementation(Dali::FixedSizeMemoryPool& pool)
{
  DALI_ASSERT_ALWAYS( pool && "FixedSizeMemoryPool handle is empty" );

  BaseObject& handle = pool.GetBaseObject();

  return static_cast<Internal::FixedSizeMemoryPool&>(handle);
}

inline const Internal::FixedSizeMemoryPool& GetImplementation(const Dali::FixedSizeMemoryPool& pool)
{
  DALI_ASSERT_ALWAYS( pool && "FixedSizeMemoryPool handle is empty" );

  const BaseObject& handle = pool.GetBaseObject();

  return static_cast<const Internal::FixedSizeMemoryPool&>(handle);
}

} // namespace Dali

#endif /* __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__ */
