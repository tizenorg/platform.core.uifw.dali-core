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
#include <dali/internal/event/effects/material-impl.h>  // Dali::Internal::Material

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/material.h>     // Dali::Internal::Material
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

/**
 *            |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "color",                           VECTOR4,  true, true,   true, Dali::Material::Property::COLOR )
DALI_PROPERTY( "face-culling-mode",               STRING,   true, false,  true, Dali::Material::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blending-mode",                   STRING,   true, false,  true, Dali::Material::Property::BLENDING_MODE )
DALI_PROPERTY( "blend-equation",                  STRING,   true, false,  true, Dali::Material::Property::BLEND_EQUATION )
DALI_PROPERTY( "source-blend-factor-rgb",         STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_RGB )
DALI_PROPERTY( "destination-blend-factor-rgb",    STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_RGB )
DALI_PROPERTY( "source-blend-factor-alpha",       STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA )
DALI_PROPERTY( "destination-blend-factor-alpha",  STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA )
DALI_PROPERTY( "blend-color",                     VECTOR4,  true, true,   true, Dali::Material::Property::BLEND_COLOR )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )


MaterialPtr Material::New()
{
  return MaterialPtr( new Material() );
}

unsigned int Material::GetDefaultPropertyCount() const
{
  //TODO: MESH_REWORK
  return 0;
}

void Material::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  //TODO: MESH_REWORK
}

const char* Material::GetDefaultPropertyName(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Index Material::GetDefaultPropertyIndex(const std::string& name) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Material::IsDefaultPropertyWritable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Material::IsDefaultPropertyAnimatable(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

bool Material::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

Property::Type Material::GetDefaultPropertyType(Property::Index index) const
{
  //TODO: MESH_REWORK
  return 0;
}

void Material::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  //TODO: MESH_REWORK
}

void Material::SetSceneGraphProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  //TODO: MESH_REWORK
}

Property::Value Material::GetDefaultProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Material::GetPropertyOwner() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyOwner* Material::GetSceneObject() const
{
  //TODO: MESH_REWORK
  return 0;
}

const SceneGraph::PropertyBase* Material::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  //TODO: MESH_REWORK
  return 0;
}

} // namespace Internal
} // namespace Dali

