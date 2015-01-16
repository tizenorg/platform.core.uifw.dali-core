#ifndef __DALI_FIXED_SIZE_MEMORY_POOL_H__
#define __DALI_FIXED_SIZE_MEMORY_POOL_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class FixedSizeMemoryPool;
}

/**
 * @brief Memory pool for a given fixed size of memory
 */
class DALI_IMPORT_API FixedSizeMemoryPool : public BaseHandle
{
public:

  /**
   * @brief Create an uninitialized FixedSizeMemoryPool handle.
   *
   * Initialise it using FixedSizeMemoryPool::New().  Calling member functions
   * with an uninitialized Dali::Object is not allowed.
   */
  FixedSizeMemoryPool();

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @return Return the newly allocated memory pool object
   */
  static FixedSizeMemoryPool New( uint32_t size );

  /**
   * @brief Create a new fixed size memory pool
   *
   * @param size The size of each allocation
   * @param initialCapacity The initial size of the memory pool
   * @param maximumBlockCapacity The maximum size that a new block of memory can be allocated
   * @return Return the newly allocated memory pool object
   */
  static FixedSizeMemoryPool New( uint32_t size, uint32_t initialCapacity, uint32_t maximumBlockCapacity );


  /**
   * @brief Downcast an Object handle to CameraActor.
   *
   * If handle points to a CameraActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a CameraActor or an uninitialized handle
   */
  static FixedSizeMemoryPool DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FixedSizeMemoryPool();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  FixedSizeMemoryPool(const FixedSizeMemoryPool& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  FixedSizeMemoryPool& operator=(const FixedSizeMemoryPool& rhs);

  /**
   * @brief Allocate a new fixed size block of memory
   *
   * @return Return the newly allocated memory
   */
  void* NewAllocation();

  /**
   * @brief Delete a block of memory for the allocation that has been allocated by this memory pool
   *
   * @param memory The memory to be deleted. Must have been allocated by this memory pool
   */
  void DeleteAllocation( void* memory );

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] memoryPool A pointer to a newly allocated memory pool
   */
  explicit DALI_INTERNAL FixedSizeMemoryPool( Internal::FixedSizeMemoryPool* memoryPool );

};

/**
 * @brief Helper for allocating/deallocating objects
 */
template< typename T >
class DALI_IMPORT_API FixedSizeObjectAllocator
{
public:

  /**
   * @brief Constructor
   */
  FixedSizeObjectAllocator()
  {
    ResetMemoryPool();
  }

  /**
   * @brief Allocate from the memory pool
   *
   * @return Return the allocated object
   */
  T* New()
  {
    return new ( mPool.NewAllocation() ) T();
  }

  /**
   * @brief Return the object to the memory pool
   *
   * @param object Pointer to the object to delete
   */
  void Delete( T* object )
  {
    object->~T();

    mPool.DeleteAllocation( object );
  }

  /**
   * @brief Reset the memory pool, unloading all block memory previously allocated
   */
  void ResetMemoryPool()
  {
    mPool = FixedSizeMemoryPool::New( TypeSizeWithAlignment< T >::size );
  }

private:

  FixedSizeMemoryPool mPool;      ///< Handle to memory pool from which allocations are made

};

} // namespace Dali

#endif /* __DALI_FIXED_SIZE_MEMORY_POOL_H__ */
