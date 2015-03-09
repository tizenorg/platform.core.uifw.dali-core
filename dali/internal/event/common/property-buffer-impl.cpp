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
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

/**
 *            |name    |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "size",  UNSIGNED_INTEGER, true, false,  true, Dali::PropertyBuffer::Property::SIZE )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )



PropertyBufferPtr PropertyBuffer::New()
{
  return PropertyBufferPtr( new PropertyBuffer() );
}


unsigned int PropertyBuffer::GetDefaultPropertyCount() const
{
  //TODO: MESH_REWORK
  return 0;
}

void PropertyBuffer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  //TODO: MESH_REWORK
}

const char* PropertyBuffer::GetDefaultPropertyName(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Index PropertyBuffer::GetDefaultPropertyIndex(const std::string& name) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool PropertyBuffer::IsDefaultPropertyWritable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool PropertyBuffer::IsDefaultPropertyAnimatable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool PropertyBuffer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Type PropertyBuffer::GetDefaultPropertyType(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

void PropertyBuffer::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  //TODO: MESH_REWORK
}

void PropertyBuffer::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  //TODO: MESH_REWORK
}

Property::Value PropertyBuffer::GetDefaultProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetPropertyOwner() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* PropertyBuffer::GetSceneObject() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyBase* PropertyBuffer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const PropertyInputImpl* PropertyBuffer::GetSceneObjectInputProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

int PropertyBuffer::GetPropertyComponentIndex( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

} // namespace Internal
} // namespace Dali

