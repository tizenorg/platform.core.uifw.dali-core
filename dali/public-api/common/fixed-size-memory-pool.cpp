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

// CLASS HEADER
#include <dali/public-api/common/fixed-size-memory-pool.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/fixed-size-memory-pool-impl.h>

namespace Dali
{

FixedSizeMemoryPool FixedSizeMemoryPool::New( SizeType size )
{
  Internal::FixedSizeMemoryPoolObjectPtr internal = Internal::FixedSizeMemoryPoolObject::New( size );

  return FixedSizeMemoryPool( internal.Get() );
}

FixedSizeMemoryPool FixedSizeMemoryPool::New( SizeType size, SizeType initialCapacity, SizeType maximumBlockCapacity )
{
  Internal::FixedSizeMemoryPoolObjectPtr internal = Internal::FixedSizeMemoryPoolObject::New( size, initialCapacity, maximumBlockCapacity );

  return FixedSizeMemoryPool( internal.Get() );
}

FixedSizeMemoryPool::FixedSizeMemoryPool( Internal::FixedSizeMemoryPoolObject* internal )
: BaseHandle(internal)
{
}

FixedSizeMemoryPool FixedSizeMemoryPool::DownCast( BaseHandle handle )
{
  return FixedSizeMemoryPool( dynamic_cast<Dali::Internal::FixedSizeMemoryPoolObject*>( handle.GetObjectPtr() ) );
}

FixedSizeMemoryPool::FixedSizeMemoryPool()
{
}

FixedSizeMemoryPool::~FixedSizeMemoryPool()
{
}

FixedSizeMemoryPool::FixedSizeMemoryPool( const FixedSizeMemoryPool& copy )
: BaseHandle(copy)
{
}

FixedSizeMemoryPool& FixedSizeMemoryPool::operator=( const FixedSizeMemoryPool& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

void* FixedSizeMemoryPool::Allocate()
{
  return GetImplementation(*this).GetPool()->Allocate();
}

void FixedSizeMemoryPool::Free( void* memory )
{
  GetImplementation(*this).GetPool()->Free( memory );
}

} // namespace Dali
