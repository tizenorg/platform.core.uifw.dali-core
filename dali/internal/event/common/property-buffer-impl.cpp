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
#include <dali/internal/event/common/property-buffer-impl.h>  // Dali::Internal::PropertyBuffer

// INTERNAL INCLUDES
#include <dali/public-api/object/property-buffer.h>     // Dali::Internal::PropertyBuffer
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name    |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "size",          UNSIGNED_INTEGER, true, false,  true,   Dali::PropertyBuffer::Property::SIZE )
DALI_PROPERTY( "buffer-format", MAP,              false, false, false,  Dali::PropertyBuffer::Property::BUFFER_FORMAT )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> PROPERTY_BUFFER_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace


unsigned int PropertyBuffer::FormatMetadata::GetComponentOffset( unsigned int index ) const
{
  DALI_ASSERT_DEBUG( index >= 0 && index < components.Size() && "Index not within the correct boundaries." );
  return index > 0 ? components[index-1u].accumulatedSize : 0;
}

unsigned int PropertyBuffer::FormatMetadata::GetElementSize() const
{
  unsigned int numComponents = components.Size();
  return numComponents ? components[-1].accumulatedSize : 0;
}

PropertyBufferPtr PropertyBuffer::New()
{
  return PropertyBufferPtr( new PropertyBuffer() );
}

void PropertyBuffer::SetSize( std::size_t size )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );

  mSize = size;

  SizeChanged();
}

std::size_t PropertyBuffer::GetSize() const
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );

  return 0;
}

void PropertyBuffer::SetData( void* data )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );
}

Dali::Property::Index PropertyBuffer::GetPropertyIndex( const std::string name, std::size_t index )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH_REWORK" );
  return 0;
}

const SceneGraph::PropertyBuffer* PropertyBuffer::GetPropertyBufferSceneObject() const
{
  return mSceneObject;
}

void PropertyBuffer::SetType( Dali::PropertyBuffer::Type type )
{
  DALI_ASSERT_DEBUG( !mTypeSet && "Type of property buffer can only be set once." );

  switch(type)
  {
    case  Dali::PropertyBuffer::STATIC:
    {
      mIsAnimatable = false;
      break;
    }
    case  Dali::PropertyBuffer::ANIMATABLE:
    {
      mIsAnimatable = true;
      break;
    }
  }

#ifdef DEBUG_ENABLED
  mTypeSet = true;
#endif // DEBUG_ENABLED
}

void PropertyBuffer::SetFormat( Dali::Property::Map& format )
{
  DALI_ASSERT_DEBUG( !mFormatSet && "Type of property buffer can only be set once." );

  mFormat = format;

  FormatChanged();

#ifdef DEBUG_ENABLED
  mFormatSet = true;
#endif // DEBUG_ENABLED
}


unsigned int PropertyBuffer::GetDefaultPropertyCount() const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyCount();
}

void PropertyBuffer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  PROPERTY_BUFFER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* PropertyBuffer::GetDefaultPropertyName(Property::Index index) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyName( index );
}

Property::Index PropertyBuffer::GetDefaultPropertyIndex( const std::string& name ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyIndex( name );
}

bool PropertyBuffer::IsDefaultPropertyWritable( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyWritable( index );
}

bool PropertyBuffer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool PropertyBuffer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type PropertyBuffer::GetDefaultPropertyType( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultPropertyType( index );
}

void PropertyBuffer::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  PROPERTY_BUFFER_IMPL.SetDefaultProperty( index, propertyValue );
}

void PropertyBuffer::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  PROPERTY_BUFFER_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value PropertyBuffer::GetDefaultProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetPropertyOwner() const
{
  return PROPERTY_BUFFER_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* PropertyBuffer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* PropertyBuffer::GetSceneObjectInputProperty( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetSceneObjectInputProperty( index );
}

int PropertyBuffer::GetPropertyComponentIndex( Property::Index index ) const
{
  return PROPERTY_BUFFER_IMPL.GetPropertyComponentIndex( index );
}

bool PropertyBuffer::OnStage() const
{
  return mOnStage;
}

void PropertyBuffer::Connect()
{
  mOnStage = true;
}

void PropertyBuffer::Disconnect()
{
  mOnStage = false;
}

PropertyBuffer::~PropertyBuffer()
{
}

PropertyBuffer::PropertyBuffer()
: mSceneObject( NULL ),
  mPropertyFormat( NULL ),
  mSize( 0 ),
  mIsAnimatable( false ),
  mOnStage( false )
#ifdef DEBUG_ENABLED
  , mTypeSet( false )
  , mFormatSet( false )
#endif
{
}

void PropertyBuffer::FormatChanged()
{
  size_t numComponents = mFormat.Count();

  // Create the format
  DALI_ASSERT_DEBUG( mPropertyFormat == NULL && "PropertyFormat should not be set yet" );
  FormatMetadata* propertyFormat = new FormatMetadata();
  propertyFormat->components.Resize( numComponents );

  unsigned int elementSize = 0;
  for( size_t i = 0u; i < numComponents; ++i )
  {
    StringValuePair component = mFormat.GetPair( i );

    // Get the name
    propertyFormat->components[i].name = component.first;

    // Get the size
    Property::Type type = Property::Type( component.second.Get<unsigned int>() );
    elementSize += GetPropertyImplementationSize( type );

    // write the accumulatedSize
    propertyFormat->components[i].accumulatedSize = elementSize;
  }

  // Store it for reference
  mPropertyFormat = propertyFormat;

  if( mSize )
  {
    SizeChanged();
  }
}

void PropertyBuffer::SizeChanged()
{
  // Check if format and size have been set yet
  if( !mPropertyFormat )
  {
    return;
  }

  unsigned int bufferSize = mPropertyFormat->GetElementSize() * mSize;
  mBuffer.Resize( bufferSize );

}

unsigned int GetPropertyImplementationSize( Property::Type& propertyType )
{
  unsigned int size = 0u;

  switch( propertyType )
  {
    case Property::NONE:
    case Property::TYPE_COUNT:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      DALI_ASSERT_ALWAYS( "No size for properties with no type, or dynamic sizes" );
      break;
    }
    case Property::BOOLEAN:
    {
      size = sizeof( PropertyImplementationType< Property::BOOLEAN >::Type );
      break;
    }
    case Property::FLOAT:
    {
      size = sizeof( PropertyImplementationType< Property::FLOAT >::Type );
      break;
    }
    case Property::INTEGER:
    {
      size = sizeof( PropertyImplementationType< Property::INTEGER >::Type );
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      size = sizeof( PropertyImplementationType< Property::UNSIGNED_INTEGER >::Type );
      break;
    }
    case Property::VECTOR2:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR2 >::Type );
      break;
    }
    case Property::VECTOR3:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR3 >::Type );
      break;
    }
    case Property::VECTOR4:
    {
      size = sizeof( PropertyImplementationType< Property::VECTOR4 >::Type );
      break;
    }
    case Property::MATRIX3:
    {
      size = sizeof( PropertyImplementationType< Property::MATRIX3 >::Type );
      break;
    }
    case Property::MATRIX:
    {
      size = sizeof( PropertyImplementationType< Property::MATRIX >::Type );
      break;
    }
    case Property::RECTANGLE:
    {
      size = sizeof( PropertyImplementationType< Property::RECTANGLE >::Type );
      break;
    }
    case Property::ROTATION:
    {
      size = sizeof( PropertyImplementationType< Property::ROTATION >::Type );
      break;
    }
  }

  return size;
}

} // namespace Internal
} // namespace Dali
