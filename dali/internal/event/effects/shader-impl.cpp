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
#include <dali/internal/event/effects/shader-impl.h> // Dali::Internal::Shader

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader.h> // Dali::Internal::Shader
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name            |type             |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "program",       MAP,              true, false,  false,  Dali::Shader::Property::PROGRAM )
DALI_PROPERTY( "shader-hints",  UNSIGNED_INTEGER, true, false,  true,   Dali::Shader::Property::SHADER_HINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> SHADER_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

ShaderPtr Shader::New()
{
  return ShaderPtr( new Shader() );
}

unsigned int Shader::GetDefaultPropertyCount() const
{
  return SHADER_IMPL.GetDefaultPropertyCount();
}

void Shader::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  SHADER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Shader::GetDefaultPropertyName(Property::Index index) const
{
  return SHADER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Shader::GetDefaultPropertyIndex( const std::string& name ) const
{
  return SHADER_IMPL.GetDefaultPropertyIndex( name );
}

bool Shader::IsDefaultPropertyWritable( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyWritable( index );
}

bool Shader::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Shader::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return SHADER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Shader::GetDefaultPropertyType( Property::Index index ) const
{
  return SHADER_IMPL.GetDefaultPropertyType( index );
}

void Shader::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  SHADER_IMPL.SetDefaultProperty( index, propertyValue );
}

void Shader::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  SHADER_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value Shader::GetDefaultProperty( Property::Index index ) const
{
  return SHADER_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* Shader::GetPropertyOwner() const
{
  return SHADER_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* Shader::GetSceneObject() const
{
  return SHADER_IMPL.GetSceneObject();
}

const SceneGraph::PropertyBase* Shader::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return SHADER_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* Shader::GetSceneObjectInputProperty( Property::Index index ) const
{
  return SHADER_IMPL.GetSceneObjectInputProperty( index );
}

int Shader::GetPropertyComponentIndex( Property::Index index ) const
{
  return SHADER_IMPL.GetPropertyComponentIndex( index );
}

} // namespace Internal
} // namespace Dali

