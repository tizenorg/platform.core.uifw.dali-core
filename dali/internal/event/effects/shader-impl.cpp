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
#include <dali/internal/event/effects/shader-impl.h>  // Dali::Internal::Shader

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader.h>      // Dali::Internal::Shader
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

/**
 *            |name            |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "program",       MAP,              true, false,  false,  Dali::Shader::Property::PROGRAM )
DALI_PROPERTY( "shader-hints",  UNSIGNED_INTEGER, true, false,  true,   Dali::Shader::Property::GEOMETRY_HINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )


ShaderPtr Shader::New()
{
  return ShaderPtr( new Shader() );
}


unsigned int Shader::GetDefaultPropertyCount() const
{
  //TODO: MESH_REWORK
  return 0;
}

void Shader::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  //TODO: MESH_REWORK
}

const char* Shader::GetDefaultPropertyName(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Index Shader::GetDefaultPropertyIndex(const std::string& name) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Shader::IsDefaultPropertyWritable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Shader::IsDefaultPropertyAnimatable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Shader::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Type Shader::GetDefaultPropertyType(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

void Shader::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  //TODO: MESH_REWORK
}

void Shader::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  //TODO: MESH_REWORK
}

Property::Value Shader::GetDefaultProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Shader::GetPropertyOwner() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Shader::GetSceneObject() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyBase* Shader::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const PropertyInputImpl* Shader::GetSceneObjectInputProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

int Shader::GetPropertyComponentIndex( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

} // namespace Internal
} // namespace Dali

