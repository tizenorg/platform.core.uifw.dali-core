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
#include <dali/public-api/object/property-buffer.h> // Dali::PropertyBuffer

// INTERNAL INCLUDES
#include <dali/public-api/object/property-map.h> // Dali::Property::Map
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer

namespace Dali
{

PropertyBuffer PropertyBuffer::New( Type type, Dali::Property::Map bufferFormat, std::size_t size )
{
  Internal::PropertyBufferPtr renderer = Internal::PropertyBuffer::New();
  return PropertyBuffer( renderer.Get() );
}

PropertyBuffer::PropertyBuffer()
{
}

PropertyBuffer::~PropertyBuffer()
{
}

PropertyBuffer::PropertyBuffer( const PropertyBuffer& handle )
: Handle( handle )
{
}

PropertyBuffer& PropertyBuffer::operator=( const PropertyBuffer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void PropertyBuffer::SetSize( std::size_t size )
{
  GetImplementation(*this).SetSize( size );
}

std::size_t PropertyBuffer::GetSize() const
{
  return  GetImplementation(*this).GetSize();
}

void PropertyBuffer::SetData( void* data )
{
  GetImplementation(*this).SetData( data );
}

Dali::Property::Index PropertyBuffer::GetPropertyIndex( const std::string name, std::size_t index )
{
  return GetImplementation(*this).GetPropertyIndex( name, index );
}

PropertyBuffer::PropertyBuffer( Internal::PropertyBuffer* pointer )
: Handle( pointer )
{
}

} // namespace Dali
