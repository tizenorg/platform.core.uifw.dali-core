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

class FixedSizeMemoryPoolObject;

typedef IntrusivePtr<FixedSizeMemoryPoolObject> FixedSizeMemoryPoolObjectPtr;


/**
 * @brief Memory pool for a given fixed size of memory
 */
class FixedSizeMemoryPool
{
public:

  typedef Dali::FixedSizeMemoryPool::SizeType SizeType;

public:

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

  struct Impl;
  Impl* mImpl;

};

/**
 * @brief Memory pool object wrapper
 */
class FixedSizeMemoryPoolObject : public BaseObject
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
  static FixedSizeMemoryPoolObjectPtr New( SizeType size );

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @param initialCapacity The initial size of the memory pool
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated
   * @return Return the newly allocated memory pool object
   */
  static FixedSizeMemoryPoolObjectPtr New( SizeType size, SizeType initialCapacity, SizeType maximumBlockCapacity );

  /**
   * @brief Retrieve the inner pool object
   *
   * @return Return the inner pool object
   */
  FixedSizeMemoryPool* GetPool();

private:

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @return Return the newly allocated memory pool object
   */
  FixedSizeMemoryPoolObject( SizeType size );

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @param initialCapacity The initial size of the memory pool
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated
   * @return Return the newly allocated memory pool object
   */
  FixedSizeMemoryPoolObject( SizeType size, SizeType initialCapacity, SizeType maximumBlockCapacity );

  /**
   * Destructor
   */
  ~FixedSizeMemoryPoolObject();

private:

  FixedSizeMemoryPool* mPool;   ///< Memory pool instance

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::FixedSizeMemoryPoolObject& GetImplementation(Dali::FixedSizeMemoryPool& pool)
{
  DALI_ASSERT_ALWAYS( pool && "FixedSizeMemoryPool handle is empty" );

  BaseObject& handle = pool.GetBaseObject();

  return static_cast<Internal::FixedSizeMemoryPoolObject&>(handle);
}

inline const Internal::FixedSizeMemoryPoolObject& GetImplementation(const Dali::FixedSizeMemoryPool& pool)
{
  DALI_ASSERT_ALWAYS( pool && "FixedSizeMemoryPool handle is empty" );

  const BaseObject& handle = pool.GetBaseObject();

  return static_cast<const Internal::FixedSizeMemoryPoolObject&>(handle);
}

} // namespace Dali

#endif /* __DALI_INTERNAL_FIXED_SIZE_MEMORY_POOL_H__ */
